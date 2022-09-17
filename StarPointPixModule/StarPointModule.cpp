#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2022
#define MODULE_RELEASE_MONTH     5
#define MODULE_RELEASE_DAY       17

#include "StarPointModule.h"
#include "StarPointProcess.h"
#include "StarPointInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

StarPointModule::StarPointModule()
{
}

// ----------------------------------------------------------------------------

const char* StarPointModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString StarPointModule::Name() const
{
   return "StarPoint";
}

// ----------------------------------------------------------------------------

String StarPointModule::Description() const
{
   return "StarPoint Process Module";
}

// ----------------------------------------------------------------------------

String StarPointModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String StarPointModule::Author() const
{
   return "Riccardo Alberghi";
}

// ----------------------------------------------------------------------------

String StarPointModule::Copyright() const
{
   return "ciaooo";
}

// ----------------------------------------------------------------------------

String StarPointModule::TradeMarks() const
{
   return "ciaooo";
}

// ----------------------------------------------------------------------------

String StarPointModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "StarPoint-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "StarPoint-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "StarPoint-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "StarPoint-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void StarPointModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::StarPointModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::StarPointProcess;
      new pcl::StarPointInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF SandboxModule.cpp - Released 2022-05-17T17:15:11Z
