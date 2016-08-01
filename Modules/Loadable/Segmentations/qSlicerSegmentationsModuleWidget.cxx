/*==============================================================================

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

// Segmentations includes
#include "qSlicerSegmentationsModuleWidget.h"
#include "ui_qSlicerSegmentationsModule.h"

#include "vtkMRMLSegmentationNode.h"
#include "vtkMRMLSegmentationDisplayNode.h"
#include "vtkSlicerSegmentationsModuleLogic.h"

#include "vtkSlicerVolumesLogic.h"

#include "qMRMLSegmentsTableView.h"
#include "qMRMLSegmentationRepresentationsListView.h"
#include "qSRPlanSegmentationDisplaySettingsDialog.h"

#include "qMRMLSegmentsEditorLogic.h"
#include "vtkMRMLGeneralParametersNode.h"
#include "qMRMLSegmentsTableView.h"

// SlicerQt includes
#include <qSlicerApplication.h>

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLLabelMapVolumeNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLSubjectHierarchyNode.h"

#include "vtkMRMLColorTableNode.h"

// VTK includes
#include <vtkSmartPointer.h>
#include "vtkImageData.h"
#include "vtkDataObject.h"


// Qt includes
#include <QDebug>
#include <QMessageBox>

//-----------------------------------------------------------------------------
/// \ingroup SlicerRt_QtModules_Segmentations
class qSlicerSegmentationsModuleWidgetPrivate: public Ui_qSlicerSegmentationsModule
{
  Q_DECLARE_PUBLIC(qSlicerSegmentationsModuleWidget);
protected:
  qSlicerSegmentationsModuleWidget* const q_ptr;
public:
  qSlicerSegmentationsModuleWidgetPrivate(qSlicerSegmentationsModuleWidget& object);
  ~qSlicerSegmentationsModuleWidgetPrivate();
  vtkSlicerSegmentationsModuleLogic* logic() const;
public:
  /// Using this flag prevents overriding the parameter set node contents when the
  ///   QMRMLCombobox selects the first instance of the specified node type when initializing
  bool ModuleWindowInitialized;
};

//-----------------------------------------------------------------------------
// qSlicerSegmentationsModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerSegmentationsModuleWidgetPrivate::qSlicerSegmentationsModuleWidgetPrivate(qSlicerSegmentationsModuleWidget& object)
  : q_ptr(&object)
  , ModuleWindowInitialized(false)
{
}

//-----------------------------------------------------------------------------
qSlicerSegmentationsModuleWidgetPrivate::~qSlicerSegmentationsModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerSegmentationsModuleLogic*
qSlicerSegmentationsModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerSegmentationsModuleWidget);
  return vtkSlicerSegmentationsModuleLogic::SafeDownCast(q->logic());
} 

//-----------------------------------------------------------------------------
// qSlicerSegmentationsModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerSegmentationsModuleWidget::qSlicerSegmentationsModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerSegmentationsModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerSegmentationsModuleWidget::~qSlicerSegmentationsModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onEnter()
{
  if (!this->mrmlScene())
  {
    qCritical() << "qSlicerSegmentationsModuleWidget::onEnter: Invalid scene!";
    return;
  }

  Q_D(qSlicerSegmentationsModuleWidget);

  d->ModuleWindowInitialized = true;

  this->onSegmentationNodeChanged( d->MRMLNodeComboBox_Segmentation->currentNode() );
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::setup()
{
  this->init();
}

//-----------------------------------------------------------------------------
vtkMRMLSegmentationDisplayNode* qSlicerSegmentationsModuleWidget::segmentationDisplayNode()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationNode* segmentationNode =  vtkMRMLSegmentationNode::SafeDownCast(
    d->MRMLNodeComboBox_Segmentation->currentNode() );
  if (!segmentationNode)
  {
    return NULL;
  }

  return vtkMRMLSegmentationDisplayNode::SafeDownCast( segmentationNode->GetDisplayNode() );
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  // Update display group from segmentation display node
  this->updateWidgetFromDisplayNode();

  // Update copy/move/import/export buttons from selection
  this->updateCopyMoveButtonStates();

  // Update segment handler button states based on segment selection
  this->onSegmentSelectionChanged(QItemSelection(),QItemSelection());
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::updateWidgetFromDisplayNode()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    return;
  }

  /*
  // Update display property widgets
  d->checkBox_Visible->setChecked( displayNode->GetVisibility() );
  d->SliderWidget_Opacity->setValue( displayNode->GetOpacity() );
  d->checkBox_SliceIntersectionVisible->setChecked( displayNode->GetSliceIntersectionVisibility() );
  d->spinBox_SliceIntersectionThickness->setValue( displayNode->GetSliceIntersectionThickness() );

  // Populate model representations combobox and select preferred poly data representation
  this->populateRepresentationsCombobox();

  // Set display node to display widgets
  d->DisplayNodeViewComboBox->setMRMLDisplayNode(displayNode);

  */
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::updateCopyMoveButtonStates()
{
  Q_D(qSlicerSegmentationsModuleWidget);
  /*
  // Disable copy/move buttons then enable later based on selection
  d->toolButton_MoveFromCurrentSegmentation->setEnabled(false);
  d->toolButton_CopyFromCurrentSegmentation->setEnabled(false);
  d->toolButton_CopyToCurrentSegmentation->setEnabled(false);
  d->toolButton_MoveToCurrentSegmentation->setEnabled(false);

  // Set button states that copy/move to current segmentation
  vtkMRMLSegmentationNode* otherSegmentationNode = vtkMRMLSegmentationNode::SafeDownCast(
    d->SegmentsTableView_Other->segmentationNode() );
  vtkMRMLNode* otherRepresentationNode = d->SegmentsTableView_Other->representationNode();
  if (otherSegmentationNode)
  {
    // All options are possible if other node is segmentation
    d->toolButton_CopyToCurrentSegmentation->setEnabled(true);
    d->toolButton_MoveToCurrentSegmentation->setEnabled(true);
  }
  else if (otherRepresentationNode)
  {
    // Move from other node is disabled if other node is representation
    d->toolButton_CopyToCurrentSegmentation->setEnabled(true);
  }

  // Set button states that copy/move from current segmentation
  vtkMRMLSegmentationNode* currentSegmentationNode =  vtkMRMLSegmentationNode::SafeDownCast(
    d->MRMLNodeComboBox_Segmentation->currentNode() );
  if (currentSegmentationNode && currentSegmentationNode->GetSegmentation()->GetNumberOfSegments() > 0)
  {
    d->toolButton_MoveFromCurrentSegmentation->setEnabled(true);
    d->toolButton_CopyFromCurrentSegmentation->setEnabled(true);
  }
  */
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::populateRepresentationsCombobox()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  // Note: This function used to collect existing poly data representations from
  // the segmentation and was connected to events like this:
  // qvtkConnect( segmentationNode, vtkSegmentation::RepresentationCreated, this, SLOT( populateRepresentationsCombobox() ) );
  // It was then decided that a preferred poly data representation can be selected
  // regardless its existence, thus the combobox is populated only once at initialization.

  // Prevent selecting incrementally added representations thus changing MRML properties
  /*
  d->comboBox_DisplayedModelRepresentation->blockSignals(true);
  d->comboBox_DisplayedModelRepresentation->clear();

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    d->comboBox_DisplayedModelRepresentation->blockSignals(false);
    return;
  }

  // Populate model representations combobox
  std::set<std::string> modelRepresentationNames;
  displayNode->GetPolyDataRepresentationNames(modelRepresentationNames);
  for (std::set<std::string>::iterator reprIt = modelRepresentationNames.begin();
    reprIt != modelRepresentationNames.end(); ++reprIt)
  {
    d->comboBox_DisplayedModelRepresentation->addItem(reprIt->c_str());
  }

  // Unblock signals
  d->comboBox_DisplayedModelRepresentation->blockSignals(false);

  // Set selection from display node
  d->comboBox_DisplayedModelRepresentation->setCurrentIndex( d->comboBox_DisplayedModelRepresentation->findText(
    displayNode->GetPreferredPolyDataDisplayRepresentationName() ) );

	*/
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::init()
{
  Q_D(qSlicerSegmentationsModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();


  //Setup the qMRMLSegmentsEditorWidget and qMRMLSegmentsEditorLogic by zoulian

  qMRMLSegmentsEditorLogic* editorlogic = vtkSlicerSegmentationsModuleLogic::SafeDownCast(this->logic())->GetEditorLogic();
  d->EditorBox->SetSegmentsEditorLogic(editorlogic);
   
  // Make connections
  connect(d->MRMLNodeComboBox_Segmentation, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onSegmentationNodeChanged(vtkMRMLNode*)) );
  connect(d->MRMLNodeComboBox_Segmentation, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    d->SegmentsTableView, SLOT(setSegmentationNode(vtkMRMLNode*)) );
 
 

  connect(d->SegmentsTableView, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
    this, SLOT(onSegmentSelectionChanged(QItemSelection,QItemSelection)));
  connect(d->pushButton_AddSegment, SIGNAL(clicked()),
    this, SLOT(onAddSegment()) );

  connect(d->pushButton_DeleteSelected, SIGNAL(clicked()),
    this, SLOT(onDeleteSelectedSegments()) );

  connect(d->pushButton_Settings, SIGNAL(clicked()),
	  this, SLOT(onSetDisplaySettings()));



}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onSegmentationNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << "qSlicerSegmentationsModuleWidget::segmentationNodeChanged: Invalid scene!";
    return;
  }
  if (!d->ModuleWindowInitialized)
  {
    return;
  }

  // Disconnect all nodes from functions connected in a QVTK way
  qvtkDisconnect( 0, vtkCommand::ModifiedEvent, this, SLOT( updateWidgetFromMRML() ) );
  qvtkDisconnect( 0, vtkMRMLDisplayableNode::DisplayModifiedEvent, this, SLOT( updateWidgetFromDisplayNode() ) );
  qvtkDisconnect( 0, vtkSegmentation::MasterRepresentationModified, this, SLOT( updateWidgetFromMRML() ) );

  vtkMRMLSegmentationNode* segmentationNode =  vtkMRMLSegmentationNode::SafeDownCast(node);
  if (segmentationNode)
  {
    // Connect node modified events to update widgets in display group function
    qvtkConnect( segmentationNode, vtkCommand::ModifiedEvent, this, SLOT( updateWidgetFromMRML() ) );
    qvtkConnect( segmentationNode, vtkMRMLDisplayableNode::DisplayModifiedEvent, this, SLOT( updateWidgetFromDisplayNode() ) );
    qvtkConnect( segmentationNode, vtkSegmentation::MasterRepresentationModified, this, SLOT( updateWidgetFromMRML() ) );
  }

  // Hide the current node in the other segmentation combo box
  QStringList hiddenNodeIDs;
  if (segmentationNode)
  {
    hiddenNodeIDs << QString(segmentationNode->GetID());
  }
