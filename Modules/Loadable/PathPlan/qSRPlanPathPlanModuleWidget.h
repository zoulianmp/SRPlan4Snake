/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSRPlanPathPlanModuleWidget_h
#define __qSRPlanPathPlanModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSRPlanPathPlanModuleExport.h"



#include <vtkScalarBarWidget.h>
#include "vtkSlicerRTScalarBarActor.h"


class QTimer;
class vtkRenderer;


class QMenu;
class QModelIndex;
class QTableWidgetItem;
class QShortcut;
class qSRPlanPathPlanModuleWidgetPrivate;
class vtkMRMLMarkupsNode;
class vtkMRMLNode;
class vtkSlicerMarkupsLogic;
class vtkSlicerIsodoseLogic;
class vtkSRPlanBDoseCalculateLogic;
class vtkSlicerDoseVolumeHistogramLogic;

class vtkMRMLGeneralParametersNode;
class vtkMRMLScalarVolumeNode;
class vtkMRMLSegmentationNode;
/// \ingroup Slicer_QtModules_Markups
class Q_SRPlan_QTMODULES_PATHPLAN_EXPORT qSRPlanPathPlanModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSRPlanPathPlanModuleWidget(QWidget *parent=0);
  virtual ~qSRPlanPathPlanModuleWidget();

  /// Set up the GUI from mrml when entering
  /// \sa updateMaximumScaleFromVolumes()
  virtual void enter();
  /// Disconnect from scene when exiting
  virtual void exit();

  /// Manage short cuts that allow key bindings for certain functions
  void installShortcuts();
  void removeShortcuts();

  /// Get the logic in the proper class
  vtkSlicerMarkupsLogic *markupsLogic();

  /// Refresh the gui from the currently active markup node as determined by
  /// the selection node
  /// \sa updateWidgetFromDisplayNode()
  void updateWidgetFromMRML();
  /// Update the GUI elements related to the display properties from MRML, by
  /// getting the display node associated with the active markups node.
  /// \sa  updateWidgetFromMRML()
  void updateWidgetFromDisplayNode();
  /// Check Red volume slice spacing to make sure that the glyph and text scale
  /// slider maximums allow the glyphs to be scaled so that they can be shown
  /// on volumes with spacing larger than the default. Find the maximum background
  /// volume spacing and multiply it by the volume spacing scale facotr
  /// \sa volumeSpacingScaleFactor
  void updateMaximumScaleFromVolumes();
  /// Refresh a row of the gui from the mth markup in the currently active
  /// markup node as defined by the selection node combo box
  void updateRow(int m);

  /// Add observations to the markups node, and remove them from other markups
  /// nodes (from all nodes if markupsNode is null)
  void observeMarkupsNode(vtkMRMLNode *markupsNode);

  /// Reset the GUI elements: clear out the table
  void clearGUI();

  /// Set up the logic default display settings from the appplication settings
  void updateLogicFromSettings();

  /// Query the logic as to the state of the slice intersection visibility on
  /// the slice composite nodes
  bool sliceIntersectionsVisible();

  //shot cut for logic
 
  vtkSRPlanBDoseCalculateLogic* getBDoseCalculateLogic();

  vtkSlicerMarkupsLogic*  getMarkupsLogic();
  vtkSlicerIsodoseLogic * getIsodoseLogic();

  vtkSlicerDoseVolumeHistogramLogic * getDVHLogic();

  //added by zoulian
  void PlaceSnakeHead(double centerX,double centerY,double centerZ,double orientX, double orientY, double orientZ);

  //Give the n index ,judge the TMark Label added by zoulian
  bool IsTMarkofNthNodeinActiveMarkupList(int n);

  enum PathPlanWorkMode
  {
	  PathPlanning = 0,
	  DoseCalEvaluation = 1,
	  RealTracking = 2
  };



