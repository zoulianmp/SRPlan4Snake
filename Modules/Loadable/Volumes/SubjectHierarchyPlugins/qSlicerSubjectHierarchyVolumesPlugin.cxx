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
#include "vtkMRMLSubjectHierarchyNode.h"
#include "vtkMRMLSubjectHierarchyConstants.h"

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchyVolumesPlugin.h"
#include "qSlicerSubjectHierarchyDefaultPlugin.h"

// Slicer includes
#include "qSlicerCoreApplication.h"
#include "vtkSlicerApplicationLogic.h"


#include "qSlicerIOManager.h"

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLSliceCompositeNode.h>
#include <vtkMRMLSliceNode.h>
#include "vtkMRMLSegmentationNode.h"

#include "vtkMRMLLabelMapVolumeNode.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkCollection.h>
#include <vtkImageData.h>

// Qt includes
#include <QDebug>
#include <QStandardItem>
#include <QAction>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerModuleManager.h"


// MRML widgets includes
#include "qMRMLNodeComboBox.h"
#include "vtkSlicerVolumesLogic.h"
#include "qSlicerApplication.h"

#include "vtkMRMLSelectionNode.h"

#include "vtkSlicerSegmentationsModuleLogic.h"

// STD includes
#include <set>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SubjectHierarchy_Plugins
class qSlicerSubjectHierarchyVolumesPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchyVolumesPlugin);
protected:
  qSlicerSubjectHierarchyVolumesPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchyVolumesPluginPrivate(qSlicerSubjectHierarchyVolumesPlugin& object);
  ~qSlicerSubjectHierarchyVolumesPluginPrivate();
  void init();
public:
  QIcon VolumeIcon;
  QIcon VolumeVisibilityOffIcon;
  QIcon VolumeVisibilityOnIcon;

  QAction* ShowVolumesInBranchAction;
  QAction* FeedImageSeriesAction;


  //QAction* AddStructureSetAction;
  QAction* StartSegmentationAction;
};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyVolumesPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyVolumesPluginPrivate::qSlicerSubjectHierarchyVolumesPluginPrivate(qSlicerSubjectHierarchyVolumesPlugin& object)
: q_ptr(&object)
{
  this->VolumeIcon = QIcon(":Icons/Volume.png");
  this->VolumeVisibilityOffIcon = QIcon(":Icons/VolumeVisibilityOff.png");
  this->VolumeVisibilityOnIcon = QIcon(":Icons/VolumeVisibilityOn.png");

  this->ShowVolumesInBranchAction = NULL;
  this->FeedImageSeriesAction = NULL;

 // this->AddStructureSetAction = NULL;
  this->StartSegmentationAction = NULL;
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyVolumesPluginPrivate::~qSlicerSubjectHierarchyVolumesPluginPrivate()
{
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyVolumesPlugin::qSlicerSubjectHierarchyVolumesPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchyVolumesPluginPrivate(*this) )
{
  this->m_Name = QString("Volumes");

  Q_D(qSlicerSubjectHierarchyVolumesPlugin);
  d->init();
}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchyVolumesPlugin);

  this->ShowVolumesInBranchAction = new QAction("Show volumes in branch",q);
  QObject::connect(this->ShowVolumesInBranchAction, SIGNAL(triggered()), q, SLOT(showVolumesInBranch()));

  this->FeedImageSeriesAction = new QAction("Feed Image Series", q);
  QObject::connect(this->FeedImageSeriesAction, SIGNAL(triggered()), q, SLOT(feedImageSeriesIntoSHNode()));

 // this->AddStructureSetAction = new QAction("Add Structure Set", q);
 // QObject::connect(this->AddStructureSetAction, SIGNAL(triggered()), q, SLOT(addStructureSetToSHNode()));

  this->StartSegmentationAction = new QAction("Start Segmentation", q);
  QObject::connect(this->StartSegmentationAction, SIGNAL(triggered()), q, SLOT(startSegmentation()));
  
  
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyVolumesPlugin::~qSlicerSubjectHierarchyVolumesPlugin()
{
}

