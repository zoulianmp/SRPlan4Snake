/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QtPlugin>

// SlicerQt includes
#include "qSlicerWelcomeModule.h"
#include "qSlicerWelcomeModuleWidget.h"

// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerIOManager.h"
#include "qSlicerModuleManager.h"

#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchySRPlanPlugin.h"

// Data includes
#include "qSlicerDataDialog.h"

#include "qSlicerSaveDataDialog.h"
#include "qSlicerSceneBundleReader.h"
#include "qSlicerSceneReader.h"
#include "qSlicerSceneWriter.h"
#include "qSlicerSlicer2SceneReader.h"
#include "qSlicerXcedeCatalogReader.h"


// SlicerLogic includes
#include <vtkSlicerApplicationLogic.h>



// Logic includes
#include <vtkMRMLColorLogic.h>
#include <vtkSlicerCamerasModuleLogic.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
//Q_EXPORT_PLUGIN2(qSlicerWelcomeModule, qSlicerWelcomeModule);



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SlicerWelcome
class qSlicerWelcomeModulePrivate
{
public:
};

//-----------------------------------------------------------------------------
qSlicerWelcomeModule::qSlicerWelcomeModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerWelcomeModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerWelcomeModule::~qSlicerWelcomeModule()
{
}

//-----------------------------------------------------------------------------
QStringList qSlicerWelcomeModule::categories()const
{
  return QStringList() << "";
}

//-----------------------------------------------------------------------------
QIcon qSlicerWelcomeModule::icon()const
{
  return QIcon(":/Icons/SlicerWelcome.png");
}

//-----------------------------------------------------------------------------
QString qSlicerWelcomeModule::helpText()const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerWelcomeModule::acknowledgementText()const
{
  return "This work was supported by NA-MIC, NAC, BIRN, NCIGT, CTSC and the Slicer Community. "
      "See <a>http://www.slicer.org</a> for details. We would also like to express our sincere "
      "thanks to members of the Slicer User Community who have helped us to design the contents "
      "of this Welcome Module, and whose feedback continues to improve functionality, usability "
      "and Slicer user experience\n.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerWelcomeModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Wendy Plesniak (SPL, BWH)");
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}



//-----------------------------------------------------------------------------
QStringList qSlicerWelcomeModule::dependencies() const
{
  QStringList moduleDependencies;
  // Colors: Required to have a valid color logic for XcedeCatalogUI.
  // Cameras: Required in qSlicerSceneReader
  moduleDependencies << "Colors" << "Cameras"<<"SubjectHierarchy";
 // moduleDependencies << "Colors" << "Cameras"  ;
  return moduleDependencies;
}



//-----------------------------------------------------------------------------
void qSlicerWelcomeModule::setup()
{
  this->Superclass::setup();

  qSlicerAbstractCoreModule* colorsModule =
    qSlicerCoreApplication::application()->moduleManager()->module("Colors");
  vtkMRMLColorLogic* colorLogic =
    vtkMRMLColorLogic::SafeDownCast(colorsModule ? colorsModule->logic() : 0);

  qSlicerAbstractCoreModule* camerasModule =
    qSlicerCoreApplication::application()->moduleManager()->module("Cameras");
  vtkSlicerCamerasModuleLogic* camerasLogic =
    vtkSlicerCamerasModuleLogic::SafeDownCast(camerasModule ? camerasModule->logic() : 0);

  qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();

  // Readers
  ioManager->registerIO(new qSlicerSceneReader(camerasLogic, this));
  ioManager->registerIO(new qSlicerSceneBundleReader(this));
  ioManager->registerIO(new qSlicerSlicer2SceneReader(this->appLogic(), this));
  ioManager->registerIO(new qSlicerXcedeCatalogReader(colorLogic, this));

  // Writers
  ioManager->registerIO(new qSlicerSceneWriter(this));

  // Dialogs
  ioManager->registerDialog(new qSlicerDataDialog(this));
  ioManager->registerDialog(new qSlicerSaveDataDialog(this));
  
  qSlicerSubjectHierarchyPluginHandler::instance()->registerPlugin(
  	  new qSlicerSubjectHierarchySRPlanPlugin());
}


//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerWelcomeModule::createWidgetRepresentation()
{
  return new qSlicerWelcomeModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerWelcomeModule::createLogic()
{
  return 0;
}