//  d->MRMLNodeComboBox_OtherSegmentationOrRepresentationNode->sortFilterProxyModel()->setHiddenNodeIDs(hiddenNodeIDs);

  // Update UI from selected segmentation node
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::selectSegmentationNode(vtkMRMLSegmentationNode* segmentationNode)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  d->MRMLNodeComboBox_Segmentation->setCurrentNode(segmentationNode);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onSegmentSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);
  Q_D(qSlicerSegmentationsModuleWidget);

  QStringList selectedSegmentIds = d->SegmentsTableView->selectedSegmentIDs();
//  d->pushButton_EditSelected->setEnabled(selectedSegmentIds.count() == 1);

  if (selectedSegmentIds.count() > 0) 
  {
	  d->pushButton_DeleteSelected->setEnabled(true);


	  // All items contain the segment ID, get that
	  QString segmentId = selectedSegmentIds[0];
	  vtkMRMLSegmentationNode * segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(d->MRMLNodeComboBox_Segmentation->currentNode());

	  vtkSegment* segment = segmentationNode->GetSegmentation()->GetSegment(segmentId.toLatin1().constData());

	  //Update the ParametersNode
	  vtkMRMLScene * scene = qSlicerCoreApplication::application()->mrmlScene();
	  vtkMRMLGeneralParametersNode* parametersNode = vtkSlicerSegmentationsModuleLogic::GetParametersNode(scene);

	  parametersNode->SetParameter("segment", segmentId.toStdString());

	  QString lable = QString::number(segment->GetLabel());
	  parametersNode->SetParameter("label", lable.toStdString()); //current label value	


     //Get the SegmentationModuleLogic and  get the selected segment master representation related LabelMapVolumeNode

	  vtkSlicerSegmentationsModuleLogic * modulelogic = vtkSlicerSegmentationsModuleLogic::SafeDownCast(this->logic());

	  vtkImageData * imagedata = vtkImageData::SafeDownCast( segment->GetRepresentation(vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName()));
	  vtkMRMLLabelMapVolumeNode *  currentlabelnode = modulelogic->GetLabelMapVolumeNodebyImageData(scene, imagedata);


	 // Set the Editor Logic Label,used for paintbrush effect;
	 // Update the LabelMapNode the current segment's Representation Node

	  qMRMLSegmentsEditorLogic* editorlogic = vtkSlicerSegmentationsModuleLogic::SafeDownCast(this->logic())->GetEditorLogic();

	  editorlogic->SetLabelMapNodetoLayoutCompositeNode("Red", currentlabelnode);

	  editorlogic->SetLabel(segment->GetLabel());

	  // Set the LabelMap to Current Segment LabelMap

  }
 

}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onAddSegment()
{
  Q_D(qSlicerSegmentationsModuleWidget);


  //Get the SegmentationModuleLogic and SegmentsEditorLogic

  vtkSlicerSegmentationsModuleLogic * modulelogic = vtkSlicerSegmentationsModuleLogic::SafeDownCast( this->logic());
  qMRMLSegmentsEditorLogic *editorlogic = modulelogic->GetEditorLogic();


  //Get the Active Scene and Set the Scene to editorlogic, if editorlogic scene is Null
  vtkMRMLScene * scene = this->mrmlScene();
  if (!editorlogic->GetMRMLScene() && scene)
  {
	  editorlogic->SetMRMLScene(scene);
  }


  //Get the CurrentSegmentation Node
  vtkMRMLSegmentationNode* currentSegmentationNode = vtkMRMLSegmentationNode::SafeDownCast(
	  d->MRMLNodeComboBox_Segmentation->currentNode());
  if (!currentSegmentationNode)
  {
	  return;
  }

  //Ensure There is a LabelMapColorTable in the paramters
  if (!vtkSlicerSegmentationsModuleLogic::HasLabelMapColorTableNode(scene))
  {
	  vtkSlicerSegmentationsModuleLogic::AddColorTabelNodeToParametersNode(scene);
  }


  vtkMRMLColorTableNode * ctNode = vtkSlicerSegmentationsModuleLogic::GetColorTabelNodeFromParametersNode(scene);

  

  // Create empty segment,and an  empty vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName() type 
  // Presentation,  Add empty segment to current segmentation
  vtkSegment * emptySegment = currentSegmentationNode->GetSegmentation()->AddEmptySegmentAndReturn();


  //Add current added vtkSegment default Display properties to ParametersNode' ColorTableNode
  char * name=  emptySegment->GetName();
  double* defaultColor =   emptySegment->GetDefaultColor();
  ctNode->AddColor(name, defaultColor[0], defaultColor[1], defaultColor[2], defaultColor[3]);

    
  //Get the vtkOrientedImageData pointer,  the main presentation of emptySegment
  vtkDataObject*  presentation = emptySegment->GetRepresentation(vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName());

  //Get the Segmentation related Primary Volume Image
  vtkMRMLScalarVolumeNode * primaryvolume = vtkMRMLScalarVolumeNode::SafeDownCast(modulelogic->GetRelatedVolumeNodeFromSegmentationNode(scene, currentSegmentationNode));

  vtkOrientedImageData* orientedimage = modulelogic->CreateOrientedImageDataFromVolumeNode(primaryvolume);


  //Creat the LabelMapVolume,Set the segement related name
  //set the labelMapVolume ImageData as the segment representation
  //Set the DisplayNode ColorTable as Segmentation ColorTable
  vtkMRMLLabelMapVolumeNode *  labelnode = vtkMRMLLabelMapVolumeNode::New();
  QString labname = emptySegment->GetName();
  labname = QString(primaryvolume->GetName()) + QString("-") +labname + QString(modulelogic->GetSegmentLabelMapVolumeNameSuffix());

  labelnode->SetName(labname.toStdString().c_str());

  scene->AddNode(labelnode);


  modulelogic->CreateLabelmapVolumeFromOrientedImageData(orientedimage, labelnode);

  vtkSlicerVolumesLogic::ClearVolumeImageData(labelnode); //initial to zero

 
  emptySegment->AddRepresentation(vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName(), labelnode->GetImageData());

 //LabelMapVolume DisplayNode use the SegmentationNode CTNode
 // char* colorid = currentSegmentationNode->GetDisplayNode()->GetColorNodeID();
 // labelnode->GetDisplayNode()->SetAndObserveColorNodeID(colorid);


  //Use the ColorTableNode of Parameters
  labelnode->GetDisplayNode()->SetAndObserveColorNodeID(ctNode->GetID());

  editorlogic->SetLabelMapNodetoLayoutCompositeNode("Red", labelnode);
	 

  //Enable the Segmentation Effect Buttons

  d->EditorBox->EnableEffectButtons();
  
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onEditSelectedSegment()
{
  //TODO:
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onDeleteSelectedSegments()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationNode* currentSegmentationNode =  vtkMRMLSegmentationNode::SafeDownCast(
    d->MRMLNodeComboBox_Segmentation->currentNode() );
  if (!currentSegmentationNode)
  {
    return;
  }

  QStringList selectedSegmentIds = d->SegmentsTableView->selectedSegmentIDs();
  foreach (QString segmentId, selectedSegmentIds)
  {
    currentSegmentationNode->GetSegmentation()->RemoveSegment(segmentId.toLatin1().constData());
  }
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::setOtherSegmentationOrRepresentationNode(vtkMRMLNode* node)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  if (!this->mrmlScene())
  {
    qCritical() << "qSlicerSegmentationsModuleWidget::setOtherSegmentationOrRepresentationNode: Invalid scene!";
    return;
  }
  if (!d->ModuleWindowInitialized)
  {
    return;
  }

  // Decide if segmentation or representation node
  vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(node);
  vtkMRMLLabelMapVolumeNode* labelmapNode = vtkMRMLLabelMapVolumeNode::SafeDownCast(node);
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(node);

  if (segmentationNode)
  {
 //   d->SegmentsTableView_Other->setMode(qMRMLSegmentsTableView::SimpleListMode);
 //   d->SegmentsTableView_Other->setSegmentationNode(node);
  }
  else if (labelmapNode || modelNode)
  {
 //   d->SegmentsTableView_Other->setMode(qMRMLSegmentsTableView::RepresentationMode);
 //   d->SegmentsTableView_Other->setRepresentationNode(node);
  }
  else
  {
//    d->SegmentsTableView_Other->setSegmentationNode(NULL);
 //   d->SegmentsTableView_Other->setRepresentationNode(NULL);
  }

  // Update widgets based on selection
  this->updateCopyMoveButtonStates();
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onVisibilityChanged(int visible)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    return;
  }

  displayNode->SetVisibility(visible > 0 ? 1 : 0);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onOpacityChanged(double opacity)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    return;
  }

  displayNode->SetOpacity(opacity);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onSliceIntersectionVisibilityChanged(int visible)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    return;
  }

  displayNode->SetSliceIntersectionVisibility(visible > 0 ? 1 : 0);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onSliceIntersectionThicknessChanged(int thickness)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    return;
  }

  displayNode->SetSliceIntersectionThickness(thickness);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onDisplayedModelRepresentationChanged(int index)
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
  if (!displayNode)
  {
    return;
  }

  // Get representation name from index
