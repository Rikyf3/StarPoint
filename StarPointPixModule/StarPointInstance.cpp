#include "StarPointInstance.h"
#include "StarPointParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>
#include <pcl/ImageWindow.h>
#include <pcl/Settings.h>

#include <tensorflow/c/c_api.h>

#define MAX_STD 24
#define DOWNSAMPLE 8
#define OUTPUT_TILE_SIZE 16
#define OUTPUT_CHANNEL 6
#define TILE_SIZE 128

namespace pcl
{

// ----------------------------------------------------------------------------

StarPointInstance::StarPointInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , amplitude_mul( TheStarPointAmplitude->DefaultValue() )
   , std_mul( TheStarPointDeviation->DefaultValue() )
   , sync_std( TheStarPointSyncDeviations->DefaultValue() )
   , deleteRotation( TheStarPointRemoveRotation->DefaultValue() )
   , deleteSpikes( TheStarPointRemoveDifractionSpikes->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

StarPointInstance::StarPointInstance( const StarPointInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void StarPointInstance::Assign( const ProcessImplementation& p )
{
   const StarPointInstance* x = dynamic_cast<const StarPointInstance*>( &p );
   if ( x != nullptr )
   {
      amplitude_mul   = x->amplitude_mul;
      std_mul   = x->std_mul;
      sync_std  = x->sync_std;
      deleteRotation  = x->deleteRotation;
      deleteSpikes = x->deleteSpikes;
   }
}

// ----------------------------------------------------------------------------

UndoFlags StarPointInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool StarPointInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "StarPointInstance cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().Height() < 128 || view.Image().Width() < 128 )
   {
      whyNot = "Image size smaller than 128.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

class StarPointEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const StarPointInstance& instance )
   {
      int finalHeight = TILE_SIZE * Ceil( image.Height() / (double) TILE_SIZE );
      int finalWidth = TILE_SIZE * Ceil( image.Width() / (double) TILE_SIZE );

      GenericImage<P> ceiled_image( finalWidth, finalHeight, image.ColorSpace() );
      GenericImage<P> star_mask( finalWidth, finalHeight, image.ColorSpace() );
      star_mask.Black();
      star_mask.EnableRangeClipping();
      ceiled_image.Black();
      ceiled_image.EnableRangeClipping();

      for ( int c = 0; c < image.NumberOfNominalChannels(); c++ )
      {
         for ( int i = 0; i < finalWidth; i++ )
         {
            for ( int j = 0; j < image.Height(); j++ )
            {
               if ( (i < image.Width()) && (j < image.Height()) )
                  ceiled_image(i, j, c) = image(i, j, c);
               else
                  ceiled_image(i, j, c) = P::FloatToSample( 0.0 );
            }

         }
      }

      Console console;

      console.WriteLn( "Loading neural network" );
      TF_Graph* graph = LoadGraphDef( "/Applications/PixInsight/library/simple_frozen_graph.pb" );

      StandardStatus callbackNN;
      StatusMonitor monitorNN;
      monitorNN.SetCallback( &callbackNN );
      monitorNN.Initialize( "Processing: ", image.NumberOfNominalChannels()*(finalHeight*finalWidth) / (TILE_SIZE*TILE_SIZE) );

      TF_Output inputOp = { TF_GraphOperationByName( graph, "inputs" ), 0 };
      TF_Output outOp = { TF_GraphOperationByName( graph, "model_5/tf.math.sigmoid_5/Sigmoid" ), 0 };
      // Input dimensions
      const int64_t inputDims[ 4 ] = { 1, TILE_SIZE, TILE_SIZE, 1 };
      // Creating all necessary tensors and session for TF.
      TF_SessionOptions* options = TF_NewSessionOptions();
      TF_Status* status = TF_NewStatus();
      TF_Session* sess = TF_NewSession( graph, options, status );

      try
      {
         console.EnableAbort();

         for ( unsigned int i = 0; i < finalWidth; i += TILE_SIZE )
         {
            for ( unsigned int j = 0; j < finalHeight; j += TILE_SIZE )
            {
               GenericImage<P> amplitude( OUTPUT_TILE_SIZE, OUTPUT_TILE_SIZE, image.ColorSpace() );
               amplitude.Black();
               GenericImage<P> x_fine( OUTPUT_TILE_SIZE, OUTPUT_TILE_SIZE, image.ColorSpace() );
               x_fine.Black();
               GenericImage<P> y_fine( OUTPUT_TILE_SIZE, OUTPUT_TILE_SIZE, image.ColorSpace() );
               y_fine.Black();
               GenericImage<P> x_std( OUTPUT_TILE_SIZE, OUTPUT_TILE_SIZE, image.ColorSpace() );
               x_std.Black();
               GenericImage<P> y_std( OUTPUT_TILE_SIZE, OUTPUT_TILE_SIZE, image.ColorSpace() );
               y_std.Black();
               GenericImage<P> theta( OUTPUT_TILE_SIZE, OUTPUT_TILE_SIZE, image.ColorSpace() );
               theta.Black();

               for ( unsigned int c = 0; c < image.NumberOfNominalChannels(); ++c )
               {
                  //
                  Array<float> inputVals;
                  for ( int w = i; w < i + TILE_SIZE; ++w )
                  {
                     for ( int k = j; k < j + TILE_SIZE; ++k )
                     {
                        float f;
                        P::FromSample( f, ceiled_image( w, k, c ) );
                        inputVals << f;
                     }
                  }

                  TF_Tensor* inputTensor = CreateTensor( TF_FLOAT, inputDims, 4, inputVals.Begin(), inputVals.Size() );
                  TF_Tensor* outputTensor = nullptr;

                  TF_SessionRun( sess, nullptr,           // run options
                           &inputOp, &inputTensor, 1,     // input tensors, input tensor values, number of inputs
                           &outOp, &outputTensor, 1,      // output tensors, output tensor values, number of outputs
                           nullptr, 0, nullptr, status ); // target operations, number of targets, run metadata, output status

                  float* data = reinterpret_cast<float*>( TF_TensorData( outputTensor ) );

                  for ( int u = 0; u < OUTPUT_TILE_SIZE; ++u )
                  {
                     for ( int v = 0; v < OUTPUT_TILE_SIZE; ++v)
                     {
                        amplitude( u, v, c ) = P::ToSample( Range( instance.amplitude_mul * data[OUTPUT_CHANNEL * (u * OUTPUT_TILE_SIZE + v)], 0.0F, 1.0F ) ); //
                        x_fine( u, v, c ) = P::ToSample( Range( data[1 + OUTPUT_CHANNEL * (u * OUTPUT_TILE_SIZE + v)], 0.0F, 1.0F ) );
                        y_fine( u, v, c ) = P::ToSample( Range( data[2 + OUTPUT_CHANNEL * (u * OUTPUT_TILE_SIZE + v)], 0.0F, 1.0F ) );
                        x_std( u, v, c ) = P::ToSample( Range( instance.std_mul * data[3 + OUTPUT_CHANNEL * (u * OUTPUT_TILE_SIZE + v)], 0.0F, 1.0F ) );
                        y_std( u, v, c ) = P::ToSample( Range( instance.std_mul * data[4 + OUTPUT_CHANNEL * (u * OUTPUT_TILE_SIZE + v)], 0.0F, 1.0F ) );
                        theta( u, v, c ) = P::ToSample( Range( data[5 + OUTPUT_CHANNEL * (u * OUTPUT_TILE_SIZE + v)], 0.0F, 1.0F ) );
                     }
                  }

                  TF_DeleteTensor( inputTensor );
                  TF_DeleteTensor( outputTensor );

                  ++monitorNN;
               }

               //render
               int x_mean, y_mean;
               int bbox;
               float temp;

               for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
               {
                  for ( int u = 0; u < OUTPUT_TILE_SIZE; ++u )
                  {
                     for ( int v = 0; v < OUTPUT_TILE_SIZE; ++v)
                     {
                        if ( Get( amplitude, u, v, c) < 0.05 )
                           continue;

                        x_mean = i + DOWNSAMPLE*u + Floor( int( DOWNSAMPLE * GetMeanOverChannels( y_fine, u, v ) ) );
                        y_mean = j + DOWNSAMPLE*v + Floor( int( DOWNSAMPLE * GetMeanOverChannels( x_fine, u, v ) ) );
                        bbox = 3 * (int) Ceil( Sqrt( MAX_STD * Max( Get( x_std, u, v, c ), Get( y_std, u, v, c ) ) ) );

                        for ( int x = Max( x_mean - bbox, 0 ); x < Min( x_mean + bbox, finalWidth ); ++x )
                        {
                           for ( int y = Max( y_mean - bbox, 0 ); y < Min( y_mean + bbox, finalHeight ); ++y )
                           {
                              P::FromSample( temp, star_mask( x, y, c ) );
                              star_mask( x, y, c ) += P::ToSample(
                                 Range(
                                    Gaussian(
                                       x,
                                       y,
                                       Get( amplitude, u, v, c ),
                                       x_mean,
                                       y_mean,
                                       ( instance.sync_std ? GetMeanOverChannels( x_std, u, v ) : Get( x_std, u, v, c ) ),
                                       ( instance.sync_std ? GetMeanOverChannels( y_std, u, v ) : Get( y_std, u, v, c ) ),
                                       ( instance.deleteRotation ? 0.0 : GetMeanOverChannels( theta, u, v ) )
                                    ),
                                    0.0F,
                                    1.0F - temp
                                 )
                              );
                           }
                        }
                     }
                  }
               }
            }
         }

         star_mask.CropBy( 0, 0, image.Width()-finalWidth, image.Height()-finalHeight );
         image = star_mask;

         TF_CloseSession( sess, status );
         TF_DeleteSession( sess, status );
         TF_DeleteSessionOptions( options );
         TF_DeleteGraph( graph );
         TF_DeleteStatus( status );
      }
      catch ( ... )
      {
         TF_CloseSession( sess, status );
         TF_DeleteSession( sess, status );
         TF_DeleteSessionOptions( options );
         TF_DeleteGraph( graph );
         TF_DeleteStatus( status );
         throw;
      }

   }

private:
   static TF_Tensor* CreateTensor( TF_DataType dataType, const int64_t* dims, int num_dims, const void* data, size_type size )
   {
      TF_Tensor* tensor = TF_AllocateTensor( dataType, dims, num_dims, size );
      if ( tensor == nullptr )
         throw Error( "Failure to allocate tensor" );
      void* tensorData = TF_TensorData( tensor );
      if ( tensorData == nullptr )
      {
         TF_DeleteTensor( tensor );
         throw Error( "No tensor data available" );
      }
      ::memcpy( tensorData, data, Min( size, TF_TensorByteSize( tensor ) ) );
      return tensor;
   }

   static TF_Graph* LoadGraphDef( const String& filePath )
   {
      TF_Buffer* buffer = ReadBufferFromFile( filePath );
      TF_Graph* graph = TF_NewGraph();
      TF_Status* status = TF_NewStatus();
      TF_ImportGraphDefOptions* options = TF_NewImportGraphDefOptions();
      TF_GraphImportGraphDef( graph, buffer, options, status );
      TF_DeleteImportGraphDefOptions( options );
      TF_DeleteBuffer( buffer );

      if ( TF_GetCode( status ) != TF_OK )
      {
         TF_DeleteGraph( graph );
         throw Error( TF_Message( status ) );
      }

      TF_DeleteStatus( status );

      return graph;
   }

   static TF_Buffer* ReadBufferFromFile( const String& filePath )
   {
      ByteArray data = File::ReadFile( filePath );
      TF_Buffer* buffer = TF_NewBuffer();
      buffer->length = data.Size();
      buffer->data = data.Release();
      buffer->data_deallocator = DeallocateBuffer;
      return buffer;
   }

   static void DeallocateBuffer( void* data, size_t )
   {
      StandardAllocator().DeallocateBlock( data );
   }

   template <typename P>
   static float GetMeanOverChannels( GenericImage<P> img, int u, int v )
   {
      float f;
      float sum = 0.0;

      for ( unsigned int c = 0; c < img.NumberOfNominalChannels(); ++c )
      {
         P::FromSample( f, img( u, v, c ) );
         sum += f;
      }

      return sum / ( float ) img.NumberOfNominalChannels();
   }

   template<typename P>
   static float Get( GenericImage<P> img, int u, int v, int c )
   {
      float f; P::FromSample( f, img( u, v, c ) );
      return f;
   }

   static float Square( float x )
   {
      return PowI( x, 2 );
   }

   static float Gaussian( int pos_x, int pos_y, float amplitude, float x_mean,  float y_mean, float x_std, float y_std, float theta )
   {
      x_std = MAX_STD * 0.5 * ( x_std + y_std );
      y_std = x_std;

      theta = theta * 3.14F;
      float a = Pow( (float) Cos( theta ), 2.0F ) / ( x_std ) + Pow( (float) Sin( theta ), 2.0F ) / ( y_std );
      float b = Sin( 2.0F * theta ) / x_std - Sin( 2.0F * theta ) / y_std;
      float c = Pow( (float) Sin( theta ), 2.0F ) / ( x_std ) + Pow( (float) Cos( theta ), 2.0F ) / ( y_std );

      return amplitude * Exp( -a * Pow( pos_x - x_mean, 2.0F ) -b * ( pos_x - x_mean ) * ( pos_y - y_mean ) -c * Pow( pos_y - y_mean, 2.0F ) );
   }
};

// ----------------------------------------------------------------------------

bool StarPointInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: StarPointEngine::Apply( static_cast<Image&>( *image ), *this ); break;
      case 64: StarPointEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: StarPointEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
      case 16: StarPointEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
      case 32: StarPointEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void* StarPointInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheStarPointAmplitude )
      return &amplitude_mul;
   if ( p == TheStarPointDeviation )
      return &std_mul;
   if ( p == TheStarPointSyncDeviations )
      return &sync_std;
   if ( p == TheStarPointRemoveRotation )
      return &deleteRotation;
   if ( p == TheStarPointRemoveDifractionSpikes)
      return &deleteSpikes;

   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxInstance.cpp - Released 2022-05-17T17:15:11Z
