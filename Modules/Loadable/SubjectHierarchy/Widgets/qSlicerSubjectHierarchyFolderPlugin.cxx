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
#include "qSlicerSubjectHierarchyFolderPlugin.h"
#include "qSlicerSubjectHierarchyDefaultPlugin.h"

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
class qSlicerSubjectHierarchyFolderPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchyFolderPlugin);
protected:
  qSlicerSubjectHierarchyFolderPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchyFolderPluginPrivate(qSlicerSubjectHierarchyFolderPlugin& object);
  ~qSlicerSubjectHierarchyFolderPluginPrivate();
  void init();
public:
  QIcon FolderIcon;

 // QAction* CreateFolderUnderSceneAction; //commentout by zoulian
  QAction* CreatePoisFolderUnderNodeAction;
  QAction* CreateTreatPathFolderUnderNodeAction;
};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyFolderPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyFolderPluginPrivate::qSlicerSubjectHierarchyFolderPluginPrivate(qSlicerSubjectHierarchyFolderPlugin& object)
: q_ptr(&object)
{
  this->FolderIcon = QIcon(":Icons/Folder.png");

  //this->CreateFolderUnderSceneAction = NULL;
  this->CreatePoisFolderUnderNodeAction = NULL;
  this->CreateTreatPathFolderUnderNodeAction = NULL;

}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchyFolderPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchyFolderPlugin);

  /*
  this->CreateFolderUnderSceneAction = new QAction("Create new folder",q);
  QObject::connect(this->CreateFolderUnderSceneAction, SIGNAL(triggered()), q, SLOT(createFolderUnderScene()));
  */
  this->CreatePoisFolderUnderNodeAction = new QAction("Add POIs to Primary Volume",q);
  QObject::connect(this->CreatePoisFolderUnderNodeAction, SIGNAL(triggered()), q, SLOT(createPOIsToPrimaryImageVolume()));

  this->CreateTreatPathFolderUnderNodeAction = new QAction("Add RTPath to Primary Volume", q);
  QObject::connect(this->CreateTreatPathFolderUnderNodeAction, SIGNAL(triggered()), q, SLOT(createTreatPathToPrimaryImageVolume()));

}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyFolderPluginPrivate::~qSlicerSubjectHierarchyFolderPluginPrivate()
{
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyFolderPlugin::qSlicerSubjectHierarchyFolderPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchyFolderPluginPrivate(*this) )
{
  this->m_Name = QString("Folder");

  Q_D(qSlicerSubjectHierarchyFolderPlugin);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyFolderPlugin::~qSlicerSubjectHierarchyFolderPlugin()
{
}

//---------------------------------------------------------------------------
double qSlicerSubjectHierarchyFolderPlugin::canOwnSubjectHierarchyNode(vtkMRMLSubjectHierarchyNode* node)const
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyFolderPlugin::canOwnSubjectHierarchyNode: Input node is NULL!";
    return 0.0;
    }

  // Folder
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder()))
    {
    return 1.0;
    }

  return 0.0;
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchyFolderPlugin::roleForPlugin()const
{
  // Get current node to determine role
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if (!currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchyFolderPlugin::roleForPlugin: Invalid current node!";
    return "Error!";
    }

  // Folder level
  if (currentNode->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder()))
    {
    return "Folder";
    }

  return QString("Error!");
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyFolderPlugin::icon(vtkMRMLSubjectHierarchyNode* node)
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyFolderPlugin::icon: NULL node given!";
    return QIcon();
    }

  Q_D(qSlicerSubjectHierarchyFolderPlugin);

  // Subject and Folder icon
  if (node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder()))
    {
    return d->FolderIcon;
    }

  // Node unknown by plugin
  return QIcon();
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyFolderPlugin::visibilityIcon(int visible)
{
  // Have the default plugin (which is not registered) take care of this
  return qSlicerSubjectHierarchyPluginHandler::instance()->defaultPlugin()->visibilityIcon(visible);
}

//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyFolderPlugin::nodeContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyFolderPlugin);

  QList<QAction*> actions;
  actions << d->CreatePoisFolderUnderNodeAction << d->CreateTreatPathFolderUnderNodeAction;
  return actions;



}

/*
//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyFolderPlugin::sceneContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyFolderPlugin);

  QList<QAction*> actions;
  actions << d->CreateFolderUnderSceneAction;
  return actions;
}

*/

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyFolderPlugin::showContextMenuActionsForNode(vtkMRMLSubjectHierarchyNode* node)
{
  Q_D(qSlicerSubjectHierarchyFolderPlugin);
  this->hideAllContextMenuActions();


  // POIs Folders Under Plan
  if (node && node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan()) )
  {

	  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	  if (!scene)
	  {
		  qCritical() << "qSlicerSubjectHierarchyFolderPlugin::showContextMenuActionsForNode: Invalid MRML scene!";
	  }

     vtkMRMLSubjectHierarchyNode* primaryImageVolumeSHNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene, vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID());

	 if (primaryImageVolumeSHNode && primaryImageVolumeSHNode->GetAssociatedNode())
	 {

		 d->CreatePoisFolderUnderNodeAction->setVisible(true);

		 vtkStdString structuresetid = primaryImageVolumeSHNode->GetUID(vtkMRMLSubjectHierarchyConstants::GetSHStructureSetUIDName());

		 if (!structuresetid.empty())
		 {
			 d->CreateTreatPathFolderUnderNodeAction->setVisible(true);
		 }
		
	 }

  }
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyFolderPlugin::editProperties(vtkMRMLSubjectHierarchyNode* node)
{
  Q_UNUSED(node);
}