//  QString representationName = d->comboBox_DisplayedModelRepresentation->itemText(index);

//  displayNode->SetPreferredPolyDataDisplayRepresentationName(representationName.toLatin1().constData());
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onAddLabelmap()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLScene* scene = this->mrmlScene();
  if (!scene)
  {
    return;
  }

  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> labelmapNode = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
  scene->AddNode(labelmapNode);

  // Select new labelmap in the other representation combobox
//  d->MRMLNodeComboBox_OtherSegmentationOrRepresentationNode->setCurrentNode(labelmapNode);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onAddModel()
{
  Q_D(qSlicerSegmentationsModuleWidget);

  vtkMRMLScene* scene = this->mrmlScene();
  if (!scene)
  {
    return;
  }

  vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
  scene->AddNode(modelNode);

  // Select new model in the other representation table
//  d->MRMLNodeComboBox_OtherSegmentationOrRepresentationNode->setCurrentNode(modelNode);
}

//-----------------------------------------------------------------------------
bool qSlicerSegmentationsModuleWidget::copySegmentBetweenSegmentations(
  vtkSegmentation* fromSegmentation, vtkSegmentation* toSegmentation,
  QString segmentId, bool removeFromSource/*=false*/ )
{
  if (!fromSegmentation || !toSegmentation || segmentId.isEmpty())
  {
    return false;
  }

  std::string segmentIdStd(segmentId.toLatin1().constData());

  // Get segment
  vtkSegment* segment = fromSegmentation->GetSegment(segmentIdStd);
  if (!segment)
  {
    qCritical() << "qSlicerSegmentationsModuleWidget::copySegmentBetweenSegmentations: Failed to get segment!";
    return false;
  }

  // If target segmentation is newly created thus have no master representation, make it match the source
  if (!toSegmentation->GetMasterRepresentationName())
  {
    toSegmentation->SetMasterRepresentationName(fromSegmentation->GetMasterRepresentationName());
  }

  // Check whether target is suitable to accept the segment.
  if (!toSegmentation->CanAcceptSegment(segment))
  {
    qCritical() << "qSlicerSegmentationsModuleWidget::copySegmentBetweenSegmentations: Segmentation cannot accept segment " << segment->GetName() << "!";

    // Pop up error message to the user explaining the problem
    vtkMRMLSegmentationNode* fromNode = vtkSlicerSegmentationsModuleLogic::GetSegmentationNodeForSegmentation(this->mrmlScene(), fromSegmentation);
    vtkMRMLSegmentationNode* toNode = vtkSlicerSegmentationsModuleLogic::GetSegmentationNodeForSegmentation(this->mrmlScene(), toSegmentation);
    if (!fromNode || !toNode) // Sanity check, should never happen
    {
      qCritical() << "qSlicerSegmentationsModuleWidget::copySegmentBetweenSegmentations: Unable to get parent nodes for segmentaiton objects!";
      return false;
    }

    QString message = QString("Cannot convert source master representation '%1' into target master '%2', thus unable to copy segment '%3' from segmentation '%4' to '%5'.\n\nWould you like to change the master representation of '%5' to '%1'?\n\nNote: This may result in unwanted data loss in %5.")
      .arg(fromSegmentation->GetMasterRepresentationName()).arg(toSegmentation->GetMasterRepresentationName()).arg(segmentId).arg(fromNode->GetName()).arg(toNode->GetName());
    QMessageBox::StandardButton answer =
      QMessageBox::question(NULL, tr("Failed to copy segment"), message,
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
      // Convert target segmentation to master representation of source segmentation
      bool successfulConversion = toSegmentation->CreateRepresentation(fromSegmentation->GetMasterRepresentationName());
      if (!successfulConversion)
      {
        QString message = QString("Failed to convert %1 to %2!").arg(toNode->GetName()).arg(fromSegmentation->GetMasterRepresentationName());
        QMessageBox::warning(NULL, tr("Conversion failed"), message);
        return false;
      }

      // Change master representation of target to that of source
      toSegmentation->SetMasterRepresentationName(fromSegmentation->GetMasterRepresentationName());

      // Retry copy of segment
      return this->copySegmentBetweenSegmentations(fromSegmentation, toSegmentation, segmentId, removeFromSource);
    }

    return false;
  }

  // Perform the actual copy/move operation
  return toSegmentation->CopySegmentFromSegmentation(fromSegmentation, segmentIdStd, removeFromSource);
}