public slots:

  /// Respond to the scene events
  /// when a markups node is added, make it the active one in the combo box
  void onNodeAddedEvent(vtkObject* scene, vtkObject* node);
  /// When a node is removed and it is the last one in the scene, clear out
  /// the gui - the node combo box will signal that a remaining node has been
  /// selected and the GUI will update separately in that case
  void onNodeRemovedEvent(vtkObject* scene, vtkObject* node);
  /// Update the table after a scene is imported
  void onMRMLSceneEndImportEvent();
  /// Update the table after a scene view is restored
  void onMRMLSceneEndRestoreEvent();
  /// Update the table after batch processing is done, needed to trigger
  /// an update after a markups file is read in
  void onMRMLSceneEndBatchProcessEvent();
  /// Clear out the gui when the scene is closed
  void onMRMLSceneEndCloseEvent();
  /// Respond to the p key being pressed
  void onPKeyActivated();

  /// Called from enter and on import/add mrml events to ask if the user
  /// wishes to convert annotation fiducial heirarchies into markups list nodes.
  /// \sa enter(), onMRMLSceneEndImportEvent(), onMRMLSceneEndBatchProcessEvent()
  void checkForAnnotationFiducialConversion();
  /// Uses the Logic to do the conversion from annotation fiducials, moving
  /// them from hierarchies to Markups list nodes
  void convertAnnotationFiducialsToMarkups();

  
  /// List button slots
  void onVisibilityOnAllMarkupsInListPushButtonClicked();
  void onVisibilityOffAllMarkupsInListPushButtonClicked();
  void onVisibilityAllMarkupsInListToggled();
  void onLockAllMarkupsInListPushButtonClicked();
  void onUnlockAllMarkupsInListPushButtonClicked();
  void onLockAllMarkupsInListToggled();
  void onSelectAllMarkupsInListPushButtonClicked();
  void onDeselectAllMarkupsInListPushButtonClicked();
  void onSelectedAllMarkupsInListToggled();
  void onAddMarkupPushButtonClicked();

  //Realtime tracing and DOSE Calculation
  void onRealTracePushButtonClicked();
  void onDoseCalculatePushButtonClicked();

  void onDeleteMarkupPushButtonClicked();
  void onDeleteAllMarkupsInListPushButtonClicked();

  /// Update the selection node from the combo box
  void onActiveMarkupMRMLNodeChanged(vtkMRMLNode *markupsNode);
  /// Update the combo box from the selection node
  void onSelectionNodeActivePlaceNodeIDChanged();

  /// When the user clicks in the comob box to create a new markups node,
  /// make sure that a display node is added
  void onActiveMarkupMRMLNodeAdded(vtkMRMLNode *markupsNode);

  /// Toggle the markups node visibility flag
  void onListVisibileInvisiblePushButtonClicked();
  /// Update the icon and tool tip on the list visibility button
  void updateListVisibileInvisiblePushButton(int visibleFlag);

  /// Toggle the markups node locked flag
  void onListLockedUnlockedPushButtonClicked();

  

  /// Update the mrml node from the table
  void onActiveMarkupTableCellChanged(int row, int column);
  /// React to clicks in the table
  void onActiveMarkupTableCellClicked(QTableWidgetItem* item);
  /// React to arrows setting the current cell
  void onActiveMarkupTableCurrentCellChanged(int currentRow, int currentColumn,
                                             int previousRow, int previousColumn);
  /// Provide a right click menu in the table
  void onRightClickActiveMarkupTableWidget(QPoint pos);
  /// Add the coordinates of the currently selected markups as strings to the given menu, then add a separator
  void addSelectedCoordinatesToMenu(QMenu *menu);
  /// Jump slices action slot
  void onJumpSlicesActionTriggered();
  /// Refocus cameras action slot
  void onRefocusCamerasActionTriggered();

  /// Build a string list of the names of other nodes with the same
  /// class name as thisMarkup. Return an empty string list if no other
  /// markups in the scene
  QStringList getOtherMarkupNames(vtkMRMLNode *thisMarkup);
  /// Right click action slot for copy
  void onCopyToOtherListActionTriggered();
  /// Right click action slot for move
  void onMoveToOtherListActionTriggered();
  /// Respond to the user selecting another list to copy the selected markup
  void copySelectedToNamedList(QString listName);
  /// Respond to the user selecting another list to move selected markups to
  /// the named list, calls logic method to do the move if it can find both
  /// markups nodes in the scene
  void moveSelectedToNamedList(QString listName);

  /// Enable/disable editing the table if the markups node is un/locked
  void onActiveMarkupsNodeLockModifiedEvent();
  /// Update the format text entry from the node
  void onActiveMarkupsNodeLabelFormatModifiedEvent();
  /// Update the table with the modified point information if the node is
  /// active
  void onActiveMarkupsNodePointModifiedEvent(vtkObject *caller, vtkObject *callData);
  /// Update the table with the new markup if the node is active
  void onActiveMarkupsNodeMarkupAddedEvent();//vtkMRMLNode *markupsNode);
  /// Update the table for the removed markup if the node is active
  void onActiveMarkupsNodeMarkupRemovedEvent();//vtkMRMLNode *markupsNode);
  /// Update a table row from a modified markup
  void onActiveMarkupsNodeNthMarkupModifiedEvent(vtkObject *caller, vtkObject *callData);
  /// Update the display properties widgets when the display node is modified
  void onActiveMarkupsNodeDisplayModifiedEvent();
  /// Update the transform related elemetns of the gui when the transform node is modified
  void onActiveMarkupsNodeTransformModifiedEvent();

  /// Create a new markups node and copy the display node settings from the
  /// current markups node  if set, otherwise just uses the defaults.
  void onNewMarkupWithCurrentDisplayPropertiesTriggered();

  // given the Flash Dose Distribution Thresholder
 // void updateDoseGridFlashShowLowerThresholder( int low);

  //when timer timeout,update the TraceMark Position.
  void UpdateTraceMarkPosition();
  void SaveSnakeHeadDirectionToParametersNode(double * directionxyz);

  void updateTMarkOpacitytoParametersNode(double);

  //****************************************************************************
  //Iso Dose  Evaluation slots

  /// Set number of levels
  void setNumberOfLevels(int newNumber);

  /// Slot for changing isoline visibility
  void setIsolineVisibility(bool);

  /// Slot for changing isosurface visibility
  void setIsosurfaceVisibility(bool);

  /// Slot for changing 3D scalar bar visibility
  void setScalarBarVisibility(bool);

  /// Slot for changing 2D scalar bar visibility
  void setScalarBar2DVisibility(bool);

  /// Slot handling clicking the Apply button
  void applyClicked();

 // Dose Volume Slots by zoulian
  void switchToToTableFourUpQuantitativeLayout();
  void switchToFourUpQuantitativeLayout();
  void switchToOneUpQuantitativeLayout();

  void updateDVHWidgetFromMRML();


  void onProgressUpdated(vtkObject*, void*, unsigned long, void*);

  // Needed to update other content when dose is invalided
  void onDoseInvalid();

  void onWorkModeChanged( int i);

  void showInChartCheckStateChanged(int aState);

