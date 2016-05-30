/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// SubjectHierarchy MRML includes
#include "vtkMRMLSubjectHierarchyConstants.h"
#include "vtkMRMLSubjectHierarchyNode.h"

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchySRPlanPlugin.h"
#include "qSlicerSubjectHierarchyDefaultPlugin.h"

// DICOMLib includes
//#include "qSlicerDICOMExportDialog.h"

// Qt includes
#include <QDebug>
#include <QAction>
#include <QStandardItem>

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SubjectHierarchy_Widgets
class qSlicerSubjectHierarchySRPlanPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchySRPlanPlugin);
protected:
  qSlicerSubjectHierarchySRPlanPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchySRPlanPluginPrivate(qSlicerSubjectHierarchySRPlanPlugin& object);
  ~qSlicerSubjectHierarchySRPlanPluginPrivate();
  void init();
public:
  QIcon PatientIcon;
  QIcon CourseIcon;
  QIcon PlanIcon;

  QAction* CreatePatientAction;
  QAction* CreateCourseAction;

  QAction* CreatePlanAction;
  QAction* CreateImageVolumeAction;

  QAction* ConvertFolderToPatientAction;
  QAction* ConvertFolderToCourseAction;
 // QAction* OpenDICOMExportDialogAction;



};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyDICOMPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchySRPlanPluginPrivate::qSlicerSubjectHierarchySRPlanPluginPrivate(qSlicerSubjectHierarchySRPlanPlugin& object)
: q_ptr(&object)
{
  this->PatientIcon = QIcon(":Icons/Patient.png");
  this->CourseIcon = QIcon(":Icons/Course.png");
  this->PlanIcon = QIcon(":Icons/Plan.png");


  this->CreatePatientAction = NULL;
  this->CreateCourseAction = NULL;
  this->CreatePlanAction = NULL;
  this->CreateImageVolumeAction = NULL;


  this->ConvertFolderToPatientAction = NULL;
  this->ConvertFolderToCourseAction = NULL;

 

 // this->OpenDICOMExportDialogAction = NULL;
}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchySRPlanPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchySRPlanPlugin);

  this->CreatePatientAction = new QAction("Create new patient",q);
  QObject::connect(this->CreatePatientAction, SIGNAL(triggered()), q, SLOT(createPatientNode()));

  this->CreateCourseAction = new QAction("Create treatment course",q);
  QObject::connect(this->CreateCourseAction, SIGNAL(triggered()), q, SLOT(createChildCourseUnderCurrentNode()));


  this->CreatePlanAction = new QAction("Add Plan to Course", q);
  QObject::connect(this->CreatePlanAction, SIGNAL(triggered()), q, SLOT(createChildPlanUnderCurrentNode()));

  this->CreateImageVolumeAction = new QAction("Add Volume Image to Plan ", q);
  QObject::connect(this->CreateImageVolumeAction, SIGNAL(triggered()), q, SLOT(createChildImageVolumeUnderCurrentNode()));


  this->ConvertFolderToPatientAction = new QAction("Convert folder to patient",q);
  QObject::connect(this->ConvertFolderToPatientAction, SIGNAL(triggered()), q, SLOT(convertCurrentNodeToPatient()));

  this->ConvertFolderToCourseAction = new QAction("Convert folder to course",q);
  QObject::connect(this->ConvertFolderToCourseAction, SIGNAL(triggered()), q, SLOT(convertCurrentNodeToCourse()));

 // this->OpenDICOMExportDialogAction = new QAction("Export to DICOM...",q);
 // QObject::connect(this->OpenDICOMExportDialogAction, SIGNAL(triggered()), q, SLOT(openDICOMExportDialog()));
  





}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchySRPlanPluginPrivate::~qSlicerSubjectHierarchySRPlanPluginPrivate()
{
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchySRPlanPlugin::qSlicerSubjectHierarchySRPlanPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchySRPlanPluginPrivate(*this) )
{
  this->m_Name = QString("SRPlan");

  Q_D(qSlicerSubjectHierarchySRPlanPlugin);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchySRPlanPlugin::~qSlicerSubjectHierarchySRPlanPlugin()
{
}

//---------------------------------------------------------------------------
double qSlicerSubjectHierarchySRPlanPlugin::canOwnSubjectHierarchyNode(vtkMRMLSubjectHierarchyNode* node)const
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::canOwnSubjectHierarchyNode: Input node is NULL!";
    return 0.0;
    }

  // Patient level
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient()))
    {
    return 0.5;
    }
  // Course level (so that creation of a generic series is possible)
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse()))
    {
    return 0.3;
    }

  // Plan level (so that creation of a generic series is possible)
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan()))
  {
	  return 0.2;
  }

  return 0.0;
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchySRPlanPlugin::roleForPlugin()const
{
  // Get current node to determine role
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if (!currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::roleForPlugin: Invalid current node!";
    return "Error!";
    }

  // Patient level
  if (currentNode->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient()))
    {
    return "Patient"; // Show the role Subject to the user, while internally it is used for the patient notation defined in DICOM
    }
  // Study level (so that creation of a generic series is possible)
  if (currentNode->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse()))
    {
    return "Course";
    }

  if (currentNode->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan()))
  {
	  return "Plan";
  }

  return QString("Error!");
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchySRPlanPlugin::helpText()const
{
  return QString(
    "<p style=\" margin-top:4px; margin-bottom:1px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\" font-family:'sans-serif'; font-size:9pt; font-weight:600; color:#000000;\">"
    "Create new Subject from scratch"
    "</span>"
    "</p>"
    "<p style=\" margin-top:0px; margin-bottom:11px; margin-left:26px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\" font-family:'sans-serif'; font-size:9pt; color:#000000;\">"
    "Right-click on the top-level item 'Scene' and select 'Create new subject'."
    "</span>"
    "</p>\n"
    "<p style=\" margin-top:4px; margin-bottom:1px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\" font-family:'sans-serif'; font-size:9pt; font-weight:600; color:#000000;\">"
    "Create new hierarchy node"
    "</span>"
    "</p>"
    "<p style=\" margin-top:0px; margin-bottom:11px; margin-left:26px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\" font-family:'sans-serif'; font-size:9pt; color:#000000;\">"
    "Right-click on an existing node and select 'Create child ...'. "
    "The type and level of the child node will be specified by the user and the possible types depend on the parent."
    "</span>"
    "</p>");
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchySRPlanPlugin::icon(vtkMRMLSubjectHierarchyNode* node)
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::icon: NULL node given!";
    return QIcon();
    }

  Q_D(qSlicerSubjectHierarchySRPlanPlugin);

  // Patient icon
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient()))
    {
    return d->PatientIcon;
    }
  // Study icon
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse()))
    {
    return d->CourseIcon;
    }

  // Plan icon
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan()))
  {
	  return d->PlanIcon;
  }


  // Node unknown by plugin
  return QIcon();
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchySRPlanPlugin::visibilityIcon(int visible)
{
  // Have the default plugin (which is not registered) take care of this
  return qSlicerSubjectHierarchyPluginHandler::instance()->defaultPlugin()->visibilityIcon(visible);
}

