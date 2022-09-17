#ifndef __StarPointInterface_h
#define __StarPointInterface_h

#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/CheckBox.h>
#include <pcl/ToolButton.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/Settings.h>

#include "StarPointInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class StarPointInterface : public ProcessInterface
{
public:

   StarPointInterface();
   virtual ~StarPointInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:
   StarPointInstance m_instance;

   struct GUIData
   {
      GUIData( StarPointInterface& );

      VerticalSizer     GlobalSizer;
         NumericControl    Amplitude_NumericControl;
         NumericControl    Dev_NumericControl;
         HorizontalSizer   SyncDevSizer;
            Label             SyncDev_Label;
            CheckBox           SyncDev_CheckBox;
         HorizontalSizer   DeleteRotationSizer;
            Label             DeleteRotation_Label;
            CheckBox           DeleteRotation_CheckBox;
         HorizontalSizer   DeleteSpikesSizer;
            Label             DeleteSpikes_Label;
            CheckBox           DeleteSpikes_CheckBox;
   };

   GUIData* GUI = nullptr;

   /*
    * Control updates
    */
   void UpdateControls();

   /*
    * Event handlers
    */
   void e_RealValueUpdated( NumericEdit& sender, double value );
   void e_ItemClicked( Button& sender, bool checked );
   void __Button_Click( Button& sender, bool checked );

   friend struct GUIData;
   friend class StatusHandler;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern StarPointInterface* TheStarPointInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SandboxInterface_h

// ----------------------------------------------------------------------------
// EOF SandboxInterface.h - Released 2022-05-17T17:15:11Z