//-----------------------------------------------------------------------------
bool qSlicerSegmentationsModuleWidget::copyFromCurrentSegmentation(bool removeFromSource/*=false*/)
{
  Q_D(qSlicerSegmentationsModuleWidget);
 
  return true;
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onMoveFromCurrentSegmentation()
{
  this->copyFromCurrentSegmentation(true);
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onCopyFromCurrentSegmentation()
{
  this->copyFromCurrentSegmentation();
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onCopyToCurrentSegmentation()
{
  Q_D(qSlicerSegmentationsModuleWidget);
 
}

//-----------------------------------------------------------------------------
void qSlicerSegmentationsModuleWidget::onMoveToCurrentSegmentation()
{
  Q_D(qSlicerSegmentationsModuleWidget);
 
}

//-----------------------------------------------------------------------------
bool qSlicerSegmentationsModuleWidget::updateMasterRepresentationInSegmentation(vtkSegmentation* segmentation, QString representation)
{
  if (!segmentation || representation.isEmpty())
  {
    return false;
  }
  std::string newMasterRepresentation(representation.toLatin1().constData());

  // Set master representation to the added one if segmentation is empty or master representation is undefined
  if (segmentation->GetNumberOfSegments() == 0 || segmentation->GetMasterRepresentationName() == NULL)
  {
    segmentation->SetMasterRepresentationName(newMasterRepresentation.c_str());
    return true;
  }

  // No action is necessary if segmentation is non-empty and the master representation matches the contained one in segment
  if (!strcmp(segmentation->GetMasterRepresentationName(), newMasterRepresentation.c_str()))
  {
    return true;
  }

  // Get segmentation node for segmentation
  vtkMRMLScene* scene = this->mrmlScene();
  if (!scene)
  {
    return false;
  }
  vtkMRMLSegmentationNode* segmentationNode = vtkSlicerSegmentationsModuleLogic::GetSegmentationNodeForSegmentation(scene, segmentation);
  if (!segmentationNode)
  {
    return false;
  }

  // Ask the user if master was different but not empty
  QString message = QString("Segment is to be added in segmentation '%1' that contains a representation (%2) different than the master representation in the segmentation (%3). "
    "The master representation need to be changed so that the segment can be added. This might result in unwanted data loss.\n\n"
    "Do you wish to change the master representation to %2?").arg(segmentationNode->GetName()).arg(newMasterRepresentation.c_str()).arg(segmentation->GetMasterRepresentationName());
  QMessageBox::StandardButton answer =
    QMessageBox::question(NULL, tr("Master representation is needed to be changed to add segment"), message,
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (answer == QMessageBox::No)
  {
    return false;
  }

  // Make sure the new master representation exists before setting it
  if (!segmentation->CreateRepresentation(newMasterRepresentation.c_str()))
  {
    std::vector<std::string> containedRepresentationNamesInSegmentation;
    segmentation->GetContainedRepresentationNames(containedRepresentationNamesInSegmentation);
    if (containedRepresentationNamesInSegmentation.empty())
    {
      qCritical() << "qSlicerSegmentationsModuleWidget::updateMasterRepresentationInSegmentation: Master representation cannot be created in segmentation as it does not contain any representations!";
      return false;
    }

    std::string firstContainedRepresentation = (*containedRepresentationNamesInSegmentation.begin());
    qCritical() << "qSlicerSegmentationsModuleWidget::updateMasterRepresentationInSegmentation: Master representation cannot be created in segmentation! Setting master to the first representation found: " << firstContainedRepresentation.c_str();
    segmentation->SetMasterRepresentationName(newMasterRepresentation.c_str());
    return false;
  }

  // Set master representation to the added one if user agreed
  segmentation->SetMasterRepresentationName(newMasterRepresentation.c_str());
  return true;
}




void qSlicerSegmentationsModuleWidget::onSetDisplaySettings()
{
	Q_D(qSlicerSegmentationsModuleWidget);

	qSRPlanSegmentationDisplaySettingsDialog * settingsdialog = new qSRPlanSegmentationDisplaySettingsDialog(d->pushButton_Settings);

	vtkMRMLSegmentationDisplayNode* displayNode = this->segmentationDisplayNode();
	if (!displayNode)
	{
		return;
	}
 
	settingsdialog->updateFromDisplayNode(displayNode);

	settingsdialog->show();


	if (settingsdialog->exec())
	{
		settingsdialog->updateToDisplayNode(displayNode);
	}


}