//---------------------------------------------------------------------------
vtkMRMLSubjectHierarchyNode* qSlicerSubjectHierarchyFolderPlugin::createFolderUnderNode(vtkMRMLSubjectHierarchyNode* parentNode)
{
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  if (!scene)
    {
    qCritical() << "qSlicerSubjectHierarchyFolderPlugin::createFolderUnderNode: Invalid MRML scene!";
    return NULL;
    }



  
  vtkMRMLSubjectHierarchyNode* pois = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene, vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUID());
  if (pois)
  {
	  return pois;	
  }

  // Create folder subject hierarchy node
  std::string nodeName = "POIs";
  nodeName = scene->GenerateUniqueName(nodeName);
  vtkMRMLSubjectHierarchyNode* childSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(
    scene, parentNode, vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder(), nodeName.c_str());

  childSubjectHierarchyNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUID());



  emit requestExpandNode(childSubjectHierarchyNode);

  return childSubjectHierarchyNode;
}

/*
//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyFolderPlugin::createFolderUnderScene()
{
  this->createFolderUnderNode(NULL);
}
*/

//---------------------------------------------------------------------------
vtkMRMLSubjectHierarchyNode* qSlicerSubjectHierarchyFolderPlugin::createPoisFolderUnderCurrentNode()
{
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  if (!currentNode)
    {
    qCritical() << "qSlicerSubjectHierarchyFolderPlugin::createPoisFolderUnderCurrentNode: Invalid current node!";
    return NULL;
    }

  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  if (!scene)
  {
	  qCritical() << "qSlicerSubjectHierarchyFolderPlugin::createFolderUnderNode: Invalid MRML scene!";
	  return NULL;
  }

  vtkMRMLSubjectHierarchyNode* pois = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene, vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUID());
  if (pois)
  {
	  return pois;
  }

  // Create folder subject hierarchy node
  std::string nodeName = "POIs";
  nodeName = scene->GenerateUniqueName(nodeName);
  vtkMRMLSubjectHierarchyNode* childSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(
	  scene, currentNode, vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder(), nodeName.c_str());

  childSubjectHierarchyNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPOIsFolderUID());



  emit requestExpandNode(childSubjectHierarchyNode);

  return childSubjectHierarchyNode;

}

void qSlicerSubjectHierarchyFolderPlugin::createTreatPathToPrimaryImageVolume()
{
	vtkMRMLSubjectHierarchyNode* path = this->createTreatPathFolderUnderCurrentNode();
	this->AddSHImageVolumeNodeUIDToSHNode(path);


}

void qSlicerSubjectHierarchyFolderPlugin::createPOIsToPrimaryImageVolume()
{
	vtkMRMLSubjectHierarchyNode* pois = this->createPoisFolderUnderCurrentNode();
	this->AddSHImageVolumeNodeUIDToSHNode(pois);
	
}







vtkMRMLSubjectHierarchyNode* qSlicerSubjectHierarchyFolderPlugin::createTreatPathFolderUnderCurrentNode()
{
	vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
	if (!currentNode)
	{
		qCritical() << "qSlicerSubjectHierarchyFolderPlugin::createTreatPathFolderUnderCurrentNode: Invalid current node!";
		return NULL;
	}

	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	if (!scene)
	{
		qCritical() << "qSlicerSubjectHierarchyFolderPlugin::createPoisFolderUnderCurrentNode: Invalid MRML scene!";
		return NULL;
	}



	// Create folder subject hierarchy node
	std::string nodeName = "SRPath";
	nodeName = scene->GenerateUniqueName(nodeName);

	vtkMRMLSubjectHierarchyNode* childSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(
		scene, currentNode, vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder(), nodeName.c_str());


	emit requestExpandNode(childSubjectHierarchyNode);

	return childSubjectHierarchyNode;






}

bool qSlicerSubjectHierarchyFolderPlugin::AddSHImageVolumeNodeUIDToSHNode(vtkMRMLSubjectHierarchyNode* node, vtkMRMLSubjectHierarchyNode* imagevolume)
{
	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	if (!scene)
	{
		qCritical() << "qSlicerSubjectHierarchyFolderPlugin::createPoisFolderUnderCurrentNode: Invalid MRML scene!";
		return NULL;
	}

	if (!imagevolume)
	{
		vtkMRMLSubjectHierarchyNode* primaryImageVolumeSHNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene, vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID());


		vtkStdString SHID = primaryImageVolumeSHNode->GetID();

	    // Associate the node to the primaryImage Volume
		node->AddUID(vtkMRMLSubjectHierarchyConstants::GetSHImageVolumeUIDName(), SHID.c_str());
		return true;

	}
	else
	{
		// Associate the node to the Predefinded Image Volume
		node->AddUID(vtkMRMLSubjectHierarchyConstants::GetSHImageVolumeUIDName(), imagevolume->GetID());
		return true;

	}

}