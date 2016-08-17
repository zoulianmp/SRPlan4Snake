/*==============================================================================

  Program: SRPlan4Snake

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QtPlugin>
//#include <QSettings>

// Markups Logic includes
#include <vtkSlicerMarkupsLogic.h>

// MRMLDisplayableManager includes
#include <vtkMRMLSliceViewDisplayableManagerFactory.h>
#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

// QTGUI includes
#include <qSlicerApplication.h>
//#include <qSlicerCoreApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerNodeWriter.h>

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
//#include "qSlicerSubjectHierarchyMarkupsPlugin.h"
//#include "qSlicerSubjectHierarchyDoseVolumeHistogramPlugin.h"
//#include "qSlicerSubjectHierarchyIsodosePlugin.h"

// Markups includes
//#include "PathPlanInstantiator.h"
#include "qSRPlanPathPlanModule.h"
#include "qSRPlanPathPlanModuleWidget.h"
#include "qSlicerMarkupsReader.h"
#include "qSlicerMarkupsSettingsPanel.h"

#include "vtkSRPlanPathPlanModuleLogic.h"

//-----------------------------------------------------------------------------
//Q_EXPORT_PLUGIN2(qSRPlanPathPlanModule, qSRPlanPathPlanModule);




//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Markups
class qSRPlanPathPlanModulePrivate
{
public:
  qSRPlanPathPlanModulePrivate();
};

//-----------------------------------------------------------------------------
// qSRPlanPathPlanModulePrivate methods

//-----------------------------------------------------------------------------
qSRPlanPathPlanModulePrivate::qSRPlanPathPlanModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSRPlanPathPlanModule methods

//-----------------------------------------------------------------------------
qSRPlanPathPlanModule::qSRPlanPathPlanModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSRPlanPathPlanModulePrivate)
{
}

//-----------------------------------------------------------------------------
QStringList qSRPlanPathPlanModule::categories()const
{
  return QStringList() << "" << "Informatics";
}

//-----------------------------------------------------------------------------
qSRPlanPathPlanModule::~qSRPlanPathPlanModule()
{
}

//-----------------------------------------------------------------------------
QString qSRPlanPathPlanModule::helpText()const
{
  QString help =
    "A module to create and manage markups in 2D and 3D."
    " Replaces the Annotations module for fiducials.\n"
    "<a href=\"%1/Documentation/%2.%3/Modules/Markups\">"
    "%1/Documentation/%2.%3/Modules/Markups</a>\n";
  return help.arg(this->slicerWikiUrl()).arg(SRPlan_VERSION_MAJOR).arg(SRPlan_VERSION_MINOR);
}

//-----------------------------------------------------------------------------
QString qSRPlanPathPlanModule::acknowledgementText()const
{
  return "This work was supported by NAMIC, NAC, and the Slicer Community.";
}

//-----------------------------------------------------------------------------
QStringList qSRPlanPathPlanModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Nicole Aucoin (BWH)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSRPlanPathPlanModule::icon()const
{
  return QIcon(":/Icons/Markups.png");
}



//-----------------------------------------------------------------------------
void qSRPlanPathPlanModule::setup()
{
  this->Superclass::setup();

  
  // Register displayable managers
  // 3D
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager("vtkMRMLMarkupsFiducialDisplayableManager3D");
  // 2D
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager("vtkMRMLMarkupsFiducialDisplayableManager2D");

  // Register IO
  qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
  qSlicerMarkupsReader *markupsIO = new qSlicerMarkupsReader(vtkSlicerMarkupsLogic::SafeDownCast(this->logic()), this);
  ioManager->registerIO(markupsIO);
  ioManager->registerIO(new qSlicerNodeWriter(
                            "MarkupsFiducials", markupsIO->fileType(),
                            QStringList() << "vtkMRMLMarkupsNode", true, this));

  // settings
  /*
  if (qSlicerApplication::application())
    {
    qSlicerMarkupsSettingsPanel* panel =
      new qSlicerMarkupsSettingsPanel;
    qSlicerApplication::application()->settingsDialog()->addPanel(
      "Markups", panel);
    panel->setMarkupsLogic(vtkSlicerMarkupsLogic::SafeDownCast(this->logic()));
    }
 */
  // for now, don't use the settings panel as it's causing the logic values to
  // be reset on start up, just set things directly
  qSRPlanPathPlanModuleWidget* moduleWidget = dynamic_cast<qSRPlanPathPlanModuleWidget*>(this->widgetRepresentation());
  if (!moduleWidget)
    {
    qDebug() << "qSRPlanPathPlanModule::setup: unable to get the markups verion of the widget to set default display settings";
    }
  else
    {
    moduleWidget->updateLogicFromSettings();
    }

  // Register Subject Hierarchy core plugins
 // qSlicerSubjectHierarchyPluginHandler::instance()->registerPlugin(new qSlicerSubjectHierarchyMarkupsPlugin());
 // qSlicerSubjectHierarchyPluginHandler::instance()->registerPlugin(new qSlicerSubjectHierarchyDoseVolumeHistogramPlugin());
 // qSlicerSubjectHierarchyPluginHandler::instance()->registerPlugin(new qSlicerSubjectHierarchyIsodosePlugin());
  
}


//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSRPlanPathPlanModule::createWidgetRepresentation()
{
   
   return new qSRPlanPathPlanModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSRPlanPathPlanModule::createLogic()
{
	
    return vtkSRPlanPathPlanModuleLogic::New();
}
