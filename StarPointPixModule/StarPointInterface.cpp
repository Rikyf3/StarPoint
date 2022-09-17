#include "StarPointInterface.h"
#include "StarPointParameters.h"
#include "StarPointProcess.h"

#include<pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarPointInterface* TheStarPointInterface = nullptr;

// ----------------------------------------------------------------------------

StarPointInterface::StarPointInterface()
   : m_instance( TheStarPointProcess )
{
   TheStarPointInterface = this;
}

// ----------------------------------------------------------------------------

StarPointInterface::~StarPointInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString StarPointInterface::Id() const
{
   return "StarPoint";
}

// ----------------------------------------------------------------------------

MetaProcess* StarPointInterface::Process() const
{
   return TheStarPointProcess;
}

// ----------------------------------------------------------------------------

String StarPointInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/Sandbox.svg";
}

// ----------------------------------------------------------------------------

void StarPointInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void StarPointInterface::ResetInstance()
{
   StarPointInstance defaultInstance( TheStarPointProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool StarPointInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "StarPoint" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheStarPointProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarPointInterface::NewProcess() const
{
   return new StarPointInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool StarPointInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const StarPointInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a StarPoint instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool StarPointInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool StarPointInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void StarPointInterface::UpdateControls()
{
   GUI->Amplitude_NumericControl.SetValue( m_instance.amplitude_mul );
   GUI->Dev_NumericControl.SetValue( m_instance.std_mul );
   GUI->SyncDev_CheckBox.SetChecked( m_instance.sync_std );
   GUI->DeleteRotation_CheckBox.SetChecked( m_instance.deleteRotation );
   GUI->DeleteSpikes_CheckBox.SetChecked( m_instance.deleteSpikes );
}

// ----------------------------------------------------------------------------

void StarPointInterface::e_RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Amplitude_NumericControl )
      m_instance.amplitude_mul = value;
   if ( sender == GUI->Dev_NumericControl )
      m_instance.std_mul = value;
}

// ----------------------------------------------------------------------------

void StarPointInterface::e_ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->SyncDev_CheckBox )
      m_instance.sync_std = checked;
   if ( sender == GUI->DeleteRotation_CheckBox )
      m_instance.deleteRotation = checked;
   if ( sender == GUI->DeleteSpikes_CheckBox )
      m_instance.deleteSpikes = checked;

   UpdateControls();
}

// ----------------------------------------------------------------------------

StarPointInterface::GUIData::GUIData( StarPointInterface& w )
{
   pcl::Font font = w.Font();
   int labelWidth1 = font.Width( String( "Amplitude multiplier: " ) ); // the longest label text
   int editWidth1 = font.Width( String( '0', 7 ) );

   //

   Amplitude_NumericControl.label.SetText( "Amplitude multiplier:" );
   Amplitude_NumericControl.label.SetFixedWidth( labelWidth1 );
   Amplitude_NumericControl.slider.SetScaledMinWidth( 250 );
   Amplitude_NumericControl.slider.SetRange( 0, 100 );
   Amplitude_NumericControl.SetReal();
   Amplitude_NumericControl.SetRange( TheStarPointAmplitude->MinimumValue(), TheStarPointAmplitude->MaximumValue() );
   Amplitude_NumericControl.SetPrecision( TheStarPointAmplitude->Precision() );
   Amplitude_NumericControl.SetToolTip( "<p>Multipling factor of the brightness of the stars</p>" );
   Amplitude_NumericControl.edit.SetFixedWidth( editWidth1 );
   Amplitude_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&StarPointInterface::e_RealValueUpdated, w );

   //

   Dev_NumericControl.label.SetText( "Deviation multiplier:" );
   Dev_NumericControl.label.SetFixedWidth( labelWidth1 );
   Dev_NumericControl.slider.SetScaledMinWidth( 250 );
   Dev_NumericControl.slider.SetRange( 0, 100 );
   Dev_NumericControl.SetReal();
   Dev_NumericControl.SetRange( TheStarPointDeviation->MinimumValue(), TheStarPointDeviation->MaximumValue() );
   Dev_NumericControl.SetPrecision( TheStarPointDeviation->Precision() );
   Dev_NumericControl.SetToolTip( "<p>Multipling factor of the deviation of the stars</p>" );
   Dev_NumericControl.edit.SetFixedWidth( editWidth1 );
   Dev_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&StarPointInterface::e_RealValueUpdated, w );

   //

   SyncDev_CheckBox.SetText( "Correct cromatic aberration" );
   SyncDev_CheckBox.SetToolTip( "<p>Syncs variance over color channels</p>" );
   SyncDev_CheckBox.OnClick( (pcl::Button::click_event_handler)&StarPointInterface::e_ItemClicked, w );

   SyncDevSizer.Add( SyncDev_CheckBox );
   SyncDevSizer.AddStretch();

   //

   //DeleteRotation_CheckBox.Enable( false );
   DeleteRotation_CheckBox.SetText( "Remove rotation" );
   DeleteRotation_CheckBox.SetToolTip( "<p>Remove orientation in stars</p>" );
   DeleteRotation_CheckBox.OnClick( (pcl::Button::click_event_handler)&StarPointInterface::e_ItemClicked, w );

   DeleteRotationSizer.Add( DeleteRotation_CheckBox );
   DeleteRotationSizer.AddStretch();

   //

   DeleteSpikes_CheckBox.Enable( false ); //TheStarPointInterface->dayTrialLeft != 0
   DeleteSpikes_CheckBox.SetText( "Remove spikes" );
   DeleteSpikes_CheckBox.SetToolTip( "<p>Remove difraction spikes in stars</p>" );
   DeleteSpikes_CheckBox.OnClick( (pcl::Button::click_event_handler)&StarPointInterface::e_ItemClicked, w );

   DeleteSpikesSizer.Add( DeleteSpikes_CheckBox );
   DeleteSpikesSizer.AddStretch();

   //

   GlobalSizer.SetMargin( 8 );
   GlobalSizer.SetSpacing( 6 );
   GlobalSizer.Add( Amplitude_NumericControl );
   GlobalSizer.Add( Dev_NumericControl );
   GlobalSizer.Add( SyncDevSizer );
   GlobalSizer.Add( DeleteRotationSizer );
   GlobalSizer.Add( DeleteSpikesSizer );

   w.SetSizer( GlobalSizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxInterface.cpp - Released 2022-05-17T17:15:11Z