signals:
  void DoseInvalided();
  void WorkModeChanged(int i);




protected:
  QScopedPointer<qSRPlanPathPlanModuleWidgetPrivate> d_ptr;

  virtual void setup();

  /// A multiplication factor to apply to the maximum volume slice spacing when determining what the maximum value for the scale sliders should be.
  /// \sa updateMaximumScaleFromVolumes
  /// Default: 10.0
  double volumeSpacingScaleFactor;
  //**********************************************************************************
  // Begin ISO Dose Evaluation
  //Given a Dose distribution Volume, enter the ISO DoseEvaluate Function
  void enterIsoDoseEvaluationFunction(vtkMRMLScalarVolumeNode* doseGrid);

  //Given the Segmentation and DoseVolume, enter the DVH Evaluation function
  void enterDVHDoseEvalutaionFunction(vtkMRMLScalarVolumeNode* DoseDistribution, vtkMRMLSegmentationNode*segmentationNode);

  //compute the selected Segmentations DVH
  void computeDvh();

  //set dose scalar bars in views 
  void SetupScalarBarsShow();

  //Test the (x,y,z) Whether in the Primary Image Range,
  bool isOutofPrimaryImageRange(double x, double y, double z);

  void updateScalarBarsFromSelectedColorTable();
  void updateIsoDoseGroupWidgetFromMRML();

  //set up the Iso Dose Evalution guis
  void SetupIsoDoseGroupGUIConnections();

  /// Updates button states
  void updateButtonsState();

  /// Updates state of show/hide chart checkboxes according to the currently selected chart
  void updateChartCheckboxesState();

  /// Refresh DVH statistics table
  /// \param force Flag indicating if refresh is to be done in any case
  void refreshDvhTable(bool force = false);

private:
  Q_DECLARE_PRIVATE(qSRPlanPathPlanModuleWidget);
  Q_DISABLE_COPY(qSRPlanPathPlanModuleWidget);

  QShortcut *pToAddShortcut;

  QTimer *tracingTimer;
  vtkMRMLGeneralParametersNode* m_parametersNode=NULL;
  //vtkRenderer * m_SnakeHeadRenderer; //Used for SnakeHead Cone Show


  //Dose Scalars in views

  vtkScalarBarWidget* ScalarBarWidget;
  vtkScalarBarWidget* ScalarBarWidget2DRed;
  vtkScalarBarWidget* ScalarBarWidget2DYellow;
  vtkScalarBarWidget* ScalarBarWidget2DGreen;

  vtkSlicerRTScalarBarActor* ScalarBarActor;
  vtkSlicerRTScalarBarActor* ScalarBarActor2DRed;
  vtkSlicerRTScalarBarActor* ScalarBarActor2DYellow;
  vtkSlicerRTScalarBarActor* ScalarBarActor2DGreen;

  //A bool index for DVH Calculation
  bool validDVH =false;
  bool showDoseEvalulationWhenEnter;
  bool ValidDose = false;


  vtkMRMLScalarVolumeNode* ActiveDoseDistribution;
  
  vtkMRMLSegmentationNode* ActivesegmentationNode;



  PathPlanWorkMode currentWorkMode;
};

#endif
