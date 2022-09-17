#ifndef __StarPointModule_h
#define __StarPointModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StarPointModule : public MetaModule
{
public:

   StarPointModule();

   const char* Version() const override;
   IsoString Name() const override;
   String Description() const override;
   String Company() const override;
   String Author() const override;
   String Copyright() const override;
   String TradeMarks() const override;
   String OriginalFileName() const override;
   void GetReleaseDate( int& year, int& month, int& day ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SandboxModule_h

// ----------------------------------------------------------------------------
// EOF SandboxModule.h - Released 2022-05-17T17:15:11Z