//----------------------------------------------------------------------------
double qSlicerSubjectHierarchyVolumesPlugin::canAddNodeToSubjectHierarchy(vtkMRMLNode* node, vtkMRMLSubjectHierarchyNode* parent/*=NULL*/)const
{
  Q_UNUSED(parent);
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::canAddNodeToSubjectHierarchy: Input node is NULL!";
    return 0.0;
    }
  else if (node->IsA("vtkMRMLScalarVolumeNode"))
    {
    // Node is a volume
    return 0.5;
    }
  return 0.0;
}





//---------------------------------------------------------------------------
double qSlicerSubjectHierarchyVolumesPlugin::canOwnSubjectHierarchyNode(vtkMRMLSubjectHierarchyNode* node)const
{
  if (!node)
    {
 //   qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::canOwnSubjectHierarchyNode: Input node is NULL!";
    return 0.0;
    }

  vtkStdString primaryImageUID = node->GetUID(vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeUIDName());
	
  //Primary Image SubjectHierarchy Node
  if (!strcmp(primaryImageUID.c_str(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID()))
  {
	  return 0.5;

  }


 
  if (node->GetOwnerPluginName())
  {
	  vtkStdString nodeOwnerPluginName = node->GetOwnerPluginName();
	  if (!strcmp(nodeOwnerPluginName.c_str(), "Volumes"))
	  {
	  
		 return 0.5;
 
	  }
  }


  // Volume
  vtkMRMLNode* associatedNode = node->GetAssociatedNode();


  if (associatedNode && associatedNode->IsA("vtkMRMLScalarVolumeNode"))
    {
    return 0.5; // There are other plugins that can handle special volume nodes better, thus the relatively low value
    }
 

  return 0.0;
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchyVolumesPlugin::roleForPlugin()const
{
  return "Scalar volume";
}

//-----------------------------------------------------------------------------
QString qSlicerSubjectHierarchyVolumesPlugin::tooltip(vtkMRMLSubjectHierarchyNode* node)const
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::tooltip: Subject hierarchy node is NULL!";
    return QString("Invalid!");
    }

  // Get basic tooltip from abstract plugin
  QString tooltipString = Superclass::tooltip(node);

  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node->GetAssociatedNode());
  vtkImageData* imageData = (volumeNode ? volumeNode->GetImageData() : NULL);
  if (volumeNode && imageData)
    {
    int dimensions[3] = {0,0,0};
    imageData->GetDimensions(dimensions);
    double spacing[3] = {0.0,0.0,0.0};
    volumeNode->GetSpacing(spacing);
    tooltipString.append( QString(" (Dimensions: %1x%2x%3  Spacing: %4mm x %5mm x %6mm)")
      .arg(dimensions[0]).arg(dimensions[1]).arg(dimensions[2])
      .arg(spacing[0],0,'g',3).arg(spacing[1],0,'g',3).arg(spacing[2],0,'g',3) );
    }
  else
    {
    tooltipString.append(" !Invalid volume!");
    }

  return tooltipString;
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyVolumesPlugin::icon(vtkMRMLSubjectHierarchyNode* node)
{
  Q_D(qSlicerSubjectHierarchyVolumesPlugin);

  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::icon: NULL node given!";
    return QIcon();
    }

  // Volume
  if (this->canOwnSubjectHierarchyNode(node))
    {
    return d->VolumeIcon;
    }

  // Node unknown by plugin
  return QIcon();
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyVolumesPlugin::visibilityIcon(int visible)
{
  Q_D(qSlicerSubjectHierarchyVolumesPlugin);

  if (visible == 1)
    {
    return d->VolumeVisibilityOnIcon;
    }
  else
    {
    return d->VolumeVisibilityOffIcon;
    }
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPlugin::setDisplayVisibility(vtkMRMLSubjectHierarchyNode* node, int visible)
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::setDisplayVisibility: NULL node!";
    return;
    }

  vtkMRMLScalarVolumeNode* associatedVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node->GetAssociatedNode());
  // Volume
  if (associatedVolumeNode)
    {
    this->showVolume(associatedVolumeNode, visible);
    }
  // Default
  else
    {
    qSlicerSubjectHierarchyPluginHandler::instance()->defaultPlugin()->setDisplayVisibility(node, visible);
    }
}

