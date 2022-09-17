#include "StarPointParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

StarPointAmplitude*   TheStarPointAmplitude = nullptr;
StarPointDeviation*   TheStarPointDeviation = nullptr;
StarPointSyncDeviations*  TheStarPointSyncDeviations = nullptr;
StarPointRemoveRotation*  TheStarPointRemoveRotation = nullptr;
StarPointRemoveDifractionSpikes* TheStarPointRemoveDifractionSpikes = nullptr;

// ----------------------------------------------------------------------------

StarPointAmplitude::StarPointAmplitude( MetaProcess* P ) : MetaFloat( P )
{
   TheStarPointAmplitude = this;
}

IsoString StarPointAmplitude::Id() const
{
   return "amplitude";
}

int StarPointAmplitude::Precision() const
{
   return 2;
}

double StarPointAmplitude::DefaultValue() const
{
   return 1;
}

double StarPointAmplitude::MinimumValue() const
{
   return 0;
}

double StarPointAmplitude::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

StarPointDeviation::StarPointDeviation( MetaProcess* P ) : MetaFloat( P )
{
   TheStarPointDeviation = this;
}

IsoString StarPointDeviation::Id() const
{
   return "std_mul";
}

int StarPointDeviation::Precision() const
{
   return 2;
}

double StarPointDeviation::DefaultValue() const
{
   return 1;
}

double StarPointDeviation::MinimumValue() const
{
   return 0;
}

double StarPointDeviation::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

StarPointSyncDeviations::StarPointSyncDeviations( MetaProcess* P ) : MetaBoolean( P )
{
   TheStarPointSyncDeviations = this;
}

IsoString StarPointSyncDeviations::Id() const
{
   return "syncDev";
}

bool StarPointSyncDeviations::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

StarPointRemoveRotation::StarPointRemoveRotation( MetaProcess* P ) : MetaBoolean( P )
{
   TheStarPointRemoveRotation = this;
}

IsoString StarPointRemoveRotation::Id() const
{
   return "removeRotation";
}

bool StarPointRemoveRotation::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

StarPointRemoveDifractionSpikes::StarPointRemoveDifractionSpikes( MetaProcess* P ) : MetaBoolean( P )
{
   TheStarPointRemoveDifractionSpikes = this;
}

IsoString StarPointRemoveDifractionSpikes::Id() const
{
   return "removeSpikes";
}

bool StarPointRemoveDifractionSpikes::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxParameters.cpp - Released 2022-05-17T17:15:11Z
