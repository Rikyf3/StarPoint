#ifndef __StarPointParameters_h
#define __StarPointParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class StarPointAmplitude : public MetaFloat
{
public:

   StarPointAmplitude( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern StarPointAmplitude* TheStarPointAmplitude;

// ----------------------------------------------------------------------------

class StarPointDeviation : public MetaFloat
{
public:

   StarPointDeviation( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern StarPointDeviation* TheStarPointDeviation;

// ----------------------------------------------------------------------------

class StarPointSyncDeviations : public MetaBoolean
{
public:

   StarPointSyncDeviations( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern StarPointSyncDeviations* TheStarPointSyncDeviations;

// ----------------------------------------------------------------------------

class StarPointRemoveRotation : public MetaBoolean
{
public:

   StarPointRemoveRotation( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern StarPointRemoveRotation* TheStarPointRemoveRotation;

// ----------------------------------------------------------------------------

class StarPointRemoveDifractionSpikes : public MetaBoolean
{
public:

   StarPointRemoveDifractionSpikes( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern StarPointRemoveDifractionSpikes* TheStarPointRemoveDifractionSpikes;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarPointParameters_h

// ----------------------------------------------------------------------------
// EOF StarPointParameters.h - Released