//-----------------------------------------------------------------------------
int qSlicerSubjectHierarchyVolumesPlugin::getDisplayVisibility(vtkMRMLSubjectHierarchyNode* node)const
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::getDisplayVisibility: NULL node!";
    return -1;
    }

  // Sanity checks for volume
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node->GetAssociatedNode());
  if (!volumeNode)
    {
    return -1;
    }
  vtkMRMLSelectionNode* selectionNode = qSlicerCoreApplication::application()->applicationLogic()->GetSelectionNode();
  if (!selectionNode)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::getDisplayVisibility: Unable to get selection node to show volume node " << node->GetName();
    return -1;
    }

  /// Update selection node based on current volumes visibility (if the selection is different in the slice viewers, then the first one is set)
  /// TODO: This is a workaround (http://www.na-mic.org/Bug/view.php?id=3551)
  this->updateSelectionNodeBasedOnCurrentVolumesVisibility();

  if ( ( selectionNode->GetActiveVolumeID() && !strcmp(volumeNode->GetID(), selectionNode->GetActiveVolumeID()) )
    || ( selectionNode->GetSecondaryVolumeID() && !strcmp(volumeNode->GetID(), selectionNode->GetSecondaryVolumeID()) ) )
    {
    return 1;
    }
  else
    {
    return 0;
    }
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPlugin::showVolume(vtkMRMLScalarVolumeNode* node, int visible/*=1*/)
{
  if (!node)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::showVolume: NULL node!";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  if (!scene)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::showVolume: Invalid MRML scene!";
    return;
    }
  vtkMRMLSelectionNode* selectionNode = qSlicerCoreApplication::application()->applicationLogic()->GetSelectionNode();
  if (!selectionNode)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::showVolume: Unable to get selection node to show volume node " << node->GetName();
    return;
    }

  // Get volume node
  vtkMRMLScalarVolumeNode* volumeNode = NULL;
  if ((volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node)) == NULL)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::showVolume: Node to show node is not a volume node: " << node->GetName();
    return;
    }

  /// Update selection node based on current volumes visibility (if the selection is different in the slice viewers, then the first one is set)
  /// TODO: This is a workaround (http://www.na-mic.org/Bug/view.php?id=3551)
  this->updateSelectionNodeBasedOnCurrentVolumesVisibility();

  // Show
  if (visible)
    {
    // If background is empty, show volume there
    if (!selectionNode->GetActiveVolumeID() || !strlen(selectionNode->GetActiveVolumeID()))
      {
      selectionNode->SetActiveVolumeID(volumeNode->GetID());
      }
    // If foreground is empty, show volume there
    else if (!selectionNode->GetSecondaryVolumeID() || !strlen(selectionNode->GetSecondaryVolumeID()))
      {
      selectionNode->SetSecondaryVolumeID(volumeNode->GetID());
      }
    // If both foreground and background are occupied, then bump background to foreground and show new volume in background
    else
      {
      // Hide volume that has been foreground
      // Needed so that visibility icon is updated (could be done in a faster way, but there is no noticeable overhead)
      vtkMRMLScalarVolumeNode* originalSecondaryVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(
        scene->GetNodeByID(selectionNode->GetSecondaryVolumeID()) );
      this->showVolume(originalSecondaryVolumeNode, 0);

      // Show volume that has been background in foreground
      vtkMRMLScalarVolumeNode* originalActiveVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(
        scene->GetNodeByID(selectionNode->GetActiveVolumeID()) );
      selectionNode->SetSecondaryVolumeID(originalActiveVolumeNode->GetID());

      // Show new volume as background
      selectionNode->SetActiveVolumeID(volumeNode->GetID());
      }

    // Set selection in slice views
    qSlicerCoreApplication::application()->applicationLogic()->PropagateVolumeSelection();

    // Make sure the secondary volume is shown in a semi-transparent way
    if (selectionNode->GetSecondaryVolumeID() && strlen(selectionNode->GetSecondaryVolumeID()))
      {
      vtkMRMLSliceCompositeNode* compositeNode = NULL;
      int numberOfCompositeNodes = scene->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
      for (int i=0; i<numberOfCompositeNodes; i++)
        {
        compositeNode = vtkMRMLSliceCompositeNode::SafeDownCast ( scene->GetNthNodeByClass( i, "vtkMRMLSliceCompositeNode" ) );
        if (compositeNode && compositeNode->GetForegroundOpacity() == 0.0)
          {
          compositeNode->SetForegroundOpacity(0.5);
          }
        }
      }
    }
  // Hide
  else
    {
    if ( selectionNode->GetActiveVolumeID() && !strcmp(volumeNode->GetID(), selectionNode->GetActiveVolumeID()) )
      {
      selectionNode->SetActiveVolumeID(NULL);
      }
    else if ( selectionNode->GetSecondaryVolumeID() && !strcmp(volumeNode->GetID(), selectionNode->GetSecondaryVolumeID()) )
      {
      selectionNode->SetSecondaryVolumeID(NULL);
      }
    // Set selection in slice views
    qSlicerCoreApplication::application()->applicationLogic()->PropagateVolumeSelection();
    }

  // Get subject hierarchy node for the volume node and have the scene model updated
  vtkMRMLSubjectHierarchyNode* subjectHierarchyNode = vtkMRMLSubjectHierarchyNode::GetAssociatedSubjectHierarchyNode(volumeNode);
  if (subjectHierarchyNode)
    {
    subjectHierarchyNode->Modified();
    }
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPlugin::updateSelectionNodeBasedOnCurrentVolumesVisibility()const
{
  vtkMRMLSelectionNode* selectionNode = qSlicerCoreApplication::application()->applicationLogic()->GetSelectionNode();
  if (!selectionNode)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::updateSelectionNodeBasedOnCurrentVolumesVisibility: Unable to get selection node";
    return;
    }

  // TODO: This is a workaround (http://www.na-mic.org/Bug/view.php?id=3551)

  // Determine background volume selection (if the selection is different in the slice viewers, then the first one is set)
  std::string selectedBackgroundVolumeID = this->getSelectedBackgroundVolumeNodeID();
  selectionNode->SetActiveVolumeID(selectedBackgroundVolumeID.c_str());

  // Determine foreground volume selection (if the selection is different in the slice viewers, then the first one is set)
  std::string selectedForegroundVolumeID = this->getSelectedForegroundVolumeNodeID();
  selectionNode->SetSecondaryVolumeID(selectedForegroundVolumeID.c_str());
}