//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchySRPlanPlugin::nodeContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchySRPlanPlugin);

  QList<QAction*> actions;
  actions << d->CreateCourseAction << d->CreatePlanAction << d->CreateImageVolumeAction
	  << d->ConvertFolderToPatientAction << d->ConvertFolderToCourseAction;	  
  return actions;

}

//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchySRPlanPlugin::sceneContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchySRPlanPlugin);

  QList<QAction*> actions;
  actions << d->CreatePatientAction;
  return actions;
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchySRPlanPlugin::showContextMenuActionsForNode(vtkMRMLSubjectHierarchyNode* node)
{
  Q_D(qSlicerSubjectHierarchySRPlanPlugin);
  this->hideAllContextMenuActions();

  // Scene
  if (!node)
    {
    d->CreatePatientAction->setVisible(true);
    return;
    }

  // Patient
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient()))
    {
    d->CreateCourseAction->setVisible(true);
    }
  // Folder
  else if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder()))
    {
    d->ConvertFolderToPatientAction->setVisible(true);
    d->ConvertFolderToCourseAction->setVisible(true);
    }
  // Course
  else if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse()))
    {
    //if (this->canBeExported(node)) //TODO:
      {
      d->CreatePlanAction->setVisible(true);
	 

      }
    }
  // Plan
  else if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan()))
    {
    //if (this->canBeExported(node)) //TODO:
      {
      d->CreateImageVolumeAction->setVisible(true);
	 
      }
    }
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchySRPlanPlugin::editProperties(vtkMRMLSubjectHierarchyNode* node)
{
  Q_UNUSED(node);
  //TODO: Show DICOM tag editor?
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchySRPlanPlugin::createPatientNode()
{
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  if (!scene)
    {
    qCritical() << "qSlicerSubjectHierarchyDICOMPlugin::createPatientNode: Invalid MRML scene!";
    return;
    }

  // It is called Subject to the user, while internally it is used for the patient 
  std::string nodeName = vtkMRMLSubjectHierarchyConstants::GetSRPlanPatientNodeBaseName();
  nodeName = scene->GenerateUniqueName(nodeName);
  // Create patient subject hierarchy node
  vtkMRMLSubjectHierarchyNode* childSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(
    scene, NULL, vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient(), nodeName.c_str());
  emit requestExpandNode(childSubjectHierarchyNode);
}

//---------------------------------------------------------------------------
//Create Course Node under current node (must be patient)
void qSlicerSubjectHierarchySRPlanPlugin::createChildCourseUnderCurrentNode()
{
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if (!scene || !currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::createChildCourseUnderCurrentNode: Invalid MRML scene or current node!";
    return;
    }

  std::string nodeName = vtkMRMLSubjectHierarchyConstants::GetSRPlanCourseNodeBaseName();
  nodeName = scene->GenerateUniqueName(nodeName);
  // Create course subject hierarchy node
  vtkMRMLSubjectHierarchyNode* childSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(
    scene, currentNode, vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse(), nodeName.c_str());
  emit requestExpandNode(childSubjectHierarchyNode);
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchySRPlanPlugin::convertCurrentNodeToPatient()
{
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if ( !currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::convertCurrentNodeToPatient: Invalid current node!";
    return;
    }

  currentNode->SetLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient());
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchySRPlanPlugin::convertCurrentNodeToCourse()
{
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if ( !currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::convertCurrentNodeToPatient: Invalid current node!";
    return;
    }

  currentNode->SetLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse());
}



//Create Plan Node under current node (must be Course)
void qSlicerSubjectHierarchySRPlanPlugin::createChildPlanUnderCurrentNode()
{
	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
	if (!scene || !currentNode)
	{
		qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::createChildPlanUnderCurrentNode: Invalid MRML scene or current node!";
		return;
	}

	std::string nodeName = vtkMRMLSubjectHierarchyConstants::GetSRPlanPlanNodeBaseName();
	nodeName = scene->GenerateUniqueName(nodeName);
	// Create plan subject hierarchy node
	vtkMRMLSubjectHierarchyNode* childSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(
		scene, currentNode, vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan(), nodeName.c_str());
	emit requestExpandNode(childSubjectHierarchyNode);

}


//Create Plan Node under current node (must be Plan)
void qSlicerSubjectHierarchySRPlanPlugin::createChildImageVolumeUnderCurrentNode()
{
	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
	if (!scene || !currentNode)
	{
		qCritical() << "qSlicerSubjectHierarchySRPlanPlugin::createChildImageVolumeUnderCurrentNode: Invalid MRML scene or current node!";
		return;
	}

	vtkMRMLSubjectHierarchyNode* primaryImageNode;
	primaryImageNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene, vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID());

	if (!primaryImageNode)
	{

		primaryImageNode = vtkMRMLSubjectHierarchyNode::New();

		primaryImageNode->SetLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRSubplan());
		primaryImageNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID());
		primaryImageNode->SetOwnerPluginName("Volumes");

		primaryImageNode->SetName("PrimaryVolume");

		primaryImageNode->SetParentNodeID(currentNode->GetID());

		scene->AddNode(primaryImageNode);

		primaryImageNode->Delete(); // Return ownership to t	
	}
	else
	{
		vtkMRMLSubjectHierarchyNode* ImageNode;		
		ImageNode = vtkMRMLSubjectHierarchyNode::New();

		ImageNode->SetLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRSubplan());
		//ImageNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSRPlanDoseVolumeUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID());

		std::string nodeName = vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeNodeBaseName();
		nodeName = scene->GenerateUniqueName(nodeName);

		ImageNode->SetOwnerPluginName("Volumes");


		ImageNode->SetName(nodeName.c_str());

		ImageNode->SetParentNodeID(currentNode->GetID());

		scene->AddNode(ImageNode);

		ImageNode->Delete(); // Return ownership to t	

		
	}




}



//---------------------------------------------------------------------------
/*
void qSlicerSubjectHierarchyDICOMPlugin::openDICOMExportDialog()
{
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if (!currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchyDICOMPlugin::openDICOMExportDialog: Invalid current node!";
    return;
    }

  qSlicerDICOMExportDialog* exportDialog = new qSlicerDICOMExportDialog(NULL);
  exportDialog->setMRMLScene(qSlicerSubjectHierarchyPluginHandler::instance()->scene());
  exportDialog->exec(currentNode);

  delete exportDialog;
  
}
*/