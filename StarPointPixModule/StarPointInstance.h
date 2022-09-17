#ifndef __StarPointInstance_h
#define __StarPointInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StarPointInstance : public ProcessImplementation
{
public:

   StarPointInstance( const MetaProcess* );
   StarPointInstance( const StarPointInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   float    amplitude_mul;
   float    std_mul;
   pcl_bool sync_std;
   pcl_bool deleteRotation;
   pcl_bool deleteSpikes;

   friend class StarPointEngine;
   friend class StarPointProcess;
   friend class StarPointInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __StarPointInstance_h