//---------------------------------------------------------------------------
std::string qSlicerSubjectHierarchyVolumesPlugin::getSelectedBackgroundVolumeNodeID()const
{
  // TODO: This method is a workaround (http://www.na-mic.org/Bug/view.php?id=3551)
  std::string selectedBackgroundVolumeID("");

  vtkMRMLSelectionNode* selectionNode = qSlicerCoreApplication::application()->applicationLogic()->GetSelectionNode();
  if (!selectionNode)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::getSelectedBackgroundVolumeNodeID: Unable to get selection node";
    return selectedBackgroundVolumeID;
    }

  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  vtkMRMLSliceCompositeNode* compositeNode = NULL;
  const int numberOfCompositeNodes = scene->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");

  for (int i=0; i<numberOfCompositeNodes; i++)
    {
    compositeNode = vtkMRMLSliceCompositeNode::SafeDownCast ( scene->GetNthNodeByClass( i, "vtkMRMLSliceCompositeNode" ) );
    if (compositeNode && compositeNode->GetBackgroundVolumeID() && selectedBackgroundVolumeID.empty())
      {
      selectedBackgroundVolumeID = std::string(compositeNode->GetBackgroundVolumeID());
      break;
      }
    }

  return selectedBackgroundVolumeID;
}

//---------------------------------------------------------------------------
std::string qSlicerSubjectHierarchyVolumesPlugin::getSelectedForegroundVolumeNodeID()const
{
  // TODO: This method is a workaround (http://www.na-mic.org/Bug/view.php?id=3551)
  std::string selectedForegroundVolumeID("");

  vtkMRMLSelectionNode* selectionNode = qSlicerCoreApplication::application()->applicationLogic()->GetSelectionNode();
  if (!selectionNode)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::getSelectedForegroundVolumeNodeID: Unable to get selection node";
    return selectedForegroundVolumeID;
    }

  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
  vtkMRMLSliceCompositeNode* compositeNode = NULL;
  const int numberOfCompositeNodes = scene->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");

  for (int i=0; i<numberOfCompositeNodes; i++)
    {
    compositeNode = vtkMRMLSliceCompositeNode::SafeDownCast ( scene->GetNthNodeByClass( i, "vtkMRMLSliceCompositeNode" ) );
    if (compositeNode && compositeNode->GetForegroundVolumeID() && selectedForegroundVolumeID.empty())
      {
      selectedForegroundVolumeID = std::string(compositeNode->GetForegroundVolumeID());
      break;
      }
    }

  return selectedForegroundVolumeID;
}

