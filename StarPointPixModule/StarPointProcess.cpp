#include "StarPointInstance.h"
#include "StarPointInterface.h"
#include "StarPointParameters.h"
#include "StarPointProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarPointProcess* TheStarPointProcess = nullptr;

// ----------------------------------------------------------------------------

StarPointProcess::StarPointProcess()
{
   TheStarPointProcess = this;

   new StarPointAmplitude( this );
   new StarPointDeviation( this );
   new StarPointSyncDeviations( this );
   new StarPointRemoveRotation( this );
   new StarPointRemoveDifractionSpikes( this );
}

// ----------------------------------------------------------------------------

IsoString StarPointProcess::Id() const
{
   return "StarPoint";
}

// ----------------------------------------------------------------------------

IsoString StarPointProcess::Category() const
{
   return "Render";
}

// ----------------------------------------------------------------------------

uint32 StarPointProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String StarPointProcess::Description() const
{
   return
   "<html>"
   "<p>"
   "Sandbox is just a starting point for development of new PixInsight "
   "modules. It is an empty module that does nothing but to provide the basic "
   "structure of a module with a process, a few parameters, and a simple "
   "process interface."
   "</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String StarPointProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Sandbox.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* StarPointProcess::DefaultInterface() const
{
   return TheStarPointInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarPointProcess::Create() const
{
   return new StarPointInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarPointProcess::Clone( const ProcessImplementation& p ) const
{
   const StarPointInstance* instance = dynamic_cast<const StarPointInstance*>( &p );
   return (instance != nullptr) ? new StarPointInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

bool StarPointProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Sandbox [<arg_list>] [<view_list>]"
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int StarPointProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   StarPointInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( const Argument& arg : arguments )
   {
      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         // These are standard parameters that all processes should provide.
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
            throw Error( "No view(s) found: " + arg.Token() );

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxProcess.cpp - Released 2022-05-17T17:15:11Z