//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyVolumesPlugin::nodeContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyVolumesPlugin);

  QList<QAction*> actions;
  actions << d->ShowVolumesInBranchAction << d->FeedImageSeriesAction << d->StartSegmentationAction;
  return actions;
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPlugin::showContextMenuActionsForNode(vtkMRMLSubjectHierarchyNode* node)
{
  Q_D(qSlicerSubjectHierarchyVolumesPlugin);
  this->hideAllContextMenuActions();

  if (!node)
    {
    // There are no scene actions in this plugin
    return;
    }

  // Volume
  if (this->canOwnSubjectHierarchyNode(node))
    {
    // No scalar volume context menu
    }

  // Folders (Patient, Study, Folder)
  if ( node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPatient())
    || node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRCourse())
    || node->IsLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelFolder()) )
    {
    d->ShowVolumesInBranchAction->setVisible(true);
    }

  // If the the SH Node Owner Plugin is  Volumes,
  // Show the FeedImageSeriesAction . by zoulian
  vtkStdString pluginName = node->GetOwnerPluginName();

  if (!strcmp(pluginName.c_str(), "Volumes"))
  {
	  d->FeedImageSeriesAction->setVisible(true);

	  if (node->GetAssociatedNode())
	  {
		  d->StartSegmentationAction->setVisible(true);
	  }
  }

}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPlugin::showVolumesInBranch()
{
  vtkMRMLSelectionNode* selectionNode = qSlicerCoreApplication::application()->applicationLogic()->GetSelectionNode();
  if (!selectionNode)
    {
    qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::showVolumesInBranch: Unable to get selection node";
    return;
    }

  std::set<vtkMRMLSubjectHierarchyNode*> subjectHierarchyNodesToUpdate;
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();

  // Collect subject hierarchy nodes of currently shown volumes to be able to update them in the scene model
  vtkMRMLSubjectHierarchyNode* activeVolumeShNode = vtkMRMLSubjectHierarchyNode::GetAssociatedSubjectHierarchyNode(
    scene->GetNodeByID(selectionNode->GetActiveVolumeID()) );
  subjectHierarchyNodesToUpdate.insert(activeVolumeShNode);
  vtkMRMLSubjectHierarchyNode* secondaryVolumeShNode = vtkMRMLSubjectHierarchyNode::GetAssociatedSubjectHierarchyNode(
    scene->GetNodeByID(selectionNode->GetSecondaryVolumeID()) );
  subjectHierarchyNodesToUpdate.insert(secondaryVolumeShNode);

  // Hide all volumes before showing the ones from the study
  selectionNode->SetActiveVolumeID(NULL);
  selectionNode->SetSecondaryVolumeID(NULL);

  // Show volumes in study
  vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
  vtkSmartPointer<vtkCollection> childVolumeNodes = vtkSmartPointer<vtkCollection>::New();
  vtkMRMLSubjectHierarchyNode::SafeDownCast(currentNode)->GetAssociatedChildrenNodes(childVolumeNodes, "vtkMRMLScalarVolumeNode");
  childVolumeNodes->InitTraversal();
  for (int i=0; i<childVolumeNodes->GetNumberOfItems(); ++i)
    {
    vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(childVolumeNodes->GetItemAsObject(i));
    if (volumeNode)
      {
      // Get subject hierarchy node for the volume node
      vtkMRMLSubjectHierarchyNode* volumeSubjectHierarchyNode = vtkMRMLSubjectHierarchyNode::GetAssociatedSubjectHierarchyNode(volumeNode);
      if (!volumeSubjectHierarchyNode)
        {
        qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::showVolumesInBranch: Unable to get subject hierarchy node!";
        continue;
        }

      // Show first two volume in study only
      if (!selectionNode->GetActiveVolumeID())
        {
        selectionNode->SetActiveVolumeID(volumeNode->GetID());
        subjectHierarchyNodesToUpdate.insert(volumeSubjectHierarchyNode);
        }
      else if (!selectionNode->GetSecondaryVolumeID())
        {
        selectionNode->SetSecondaryVolumeID(volumeNode->GetID());
        subjectHierarchyNodesToUpdate.insert(volumeSubjectHierarchyNode);

        // Make sure the secondary volume is shown in a semi-transparent way
        vtkMRMLSliceCompositeNode* compositeNode = NULL;
        int numberOfCompositeNodes = scene->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
        for (int i=0; i<numberOfCompositeNodes; i++)
          {
          compositeNode = vtkMRMLSliceCompositeNode::SafeDownCast ( scene->GetNthNodeByClass( i, "vtkMRMLSliceCompositeNode" ) );
          if (compositeNode && compositeNode->GetForegroundOpacity() == 0.0)
            {
            compositeNode->SetForegroundOpacity(0.5);
            }
          }
        }
      }
    }

  qSlicerCoreApplication::application()->applicationLogic()->PropagateVolumeSelection();

  // Update scene model for subject hierarchy nodes just hidden
  for (std::set<vtkMRMLSubjectHierarchyNode*>::iterator volumeShNodeIt = subjectHierarchyNodesToUpdate.begin();
    volumeShNodeIt != subjectHierarchyNodesToUpdate.end(); ++volumeShNodeIt)
    {
    if (*volumeShNodeIt)
      {
      (*volumeShNodeIt)->Modified();
      }
    }

}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyVolumesPlugin::editProperties(vtkMRMLSubjectHierarchyNode* node)
{

	Q_UNUSED(node);


  /* Comment out by zoulian

  // Switch to volumes module and volume already selected
  qSlicerAbstractModuleWidget* moduleWidget = qSlicerSubjectHierarchyAbstractPlugin::switchToModule("Volumes");
  if (moduleWidget)
    {
    // Get node selector combobox
    qMRMLNodeComboBox* nodeSelector = moduleWidget->findChild<qMRMLNodeComboBox*>("ActiveVolumeNodeSelector");

    // Choose current data node
    if (nodeSelector)
      {
      nodeSelector->setCurrentNode(node->GetAssociatedNode()); 
      }
    }

	*/
}



//-----------------------------------------------------------------------
// Attach the vtkMRMLScalarVolumeNode tho the primaryImageVolumeSubjectNode if its associatedNode is null

bool qSlicerSubjectHierarchyVolumesPlugin::addNodeToSubjectHierarchy(vtkMRMLNode* node, vtkMRMLSubjectHierarchyNode* parent,const char* level )
{
	//associate the volumeNode to the ImageVolumeSubjectNode
	// Added by zoulian


	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	if (!scene)
	{
		qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::addNodeToSubjectHierarchy: Invalid MRML scene!";
		return false;
	}

	vtkMRMLSubjectHierarchyNode* primaryImageVolumeSHNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene, vtkMRMLSubjectHierarchyConstants::GetSRPlanImageVolumeUIDName(), vtkMRMLSubjectHierarchyConstants::GetSRPlanPrimaryImageVolumeUID());

	//if Create the Volume SH from subject hierarchy Module
	vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();

	if (currentNode && !currentNode->GetAssociatedNode())
	{
		currentNode->SetAssociatedNodeID(node->GetID());
		return true;
	}

	//if the primaryImage SHNode is not null and associated Node is Null
	if (primaryImageVolumeSHNode && !primaryImageVolumeSHNode->GetAssociatedNode())
	{

	//	char * volumeNodeID = node->GetID();
		primaryImageVolumeSHNode->SetAssociatedNodeID(node->GetID());

	//	this->setDisplayVisibility(primaryImageVolumeSHNode, true);
	//    primaryImageVolumeSHNode->Modified();

		node->Modified();

		return true;

	}
	else
	{
		return  this->Superclass::addNodeToSubjectHierarchy(node, NULL);
	 
	}
}

//feed Image Series into Subject Hierarchy Node
bool qSlicerSubjectHierarchyVolumesPlugin::feedImageSeriesIntoSHNode()
{

	qSlicerIOManager *ioManager = qSlicerApplication::application()->ioManager();
	if (!ioManager)
	{
		return false;
	}
	return ioManager->openAddDataDialog();
}





bool qSlicerSubjectHierarchyVolumesPlugin::addStructureSetToSHNode()
{

	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();
	if (!scene)
	{
		qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::addNodeToSubjectHierarchy: Invalid MRML scene!";
		return false;
	}

	//Get the Assigned Volume
	vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode();
	if (!currentNode->GetAssociatedNode())
	{
		qCritical() << "qSlicerSubjectHierarchyVolumesPlugin::addStructureSetToSHNode: Invalid Image Volume SH Node!";
		return false;
	}


	vtkMRMLSubjectHierarchyNode* StructureSetNode = vtkMRMLSubjectHierarchyNode::New();

	StructureSetNode->SetLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRSubplan());


	StructureSetNode->SetOwnerPluginName("Segmentations");

	StructureSetNode->SetName(scene->GetUniqueNameByString(vtkMRMLSubjectHierarchyConstants::GetSRPlanStructureSetNodeBaseName()));


	vtkMRMLSubjectHierarchyNode * parent = currentNode->GetAncestorAtLevel(vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRPlan());

	//Assign the ScalarVolumeNode ID  to 
	StructureSetNode->SetParentNodeID(parent->GetID());


	// Relate StructureSetSH  to  ImageVolumeSH
	StructureSetNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSHImageVolumeUIDName(), currentNode->GetID());

	//AddNode generates SH Node ID
	scene->AddNode(StructureSetNode);

	currentNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSHStructureSetUIDName(), StructureSetNode->GetID());

	
	StructureSetNode->Delete(); // Return ownership to t	
	return true;

}




void qSlicerSubjectHierarchyVolumesPlugin::startSegmentation()
{

	vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->scene();

	if (!scene)
	{
		qCritical() << "qSlicerSubjectHierarchySegmentationsPlugin::startSegmentation: Invalid MRML scene!";

	}

	

	vtkMRMLSubjectHierarchyNode* currentNode = qSlicerSubjectHierarchyPluginHandler::instance()->currentNode(); //the ImageSH Node

	vtkMRMLSubjectHierarchyNode* parentNode = vtkMRMLSubjectHierarchyNode::SafeDownCast(currentNode->GetParentNode()); //the plan SH Node

	vtkStdString ssuid = currentNode->GetUID(vtkMRMLSubjectHierarchyConstants::GetSHStructureSetUIDName());
	vtkMRMLSubjectHierarchyNode* SegmentationSH = vtkMRMLSubjectHierarchyNode::SafeDownCast(scene->GetNodeByID(ssuid)); //Get The Associated Segmetation of Image Volume

	//New a tempLabelMap add to scene and
	vtkMRMLLabelMapVolumeNode * templabelmapnode = vtkMRMLLabelMapVolumeNode::New();
	scene->AddNode(templabelmapnode);


	vtkMRMLSelectionNode * selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));

	

	if (SegmentationSH)
	{
		

		// Switch to segmentations module and select node
		qSlicerAbstractModuleWidget* moduleWidget = qSlicerSubjectHierarchyAbstractPlugin::switchToModule("Segmentations");
		if (moduleWidget)
		{
			// Get node selector combobox
			qMRMLNodeComboBox* nodeSelector = moduleWidget->findChild<qMRMLNodeComboBox*>("MRMLNodeComboBox_Segmentation");

			// Choose current data node
			if (nodeSelector && SegmentationSH)
			{
				nodeSelector->setCurrentNode(SegmentationSH->GetAssociatedNode());
			}
			
			selectionNode->SetActiveSegmentationID(SegmentationSH->GetAssociatedNode()->GetID());
			
		}


	}
	else 
	{
		vtkMRMLSegmentationNode * segmentation = vtkMRMLSegmentationNode::New();

		segmentation->SetName(scene->GetUniqueNameByString("StructureSet"));


		scene->AddNode(segmentation);


		vtkStdString nodename = scene->GetUniqueNameByString(vtkMRMLSubjectHierarchyConstants::GetSRPlanStructureSetNodeBaseName());

		SegmentationSH = vtkMRMLSubjectHierarchyNode::CreateSubjectHierarchyNode(scene, parentNode,
			vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyLevelSRSubplan(), nodename, segmentation);


		// Start to Related Structure and ImageVolume
		SegmentationSH->AddUID(vtkMRMLSubjectHierarchyConstants::GetSHImageVolumeUIDName(), currentNode->GetID());
		currentNode->AddUID(vtkMRMLSubjectHierarchyConstants::GetSHStructureSetUIDName(), SegmentationSH->GetID());

		//Creat and add LabelVolume to Scene and used for Lable Segmentation
		vtkMRMLVolumeNode * mastervolume = vtkMRMLVolumeNode::SafeDownCast(currentNode->GetAssociatedNode());

		//Add ID to ParametersNode

		vtkSlicerSegmentationsModuleLogic::



		vtkStdString basename = mastervolume->GetName();

		
		//UpDate the SelectionNode,save the plan primaryVolume and ActiveSegmentation

		selectionNode->SetActiveSegmentationID(segmentation->GetID());
		selectionNode->SetPlanPrimaryVolumeID(mastervolume->GetID());

		// related templabelmapnode to SegmentationSH
		SegmentationSH->AddUID(vtkMRMLSubjectHierarchyConstants::GetTempLabelMapUIDName(), templabelmapnode->GetID());


		//Initial the Temp LabelMap Node by  VolumesLogic
	    vtkSlicerVolumesLogic::CreateTempLabelVolumeFromVolume(scene, templabelmapnode, mastervolume);


 
		// Switch to segmentations module and select node
		qSlicerAbstractModuleWidget* moduleWidget = qSlicerSubjectHierarchyAbstractPlugin::switchToModule("Segmentations");
		if (moduleWidget)
		{
			// Get node selector combobox
			qMRMLNodeComboBox* nodeSelector = moduleWidget->findChild<qMRMLNodeComboBox*>("MRMLNodeComboBox_Segmentation");

			// Choose current data node
			if (nodeSelector)
			{
				nodeSelector->setCurrentNode(segmentation);
			}
		}


	}


}