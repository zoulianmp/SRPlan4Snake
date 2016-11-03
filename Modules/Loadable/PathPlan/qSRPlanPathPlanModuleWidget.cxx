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

// Qt includes
#include <QDebug>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QSettings>
#include <QShortcut>
#include <QSignalMapper>
#include <QStringList>
#include <QTableWidgetItem>

#include <QProgressDialog>

#include "QTimer.h"

// CTK includes
#include "ctkMessageBox.h"

// SlicerQt includes
#include "qMRMLSceneModel.h"
#include "qMRMLUtils.h"
#include "qSlicerApplication.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLSliceNode.h"

#include "vtkMRMLApplicationLogic.h"
#include "vtkMRMLColorTableNode.h"
#include "vtkMRMLIsodoseNode.h"
#include "vtkSlicerIsodoseLogic.h"

#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLSceneUtility.h"
#include "vtkMRMLGeneralParametersNode.h"
#include "vtkMRMLModelHierarchyNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkMRMLSegmentationNode.h"
#include "vtkMRMLDoseVolumeHistogramNode.h"
#include "vtkMRMLChartNode.h"

#include "qMRMLSimpleTableWidget.h"
 

// Markups includes
#include "qSRPlanPathPlanModuleWidget.h"
#include "qSRPlanPathPlanModule.h"
#include "ui_qSRPlanPathPlanModule.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"
#include "vtkSRPlanPathPlanModuleLogic.h"
#include "vtkSlicerDoseVolumeHistogramLogic.h"

// VTK includes
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkCollection.h>

#include <vtkLookupTable.h>

#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

//3D RenderWindow
#include "qSlicerLayoutManager.h"
#include "qSlicerApplication.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"

#include "qMRMLSliceView.h"
#include "qMRMLSliceWidget.h"

#include "SlicerRtCommon.h"

#include "vtkRenderWindow.h"

#include "vtkMRMLGeneralParametersNode.h"

#include <vtksys/SystemTools.hxx>
#include <math.h>




//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Markups
class qSRPlanPathPlanModuleWidgetPrivate: public Ui_qSRPlanPathPlanModule
{
	
  Q_DECLARE_PUBLIC(qSRPlanPathPlanModuleWidget);
protected:
  qSRPlanPathPlanModuleWidget* const q_ptr;

public:
	qSRPlanPathPlanModuleWidgetPrivate(qSRPlanPathPlanModuleWidget& object);
  ~qSRPlanPathPlanModuleWidgetPrivate();

  void setupUi(qSlicerWidget* widget);

  /// the number of columns matches the column labels by using the size of the QStringList
  int numberOfColumns();
  /// return the column index for a given QString, -1 if not a valid header
  int columnIndex(QString label);

  /// Map that associates the vtkMRMLDoubleArrayNode id to the show/hide in chart checkboxes
  QMap<QCheckBox*, QString> PlotCheckboxToStructureNameMap;

  /// Progress dialog for tracking DVH calculation progress
  QProgressDialog* ConvertProgressDialog;

private:
  QStringList columnLabels;

  QAction*    newMarkupWithCurrentDisplayPropertiesAction;

  QMenu*      visibilityMenu;
  QAction*    visibilityOnAllMarkupsInListAction;
  QAction*    visibilityOffAllMarkupsInListAction;

  QMenu*      selectedMenu;
  QAction*    selectedOnAllMarkupsInListAction;
  QAction*    selectedOffAllMarkupsInListAction;

  QMenu*      lockMenu;
  QAction*    lockAllMarkupsInListAction;
  QAction*    unlockAllMarkupsInListAction;
};

//-----------------------------------------------------------------------------
// qSRPlanPathPlanModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSRPlanPathPlanModuleWidgetPrivate::qSRPlanPathPlanModuleWidgetPrivate(qSRPlanPathPlanModuleWidget& object)
  : q_ptr(&object)
{
  this->columnLabels << "Selected" << "Locked" << "Visible" << "Name" << "Weight" << "R" << "A" << "S";

  this->newMarkupWithCurrentDisplayPropertiesAction = 0;
  this->visibilityMenu = 0;
  this->visibilityOnAllMarkupsInListAction = 0;
  this->visibilityOffAllMarkupsInListAction = 0;

  this->selectedMenu = 0;
  this->selectedOnAllMarkupsInListAction = 0;
  this->selectedOffAllMarkupsInListAction = 0;

  this->lockMenu = 0;
  this->lockAllMarkupsInListAction = 0;
  this->unlockAllMarkupsInListAction = 0;

  
}

//-----------------------------------------------------------------------------
int qSRPlanPathPlanModuleWidgetPrivate::columnIndex(QString label)
{
  return this->columnLabels.indexOf(label);
}

//-----------------------------------------------------------------------------
qSRPlanPathPlanModuleWidgetPrivate::~qSRPlanPathPlanModuleWidgetPrivate()
{

}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidgetPrivate::setupUi(qSlicerWidget* widget)
{
  Q_Q(qSRPlanPathPlanModuleWidget);
  this->Ui_qSRPlanPathPlanModule::setupUi(widget);

  //std::cout << "setupUI\n";

  
  // set up the list buttons
  // visibility
  // first add actions to the menu, then hook them up
  visibilityMenu = new QMenu(q->tr("Visibility"), this->visibilityAllMarkupsInListMenuButton);
  // visibility on
  this->visibilityOnAllMarkupsInListAction =
    new QAction(QIcon(":/Icons/Small/SlicerVisible.png"), "Visibility On", visibilityMenu);
  this->visibilityOnAllMarkupsInListAction->setToolTip("Set visibility flag to on for all markups in active list");
  this->visibilityOnAllMarkupsInListAction->setCheckable(false);
  QObject::connect(this->visibilityOnAllMarkupsInListAction, SIGNAL(triggered()),
                   q, SLOT(onVisibilityOnAllMarkupsInListPushButtonClicked()));

  // visibility off
  this->visibilityOffAllMarkupsInListAction =
    new QAction(QIcon(":/Icons/Small/SlicerInvisible.png"), "Visibility Off", visibilityMenu);
  this->visibilityOffAllMarkupsInListAction->setToolTip("Set visibility flag to off for all markups in active list");
  this->visibilityOffAllMarkupsInListAction->setCheckable(false);
  QObject::connect(this->visibilityOffAllMarkupsInListAction, SIGNAL(triggered()),
                   q, SLOT(onVisibilityOffAllMarkupsInListPushButtonClicked()));

  this->visibilityMenu->addAction(this->visibilityOnAllMarkupsInListAction);
  this->visibilityMenu->addAction(this->visibilityOffAllMarkupsInListAction);
  this->visibilityAllMarkupsInListMenuButton->setMenu(this->visibilityMenu);
  this->visibilityAllMarkupsInListMenuButton->setIcon(QIcon(":/Icons/VisibleOrInvisible.png"));

  // visibility toggle
  QObject::connect(this->visibilityAllMarkupsInListMenuButton, SIGNAL(clicked()),
                   q, SLOT(onVisibilityAllMarkupsInListToggled()));

  // lock
  // first add actions to the menu, then hook them up
  lockMenu = new QMenu(q->tr("Lock"), this->lockAllMarkupsInListMenuButton);
  // lock
  this->lockAllMarkupsInListAction =
    new QAction(QIcon(":/Icons/Small/SlicerLock.png"), "Lock", lockMenu);
  this->lockAllMarkupsInListAction->setToolTip("Set lock flag to on for all markups in active list");
  this->lockAllMarkupsInListAction->setCheckable(false);
  QObject::connect(this->lockAllMarkupsInListAction, SIGNAL(triggered()),
                   q, SLOT(onLockAllMarkupsInListPushButtonClicked()));

  // lock off
  this->unlockAllMarkupsInListAction =
    new QAction(QIcon(":/Icons/Small/SlicerUnlock.png"), "Unlock", lockMenu);
  this->unlockAllMarkupsInListAction->setToolTip("Set lock flag to off for all markups in active list");
  this->unlockAllMarkupsInListAction->setCheckable(false);
  QObject::connect(this->unlockAllMarkupsInListAction, SIGNAL(triggered()),
                   q, SLOT(onUnlockAllMarkupsInListPushButtonClicked()));

  this->lockMenu->addAction(this->lockAllMarkupsInListAction);
  this->lockMenu->addAction(this->unlockAllMarkupsInListAction);
  this->lockAllMarkupsInListMenuButton->setMenu(this->lockMenu);
  this->lockAllMarkupsInListMenuButton->setIcon(QIcon(":/Icons/Small/SlicerLockUnlock.png"));

  // lock toggle
  QObject::connect(this->lockAllMarkupsInListMenuButton, SIGNAL(clicked()),
                   q, SLOT(onLockAllMarkupsInListToggled()));

  // selected
  // first add actions to the menu, then hook them up
  selectedMenu = new QMenu(q->tr("Selected"), this->selectedAllMarkupsInListMenuButton);
  // selected on
  this->selectedOnAllMarkupsInListAction =
    new QAction(QIcon(":/Icons/MarkupsSelected.png"), "Selected On", selectedMenu);
  this->selectedOnAllMarkupsInListAction->setToolTip("Set selected flag to on for all markups in active list");
  this->selectedOnAllMarkupsInListAction->setCheckable(false);
  QObject::connect(this->selectedOnAllMarkupsInListAction, SIGNAL(triggered()),
                   q, SLOT(onSelectAllMarkupsInListPushButtonClicked()));

  // selected off
  this->selectedOffAllMarkupsInListAction =
    new QAction(QIcon(":/Icons/MarkupsUnselected.png"), "Selected Off", selectedMenu);
  this->selectedOffAllMarkupsInListAction->setToolTip("Set selected flag to off for all markups in active list");
  this->selectedOffAllMarkupsInListAction->setCheckable(false);
  QObject::connect(this->selectedOffAllMarkupsInListAction, SIGNAL(triggered()),
                   q, SLOT(onDeselectAllMarkupsInListPushButtonClicked()));

  this->selectedMenu->addAction(this->selectedOnAllMarkupsInListAction);
  this->selectedMenu->addAction(this->selectedOffAllMarkupsInListAction);
  this->selectedAllMarkupsInListMenuButton->setMenu(this->selectedMenu);
  this->selectedAllMarkupsInListMenuButton->setIcon(QIcon(":/Icons/MarkupsSelectedOrUnselected.png"));

  // selected toggle
  QObject::connect(this->selectedAllMarkupsInListMenuButton, SIGNAL(clicked()),
                   q, SLOT(onSelectedAllMarkupsInListToggled()));

  // add
  QObject::connect(this->addMarkupPushButton, SIGNAL(clicked()),
                   q, SLOT(onAddMarkupPushButtonClicked()));


  // realtime tracing 

  QObject::connect(this->realTracePushButton, SIGNAL(clicked()),
	  q, SLOT(onRealTracePushButtonClicked()));

  QObject::connect(this->doseCalculatePushButton, SIGNAL(clicked()),
	  q, SLOT(onDoseCalculatePushButtonClicked()));
  
  //Flash Dose Distribution show

 // QObject::connect(this->spinBox_RelativeDoseValue, SIGNAL(valueChanged(int)),
 //	  q, SLOT(updateDoseGridFlashShowLowerThresholder(int )));


  //TMark Opacity for Snake header

  QObject::connect(this->doubleSpinBox_TMOpacity, SIGNAL(valueChanged(double)),
	  q, SLOT(updateTMarkOpacitytoParametersNode(double)));



   
  // delete
  QObject::connect(this->deleteMarkupPushButton, SIGNAL(clicked()),
                   q, SLOT(onDeleteMarkupPushButtonClicked()));
  QObject::connect(this->deleteAllMarkupsInListPushButton, SIGNAL(clicked()),
                   q, SLOT(onDeleteAllMarkupsInListPushButtonClicked()));



  // set up the active markups node selector
  QObject::connect(this->activeMarkupMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                   q, SLOT(onActiveMarkupMRMLNodeChanged(vtkMRMLNode*)));
  QObject::connect(this->activeMarkupMRMLNodeComboBox, SIGNAL(nodeAddedByUser(vtkMRMLNode*)),
                   q, SLOT(onActiveMarkupMRMLNodeAdded(vtkMRMLNode*)));



  //
  // add an action to create a new markups list using the display node
  // settings from the currently active list
  //

  this->newMarkupWithCurrentDisplayPropertiesAction =
    new QAction("New markups with current display properties",
                this->activeMarkupMRMLNodeComboBox);
  this->activeMarkupMRMLNodeComboBox->addMenuAction(this->newMarkupWithCurrentDisplayPropertiesAction);
  this->activeMarkupMRMLNodeComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  QObject::connect(this->newMarkupWithCurrentDisplayPropertiesAction, SIGNAL(triggered()),
                    q, SLOT(onNewMarkupWithCurrentDisplayPropertiesTriggered()));

  //
  // set up the list visibility/locked buttons
  //
  QObject::connect(this->listVisibileInvisiblePushButton, SIGNAL(clicked()),
                   q, SLOT(onListVisibileInvisiblePushButtonClicked()));
  QObject::connect(this->listLockedUnlockedPushButton, SIGNAL(clicked()),
                   q, SLOT(onListLockedUnlockedPushButtonClicked()));
  

  //
  // set up the table
  //

  // only select rows rather than cells
  this->activeMarkupTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  // allow multi select
  this->activeMarkupTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // number of columns with headers
  this->activeMarkupTableWidget->setColumnCount(this->numberOfColumns());
  this->activeMarkupTableWidget->setHorizontalHeaderLabels(this->columnLabels);
  this->activeMarkupTableWidget->horizontalHeader()->setFixedHeight(32);

  // adjust the column widths
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("Name"), 120);
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("Weight"), 60);
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("R"), 65);
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("A"), 65);
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("S"), 65);

 

  // use an icon for some column headers
  // selected is a check box
  QTableWidgetItem *selectedHeader = this->activeMarkupTableWidget->horizontalHeaderItem(this->columnIndex("Selected"));
  selectedHeader->setText("");
  selectedHeader->setIcon(QIcon(":/Icons/MarkupsSelected.png"));
  selectedHeader->setToolTip(QString("Click in this column to select/deselect markups for passing to CLI modules"));
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("Selected"), 30);
  // locked is an open and closed lock
  QTableWidgetItem *lockedHeader = this->activeMarkupTableWidget->horizontalHeaderItem(this->columnIndex("Locked"));
  lockedHeader->setText("");
  lockedHeader->setIcon(QIcon(":/Icons/Small/SlicerLockUnlock.png"));
  lockedHeader->setToolTip(QString("Click in this column to lock/unlock markups to prevent them from being moved by mistake"));
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("Locked"), 30);
  // visible is an open and closed eye
  QTableWidgetItem *visibleHeader = this->activeMarkupTableWidget->horizontalHeaderItem(this->columnIndex("Visible"));
  visibleHeader->setText("");
  visibleHeader->setIcon(QIcon(":/Icons/Small/SlicerVisibleInvisible.png"));
  visibleHeader->setToolTip(QString("Click in this column to show/hide markups in 2D and 3D"));
  this->activeMarkupTableWidget->setColumnWidth(this->columnIndex("Visible"), 30);

  // listen for changes so can update mrml node
  QObject::connect(this->activeMarkupTableWidget, SIGNAL(cellChanged(int, int)),
                   q, SLOT(onActiveMarkupTableCellChanged(int, int)));

  // listen for click on a markup
  QObject::connect(this->activeMarkupTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)),
                   q, SLOT(onActiveMarkupTableCellClicked(QTableWidgetItem*)));
  // listen for the current cell changing (happens when arrows are used to navigate)
  QObject::connect(this->activeMarkupTableWidget, SIGNAL(currentCellChanged(int, int, int, int)),
                   q, SLOT(onActiveMarkupTableCurrentCellChanged(int, int, int, int)));
  // listen for a right click
  this->activeMarkupTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this->activeMarkupTableWidget, SIGNAL(customContextMenuRequested(QPoint)),
                   q, SLOT(onRightClickActiveMarkupTableWidget(QPoint)));


  //Set up the enter DVH Function Button signal and slots added by zoulian
  QObject::connect(this->pushButton_SwitchToFourUpQuantitativeLayout, SIGNAL(clicked()), q, SLOT(switchToFourUpQuantitativeLayout()));
  QObject::connect(this->pushButton_SwitchToOneUpQuantitativeLayout, SIGNAL(clicked()), q, SLOT(switchToOneUpQuantitativeLayout()));
  QObject::connect(this->pushButton_SwitchToTableFourUpQuantitativeLayout, SIGNAL(clicked()), q, SLOT(switchToToTableFourUpQuantitativeLayout()));


  //Connect the Dose Invalidation
  QObject::connect(q, SIGNAL(DoseInvalided()), q, SLOT(onDoseInvalid()));

}

//-----------------------------------------------------------------------------
int qSRPlanPathPlanModuleWidgetPrivate::numberOfColumns()
{
  return this->columnLabels.size();
}

//-----------------------------------------------------------------------------
// qSRPlanPathPlanModuleWidget methods

//-----------------------------------------------------------------------------
qSRPlanPathPlanModuleWidget::qSRPlanPathPlanModuleWidget(QWidget* _parent)
  : Superclass( _parent )
    , d_ptr( new qSRPlanPathPlanModuleWidgetPrivate(*this) )
{

	Q_D(qSRPlanPathPlanModuleWidget);
  this->pToAddShortcut = 0;

  this->tracingTimer = 0;



  this->volumeSpacingScaleFactor = 10.0;



  ScalarBarWidget = NULL;
  ScalarBarWidget2DRed = NULL;
  ScalarBarWidget2DYellow = NULL;
  ScalarBarWidget2DGreen = NULL;

  ScalarBarActor = NULL;
  ScalarBarActor2DRed = NULL;
  ScalarBarActor2DYellow =NULL;
  ScalarBarActor2DGreen = NULL;

  ActiveDoseDistribution = NULL;
  ActivesegmentationNode = NULL;

  showDoseEvalulationWhenEnter = false;
}


//-----------------------------------------------------------------------------
qSRPlanPathPlanModuleWidget::~qSRPlanPathPlanModuleWidget()
{
	if (this->ScalarBarWidget)
	{
		this->ScalarBarWidget->Delete();
		this->ScalarBarWidget = 0;
	}
	if (this->ScalarBarActor)
	{
		this->ScalarBarActor->Delete();
		this->ScalarBarActor = 0;
	}
	if (this->ScalarBarWidget2DRed)
	{
		this->ScalarBarWidget2DRed->Delete();
		this->ScalarBarWidget2DRed = 0;
	}
	if (this->ScalarBarActor2DRed)
	{
		this->ScalarBarActor2DRed->Delete();
		this->ScalarBarActor2DRed = 0;
	}
	if (this->ScalarBarWidget2DYellow)
	{
		this->ScalarBarWidget2DYellow->Delete();
		this->ScalarBarWidget2DYellow = 0;
	}
	if (this->ScalarBarActor2DYellow)
	{
		this->ScalarBarActor2DYellow->Delete();
		this->ScalarBarActor2DYellow = 0;
	}
	if (this->ScalarBarWidget2DGreen)
	{
		this->ScalarBarWidget2DGreen->Delete();
		this->ScalarBarWidget2DGreen = 0;
	}
	if (this->ScalarBarActor2DGreen)
	{
		this->ScalarBarActor2DGreen->Delete();
		this->ScalarBarActor2DGreen = 0;
	}
	
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::setup()
{
  Q_D(qSRPlanPathPlanModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  // for now disable the combo box: when the combo box has it's mrml scene
  // set, it sets the first markups node as the current node, which can end
  // up over riding the selection node's active place node id
  // d->activeMarkupMRMLNodeComboBox->setEnabled(false);
  d->activeMarkupMRMLNodeComboBox->blockSignals(true);

  // this->updateLogicFromSettings();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::enter()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  this->Superclass::enter();

 
  if (!this->showDoseEvalulationWhenEnter)
  {
	  d->isoDoseGroup->hide();
  }


  d->traceMarkGroup->hide();
  

  // qDebug() << "enter widget";

  // set up mrml scene observations so that the GUI gets updated
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeAddedEvent,
                    this, SLOT(onNodeAddedEvent(vtkObject*, vtkObject*)));
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeRemovedEvent,
                    this, SLOT(onNodeRemovedEvent(vtkObject*, vtkObject*)));
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::EndImportEvent,
                    this, SLOT(onMRMLSceneEndImportEvent()));
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::EndBatchProcessEvent,
                    this, SLOT(onMRMLSceneEndBatchProcessEvent()));
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::EndCloseEvent,
                    this, SLOT(onMRMLSceneEndCloseEvent()));
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::EndRestoreEvent,
                    this, SLOT(onMRMLSceneEndRestoreEvent()));

  if (this->mrmlScene() && this->markupsLogic())
    {
    vtkMRMLNode *selectionNode = this->mrmlScene()->GetNodeByID(this->markupsLogic()->GetSelectionNodeID());
    if (selectionNode)
      {
      this->qvtkConnect(selectionNode, vtkMRMLSelectionNode::ActivePlaceNodeIDChangedEvent,
                        this, SLOT(onSelectionNodeActivePlaceNodeIDChanged()));
      }
    }

  // now enable the combo box and update
  //d->activeMarkupMRMLNodeComboBox->setEnabled(true);
  d->activeMarkupMRMLNodeComboBox->blockSignals(false);

  // install some shortcuts for use while in this module
  this->installShortcuts();

  this->checkForAnnotationFiducialConversion();

  // check the max scales against volume spacing, they might need to be updated
  this->updateMaximumScaleFromVolumes();

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::checkForAnnotationFiducialConversion()
{
  // check to see if there are any annotation fiducial nodes
  // and offer to import them as markups
  int numFids = this->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLAnnotationFiducialNode");
  int numSceneViews = this->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLSceneViewNode");
  if (numFids > 0)
    {
    ctkMessageBox convertMsgBox;
    convertMsgBox.setWindowTitle("Convert Annotation hierarchies to Markups list nodes?");
    QString labelText = QString("Convert ")
      + QString::number(numFids)
      + QString(" Annotation fiducials to Markups list nodes?")
      + QString(" Moves all Annotation fiducials out of hierarchies (deletes")
      + QString(" the nodes, but leaves the hierarchies in case rulers or")
      + QString(" ROIs are mixed in) and into Markups fiducial list nodes.");
    if (numSceneViews > 0)
      {
      labelText += QString(" Iterates through ")
        + QString::number(numSceneViews)
        + QString(" Scene Views and converts any fiducials saved in those")
        + QString(" scenes into Markups as well.");
      }
    // don't show again check box conflicts with informative text, so use
    // a long text
    convertMsgBox.setText(labelText);
    QPushButton *convertButton =
      convertMsgBox.addButton(tr("Convert"), QMessageBox::AcceptRole);
    convertMsgBox.addButton(QMessageBox::Cancel);
    convertMsgBox.setDefaultButton(convertButton);
    convertMsgBox.setDontShowAgainVisible(true);
    convertMsgBox.setDontShowAgainSettingsKey("Markups/AlwaysConvertAnnotationFiducials");
    convertMsgBox.exec();
    if (convertMsgBox.clickedButton() == convertButton)
      {
      this->convertAnnotationFiducialsToMarkups();
      }
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::installShortcuts()
{
  // add some shortcut keys
  if (this->pToAddShortcut == 0)
    {
    this->pToAddShortcut = new QShortcut(QKeySequence(QString("p")), this);
    }
  QObject::connect(this->pToAddShortcut, SIGNAL(activated()),
                   this, SLOT(onPKeyActivated()));
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::removeShortcuts()
{
  if (this->pToAddShortcut != 0)
    {
    //qDebug() << "removeShortcuts";
    this->pToAddShortcut->disconnect(SIGNAL(activated()));
    // TODO: when parent is set to null, using the mouse to place a fid when outside the Markups module is triggering a crash
//    this->pToAddShortcut->setParent(NULL);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::convertAnnotationFiducialsToMarkups()
{
  if (this->markupsLogic())
    {
    this->markupsLogic()->ConvertAnnotationFiducialsToMarkups();
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::exit()
{
  this->Superclass::exit();

  // qDebug() << "exit widget";

  this->removeShortcuts();

  // remove mrml scene observations, don't need to update the GUI while the
  // module is not showing
  this->qvtkDisconnectAll();
}

//-----------------------------------------------------------------------------
vtkSlicerMarkupsLogic *qSRPlanPathPlanModuleWidget::markupsLogic()
{
  if (this->logic() == NULL)
    {
    return NULL;
    }

  vtkSRPlanPathPlanModuleLogic * pathPlanLogic = vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic());



  return pathPlanLogic->GetMarkupsLogic();
}


//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // std::cout << "updateWidgetFromMRML" << std::endl;

  
  if (!this->mrmlScene())
    {
    this->clearGUI();
    return;
    }

  // get the active markup
  vtkMRMLNode *markupsNodeMRML = NULL;
  std::string listID = (this->markupsLogic() ?
                        this->markupsLogic()->GetActiveListID() :
                        std::string(""));

  // if there's no active list in the logic, check if the node selector has one
  // selected. This can happen after restoring an older scene view that
  // didn't have the active place node set on the selection node
  if (listID.compare("") == 0)
    {
    QString currentNodeID = d->activeMarkupMRMLNodeComboBox->currentNodeID();
    if (currentNodeID.compare("") != 0)
      {
      listID = std::string(currentNodeID.toLatin1());
      qDebug() << "Active list id was null, using GUI combo box setting";
      // and set it active
      vtkMRMLNode *mrmlNode = NULL;
      vtkMRMLSelectionNode *selectionNode = NULL;
      if (this->markupsLogic())
        {
         mrmlNode = this->mrmlScene()->GetNodeByID(this->markupsLogic()->GetSelectionNodeID());
        }
      if (mrmlNode)
        {
        selectionNode = vtkMRMLSelectionNode::SafeDownCast(mrmlNode);
        }
      if (selectionNode)
        {
        selectionNode->SetActivePlaceNodeID(listID.c_str());
        }
      }
    }

  markupsNodeMRML = this->mrmlScene()->GetNodeByID(listID.c_str());
  vtkMRMLMarkupsNode *markupsNode = NULL;
  if (markupsNodeMRML)
    {
    markupsNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNodeMRML);
    }

  // make sure that the GUI updates on changes to the current mark up node,
  // remove observations if no current node
  this->observeMarkupsNode(markupsNode);

  if (!markupsNode)
    {
    // qDebug() << "updateWidgetFromMRML: Unable to get active markups node,
    // clearing out the table";
    this->clearGUI();

    return;
    }

  // update the combo box
  QString activePlaceNodeID = QString(listID.c_str());
  QString currentNodeID = d->activeMarkupMRMLNodeComboBox->currentNodeID();
  if (currentNodeID == "" ||
      (currentNodeID != activePlaceNodeID &&
       activePlaceNodeID.contains("vtkMRMLMarkups")))
    {
    d->activeMarkupMRMLNodeComboBox->setCurrentNodeID(activePlaceNodeID);
    }

  // update display widgets
  this->updateWidgetFromDisplayNode();

  // update the visibility and locked buttons
  this->updateListVisibileInvisiblePushButton(markupsNode->GetDisplayVisibility());

  if (markupsNode->GetLocked())
    {
    d->listLockedUnlockedPushButton->setIcon(QIcon(":Icons/Medium/SlicerLock.png"));
    d->listLockedUnlockedPushButton->setToolTip(QString("Click to unlock this markup list so that the markups can be moved by the mouse"));
    }
  else
    {
    d->listLockedUnlockedPushButton->setIcon(QIcon(":Icons/Medium/SlicerUnlock.png"));
    d->listLockedUnlockedPushButton->setToolTip(QString("Click to lock this markup list so that the markups cannot be moved by the mouse"));
    }
  

  // update the table
  int numberOfMarkups = markupsNode->GetNumberOfMarkups();
  if (d->activeMarkupTableWidget->rowCount() != numberOfMarkups)
    {
    // clear it out
    d->activeMarkupTableWidget->setRowCount(numberOfMarkups);
    }
  // update the table per markup
  for (int m = 0; m < numberOfMarkups; m++)
    {
    this->updateRow(m);
    }

	
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateWidgetFromDisplayNode()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active markup
  vtkMRMLNode *markupsNodeMRML = NULL;
  std::string listID = (this->markupsLogic() ? this->markupsLogic()->GetActiveListID() : std::string(""));
  markupsNodeMRML = this->mrmlScene()->GetNodeByID(listID.c_str());
  vtkMRMLMarkupsNode *markupsNode = NULL;
  if (markupsNodeMRML)
    {
    markupsNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNodeMRML);
    }

  // update the display properties from the markups display node
  vtkMRMLDisplayNode *displayNode = markupsNode->GetDisplayNode();
  double *color;
  QColor qColor;

  
 
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateMaximumScaleFromVolumes()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  double maxSliceSpacing = 1.0;

  vtkMRMLSliceLogic *sliceLogic = NULL;
  vtkMRMLApplicationLogic *mrmlAppLogic = this->logic()->GetMRMLApplicationLogic();
  if (!mrmlAppLogic)
    {
    return;
    }

  vtkMRMLNode *mrmlNode = this->mrmlScene()->GetNodeByID("vtkMRMLSliceNodeRed");
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLSliceNode *redSlice = vtkMRMLSliceNode::SafeDownCast(mrmlNode);
  if (!redSlice)
    {
    return;
    }
  sliceLogic = mrmlAppLogic->GetSliceLogic(redSlice);
  if (!sliceLogic)
    {
    return;
    }

  double *volumeSliceSpacing = sliceLogic->GetBackgroundSliceSpacing();
  if (volumeSliceSpacing != NULL)
    {
    for (int i = 0; i < 3; ++i)
      {
      if (volumeSliceSpacing[i] > maxSliceSpacing)
        {
        maxSliceSpacing = volumeSliceSpacing[i];
        }
      }
    }

  
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateRow(int m)
{
  Q_D(qSRPlanPathPlanModuleWidget);


  
  // this is updating the qt widget from MRML, and should not trigger any updates on the node, so turn off events
  d->activeMarkupTableWidget->blockSignals(true);

  // qDebug() << QString("updateRow: row = ") + QString::number(m) + QString(", number of rows = ") + QString::number(d->activeMarkupTableWidget->rowCount());
  // get active markups node
  QString activeMarkupsNodeID = d->activeMarkupMRMLNodeComboBox->currentNodeID();
  vtkMRMLNode *mrmlNode = this->mrmlScene()->GetNodeByID(activeMarkupsNodeID.toLatin1());
  vtkMRMLMarkupsNode *markupsNode = NULL;
  if (mrmlNode)
    {
    markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (!markupsNode)
    {
    //qDebug() << QString("update Row: unable to get markups node with id ") + activeMarkupsNodeID;
    return;
    }

  // selected
  QTableWidgetItem* selectedItem = new QTableWidgetItem();
  if (markupsNode->GetNthMarkupSelected(m))
    {
    selectedItem->setCheckState(Qt::Checked);
    }
  else
    {
    selectedItem->setCheckState(Qt::Unchecked);
    }
  // disable editing so that a double click won't bring up an entry box
  selectedItem->setFlags(selectedItem->flags() & ~Qt::ItemIsEditable);
  int selectedIndex = d->columnIndex("Selected");
  if (d->activeMarkupTableWidget->item(m,selectedIndex) == NULL ||
      (d->activeMarkupTableWidget->item(m,selectedIndex)->checkState() != selectedItem->checkState()))
    {
    d->activeMarkupTableWidget->setItem(m,selectedIndex,selectedItem);
    }

  // locked
  QTableWidgetItem* lockedItem = new QTableWidgetItem();
  // disable checkable
  lockedItem->setData(Qt::CheckStateRole, QVariant());
  lockedItem->setFlags(lockedItem->flags() & ~Qt::ItemIsUserCheckable);
  // disable editing so that a double click won't bring up an entry box
  lockedItem->setFlags(lockedItem->flags() & ~Qt::ItemIsEditable);
  if (markupsNode->GetNthMarkupLocked(m))
    {
    lockedItem->setData(Qt::UserRole, QVariant(true));
    lockedItem->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerLock.png"));
    }
  else
    {
    lockedItem->setData(Qt::UserRole, QVariant(false));
    lockedItem->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerUnlock.png"));
    }
  int lockedIndex = d->columnIndex("Locked");
  if (d->activeMarkupTableWidget->item(m,lockedIndex) == NULL ||
      d->activeMarkupTableWidget->item(m,lockedIndex)->data(Qt::UserRole) != lockedItem->data(Qt::UserRole))
    {
    d->activeMarkupTableWidget->setItem(m,lockedIndex,lockedItem);
    }

  // visible
  QTableWidgetItem* visibleItem = new QTableWidgetItem();
  // disable checkable
  visibleItem->setData(Qt::CheckStateRole, QVariant());
  visibleItem->setFlags(visibleItem->flags() & ~Qt::ItemIsUserCheckable);
  // disable editing so that a double click won't bring up an entry box
  visibleItem->setFlags(visibleItem->flags() & ~Qt::ItemIsEditable);
  if (markupsNode->GetNthMarkupVisibility(m))
    {
    visibleItem->setData(Qt::UserRole, QVariant(true));
    visibleItem->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerVisible.png"));
    }
  else
    {
    visibleItem->setData(Qt::UserRole, QVariant(false));
    visibleItem->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerInvisible.png"));
    }
    int visibleIndex = d->columnIndex("Visible");
   if (d->activeMarkupTableWidget->item(m,visibleIndex) == NULL ||
       d->activeMarkupTableWidget->item(m,visibleIndex)->data(Qt::UserRole) != visibleItem->data(Qt::UserRole))
     {
     d->activeMarkupTableWidget->setItem(m,visibleIndex,visibleItem);
     }

   // name
   int nameIndex = d->columnIndex("Name");
   QString markupLabel = QString(markupsNode->GetNthMarkupLabel(m).c_str());
   if (d->activeMarkupTableWidget->item(m,nameIndex) == NULL ||
       d->activeMarkupTableWidget->item(m,nameIndex)->text() != markupLabel)
     {
     d->activeMarkupTableWidget->setItem(m,nameIndex,new QTableWidgetItem(markupLabel));
     }

   

   // Weight
   int WeightIndex = d->columnIndex("Weight");
   QString markupWeight = QString::number(markupsNode->GetNthMarkupWeight(m), 'f', 3);
   if (d->activeMarkupTableWidget->item(m, WeightIndex) == NULL ||
	   d->activeMarkupTableWidget->item(m, WeightIndex)->text() != markupLabel)
   {
	   d->activeMarkupTableWidget->setItem(m, WeightIndex, new QTableWidgetItem(markupWeight));
   }

   /*
   // description
   int descriptionIndex = d->columnIndex("Description");
   QString markupDescription = QString(markupsNode->GetNthMarkupDescription(m).c_str());
   if (d->activeMarkupTableWidget->item(m,descriptionIndex) == NULL ||
       d->activeMarkupTableWidget->item(m,descriptionIndex)->text() != markupLabel)
     {
     d->activeMarkupTableWidget->setItem(m,descriptionIndex,new QTableWidgetItem(markupDescription));
     }

	 */


   // point
  double point[3] = {0.0, 0.0, 0.0};
   
  markupsNode->GetMarkupPoint(m, 0, point);
 
  int rColumnIndex = d->columnIndex("R");
  for (int p = 0; p < 3; p++)
    {
    // last argument to number sets the precision
    QString coordinate = QString::number(point[p], 'f', 3);
    if (d->activeMarkupTableWidget->item(m,rColumnIndex + p) == NULL ||
        d->activeMarkupTableWidget->item(m,rColumnIndex + p)->text() != coordinate)
      {
      d->activeMarkupTableWidget->setItem(m,rColumnIndex + p,new QTableWidgetItem(coordinate));
      }
    }

  // unblock so that changes to the table will propagate to MRML
  d->activeMarkupTableWidget->blockSignals(false);
  
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onNodeAddedEvent(vtkObject*, vtkObject* node)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (markupsNode)
    {
    // make it active
    d->activeMarkupMRMLNodeComboBox->setCurrentNodeID(markupsNode->GetID());
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onNodeRemovedEvent(vtkObject* scene, vtkObject* node)
{
  Q_UNUSED(scene);
  Q_UNUSED(node);

  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }

  // only respond if it was the last node that was removed
  int numNodes = this->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLMarkupsNode");
  if (numNodes == 0)
    {
    this->clearGUI();
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onMRMLSceneEndImportEvent()
{
  this->checkForAnnotationFiducialConversion();
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onMRMLSceneEndRestoreEvent()
{
  this->checkForAnnotationFiducialConversion();
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onMRMLSceneEndBatchProcessEvent()
{
  if (!this->mrmlScene())
    {
    return;
    }
  this->checkForAnnotationFiducialConversion();
  // qDebug() << "qSRPlanPathPlanModuleWidget::onMRMLSceneEndBatchProcessEvent";
  std::string selectionNodeID = (this->markupsLogic() ? this->markupsLogic()->GetSelectionNodeID() : std::string(""));
  vtkMRMLNode *node = this->mrmlScene()->GetNodeByID(selectionNodeID.c_str());
  vtkMRMLSelectionNode *selectionNode = NULL;
  if (node)
    {
    selectionNode = vtkMRMLSelectionNode::SafeDownCast(node);
    }

  QString activePlaceNodeID;
  if (selectionNode)
    {
    activePlaceNodeID = selectionNode->GetActivePlaceNodeID();
    }
  if (activePlaceNodeID.isEmpty())
    {
    // this might have been triggered after a file load, set the last markups node active
    int numNodes = this->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLMarkupsNode");
    if (numNodes > 0)
      {
      vtkMRMLNode *lastNode = this->mrmlScene()->GetNthNodeByClass(numNodes - 1, "vtkMRMLMarkupsNode");
      if (lastNode)
        {
        // qDebug() << "onMRMLSceneEndBatchProcessEvent: setting active place node id to " << lastNode->GetID();
        selectionNode->SetActivePlaceNodeID(lastNode->GetID());
        }
      }
    }
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onMRMLSceneEndCloseEvent()
{
  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }
  // qDebug() << "qSRPlanPathPlanModuleWidget::onMRMLSceneEndCloseEvent";
  this->clearGUI();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onPKeyActivated()
{
  QPoint pos = QCursor::pos();

  // find out which widget it was over
  QWidget *widget = qSlicerApplication::application()->widgetAt(pos);

  // simulate a mouse press inside the widget
  QPoint widgetPos = widget->mapFromGlobal(pos);
  QMouseEvent click(QEvent::MouseButtonRelease, widgetPos, Qt::LeftButton, 0, 0);
  click.setAccepted(true);

  // and send it to the widget
  //qDebug() << "onPKeyActivated: sending event with pos " << widgetPos;
  QCoreApplication::sendEvent(widget, &click);
}


//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onVisibilityOnAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetAllMarkupsVisibility(listNode, true);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onVisibilityOffAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetAllMarkupsVisibility(listNode, false);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onVisibilityAllMarkupsInListToggled()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->ToggleAllMarkupsVisibility(listNode);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onLockAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);
  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetAllMarkupsLocked(listNode, true);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onUnlockAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetAllMarkupsLocked(listNode, false);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onLockAllMarkupsInListToggled()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->ToggleAllMarkupsLocked(listNode);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onSelectAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetAllMarkupsSelected(listNode, true);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onDeselectAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetAllMarkupsSelected(listNode, false);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onSelectedAllMarkupsInListToggled()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (this->markupsLogic())
    {
    this->markupsLogic()->ToggleAllMarkupsSelected(listNode);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onAddMarkupPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (listNode)
    {
		// for debug added by zoulian
		char * snakePathID = listNode->GetID();  
		// for now, assume a fiducial  
		listNode->AddMarkupWithNPoints(1);
    }

  this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();

  emit DoseInvalided();

  this->updateButtonsState();
   
}


void qSRPlanPathPlanModuleWidget::onDoseCalculatePushButtonClicked()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	// If the realTracing opacity show, then hide
	if (d->traceMarkGroup->isVisible())
	{
		d->traceMarkGroup->hide();
	}


	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	//********************************************************
	//Get the Logic and input Data Sets

	vtkMRMLScene *scene = this->mrmlScene();

	vtkSRPlanPathPlanModuleLogic * PathPlanLogic = vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic());

	vtkSRPlanBDoseCalculateLogic * BDoseLogic = PathPlanLogic->GetBDoseCalculateLogic();

	
	vtkMRMLSelectionNode * selectionNode = PathPlanLogic->GetSelectionNode();
	if (!selectionNode)
	{
		selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
		PathPlanLogic->SetSelectionNode(selectionNode);
	}


	char * planVolumeID = selectionNode->GetPlanPrimaryVolumeID();
	char * snakePathID = selectionNode->GetActivePlaceNodeID();
	char * segmenatationID = selectionNode->GetActiveSegmentationID();

	vtkMRMLScalarVolumeNode* ScalarNode = vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(planVolumeID));

	vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(scene->GetNodeByID(segmenatationID));

	//******************************************************************************************
	//The length in mm,to determine the range of seed source dose distribution

	BDoseLogic->SetDoseCalculateGridSize(1.0);
	BDoseLogic->SetDoseCalculateCutoff(50.0);



	BDoseLogic->SetPlanPrimaryVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(planVolumeID)));
	BDoseLogic->SetSnakePlanPath(vtkMRMLMarkupsNode::SafeDownCast(scene->GetNodeByID(snakePathID)));

	BDoseLogic->StartDoseCalcualte();

	//reset the Dose Staticstic funciton
	this->validDVH = false;


	



	//Get the Final Dose distribution for ISO Dose Node 
	vtkMRMLScalarVolumeNode* DoseDistribution = BDoseLogic->GetCalculatedDoseVolume();

	//vtkMRMLScalarVolumeNode* DoseDistribution = BDoseLogic->GetResampledDoseVolume();
	//********************************************************
	//Begin the ISO DOSE Evaluation Function and DVH Evaluation Function

	if (DoseDistribution)
	{
		this->enterIsoDoseEvaluationFunction(DoseDistribution);

		//Prepared for DVH Statistic
		this->ActiveDoseDistribution = DoseDistribution;

	    this->ActivesegmentationNode = segmentationNode;

	//	this->enterDVHDoseEvalutaionFunction(DoseDistribution, segmentationNode);

		//Show the ISO Dose GUI
		if (d->isoDoseGroup->isHidden())
		{
			d->isoDoseGroup->show();
			this->showDoseEvalulationWhenEnter = true;

		}


	 
		this->updateButtonsState();
		
	}
	
	 


	QApplication::restoreOverrideCursor();
}

//Given a Dose distribution Volume, enter the ISO DoseEvaluate Function
void qSRPlanPathPlanModuleWidget::enterIsoDoseEvaluationFunction(vtkMRMLScalarVolumeNode* doseGrid)
{
	Q_D(qSRPlanPathPlanModuleWidget);

	//**************************************************************************
	//	Construct the ISO Dose Node
	vtkSlicerIsodoseLogic *	isoLogic = vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic())->GetISODoseLogic();

	std::string volumeNodeName = doseGrid->GetName();

	char* DefaultDoseColorTableNodeId = isoLogic->GetDefaultIsodoseColorTableNodeId();

	// Create dose color table from default isodose color table
	
	vtkMRMLColorTableNode* defaultIsodoseColorTable = vtkMRMLColorTableNode::SafeDownCast(
		this->mrmlScene()->GetNodeByID(DefaultDoseColorTableNodeId));

	// Create isodose parameter set node and set color table to default
	std::string isodoseParameterSetNodeName;
	isodoseParameterSetNodeName =this->mrmlScene()->GenerateUniqueName(
		vtkSlicerIsodoseLogic::ISODOSE_PARAMETER_SET_BASE_NAME_PREFIX + volumeNodeName);

	vtkSmartPointer<vtkMRMLIsodoseNode> isodoseParameterSetNode = vtkSmartPointer<vtkMRMLIsodoseNode>::New();
	isodoseParameterSetNode->SetName(isodoseParameterSetNodeName.c_str());
	isodoseParameterSetNode->SetAndObserveDoseVolumeNode(doseGrid);

	if (isoLogic && defaultIsodoseColorTable)
	{
		isodoseParameterSetNode->SetAndObserveColorTableNode(defaultIsodoseColorTable);
	}
	this->mrmlScene()->AddNode(isodoseParameterSetNode);


	isoLogic->SetAndObserveIsodoseNode(isodoseParameterSetNode);

	// don't check the show ISO Dose Lines and Iso dose Surfaces as default
	isodoseParameterSetNode->SetShowIsodoseLines(false);
	isodoseParameterSetNode->SetShowIsodoseSurfaces(false);

	//************************************************************************
	//Setup  Dose SliceView


	// Set default colormap to the loaded one if found or generated, or to rainbow otherwise
	vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> volumeDisplayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();
	volumeDisplayNode->SetAndObserveColorNodeID(DefaultDoseColorTableNodeId);

	this->mrmlScene()->AddNode(volumeDisplayNode);
	doseGrid->SetAndObserveDisplayNodeID(volumeDisplayNode->GetID());

	// Set window/level to match the isodose levels
	if (isoLogic && defaultIsodoseColorTable)
	{
		/*
		std::stringstream ssMin;
		ssMin << defaultIsodoseColorTable->GetColorName(0);;
		int minDoseInDefaultIsodoseLevels;
		ssMin >> minDoseInDefaultIsodoseLevels;

		std::stringstream ssMax;
		ssMax << defaultIsodoseColorTable->GetColorName(defaultIsodoseColorTable->GetNumberOfColors() - 1);
		int maxDoseInDefaultIsodoseLevels;
		ssMax >> maxDoseInDefaultIsodoseLevels;
		*/
				
		// for debug show dose distribution


		volumeDisplayNode->AutoWindowLevelOff();
		volumeDisplayNode->SetWindowLevelMinMax( 0, 100);
		volumeDisplayNode->Modified();

	

		//set the minimun of ISO Level
	   // volumeDisplayNode->SetLowerThreshold(minDoseInDefaultIsodoseLevels);
	   // volumeDisplayNode->SetApplyThreshold(1);

	}


	// Set display threshold

	// volumeDisplayNode->AutoThresholdOff();


	

	//*************************************************************************
	//Setup the Dose Color Table view

	this->SetupScalarBarsShow();
	this->updateScalarBarsFromSelectedColorTable();

	this->SetupIsoDoseGroupGUIConnections();












}


/*

//------------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::CreateDefaultDoseColorTable()
{
	if (!this->GetMRMLScene() || !this->IsodoseLogic)
	{
		vtkErrorMacro("CreateDefaultDoseColorTable: No scene or Isodose logic present!");
		return;
	}

	// Check if default color table node already exists
	vtkSmartPointer<vtkCollection> defaultDoseColorTableNodes = vtkSmartPointer<vtkCollection>::Take(
		this->GetMRMLScene()->GetNodesByName(SlicerRtCommon::DICOMRTIMPORT_DEFAULT_DOSE_COLOR_TABLE_NAME));
	if (defaultDoseColorTableNodes->GetNumberOfItems() > 0)
	{
		vtkDebugMacro("CreateDefaultDoseColorTable: Default dose color table already exists");
		vtkMRMLColorTableNode* doseColorTable = vtkMRMLColorTableNode::SafeDownCast(
			defaultDoseColorTableNodes->GetItemAsObject(0));
		this->SetDefaultDoseColorTableNodeId(doseColorTable->GetID());
		return;
	}

	vtkMRMLColorTableNode* defaultIsodoseColorTable = vtkMRMLColorTableNode::SafeDownCast(
		this->GetMRMLScene()->GetNodeByID(this->IsodoseLogic->GetDefaultIsodoseColorTableNodeId()));
	if (!defaultIsodoseColorTable)
	{
		vtkErrorMacro("CreateDefaultDoseColorTable: Invalid default isodose color table found in isodose logic!");
		return;
	}

	vtkSmartPointer<vtkMRMLColorTableNode> defaultDoseColorTable = vtkSmartPointer<vtkMRMLColorTableNode>::New();
	defaultDoseColorTable->SetName(SlicerRtCommon::DICOMRTIMPORT_DEFAULT_DOSE_COLOR_TABLE_NAME);
	defaultDoseColorTable->SetTypeToUser();
	defaultDoseColorTable->SetSingletonTag(SlicerRtCommon::DICOMRTIMPORT_DEFAULT_DOSE_COLOR_TABLE_NAME);
	defaultDoseColorTable->SetAttribute("Category", SlicerRtCommon::SLICERRT_EXTENSION_NAME);
	defaultDoseColorTable->HideFromEditorsOff();
	defaultDoseColorTable->SetNumberOfColors(256);

	SlicerRtCommon::StretchDiscreteColorTable(defaultIsodoseColorTable, defaultDoseColorTable);

	this->GetMRMLScene()->AddNode(defaultDoseColorTable);
	this->SetDefaultDoseColorTableNodeId(defaultDoseColorTable->GetID());
}


*/


//set dose scalar bars in views 
void qSRPlanPathPlanModuleWidget::SetupScalarBarsShow()
{
	this->ScalarBarWidget = vtkScalarBarWidget::New();
	this->ScalarBarActor = vtkSlicerRTScalarBarActor::New();
	this->ScalarBarWidget->SetScalarBarActor(this->ScalarBarActor);
	this->ScalarBarWidget->GetScalarBarActor()->SetOrientationToVertical();
	this->ScalarBarWidget->GetScalarBarActor()->SetNumberOfLabels(6);
	this->ScalarBarWidget->GetScalarBarActor()->SetMaximumNumberOfColors(6);
	this->ScalarBarWidget->GetScalarBarActor()->SetTitle("RelativeDose(%)");
	this->ScalarBarWidget->GetScalarBarActor()->SetLabelFormat(" %s");

	// it's a 2d actor, position it in screen space by percentages
	this->ScalarBarWidget->GetScalarBarActor()->SetPosition(0.1, 0.1);
	this->ScalarBarWidget->GetScalarBarActor()->SetWidth(0.1);
	this->ScalarBarWidget->GetScalarBarActor()->SetHeight(0.8);

	this->ScalarBarWidget2DRed = vtkScalarBarWidget::New();
	this->ScalarBarActor2DRed = vtkSlicerRTScalarBarActor::New();
	this->ScalarBarWidget2DRed->SetScalarBarActor(this->ScalarBarActor2DRed);
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetOrientationToVertical();
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetNumberOfLabels(6);
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetMaximumNumberOfColors(6);
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetTitle("RelativeDose(%)");
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetLabelFormat(" %s");

	// it's a 2d actor, position it in screen space by percentages
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetPosition(0.1, 0.1);
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetWidth(0.1);
	this->ScalarBarWidget2DRed->GetScalarBarActor()->SetHeight(0.8);

	this->ScalarBarWidget2DYellow = vtkScalarBarWidget::New();
	this->ScalarBarActor2DYellow = vtkSlicerRTScalarBarActor::New();
	this->ScalarBarWidget2DYellow->SetScalarBarActor(this->ScalarBarActor2DYellow);
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetOrientationToVertical();
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetNumberOfLabels(6);
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetMaximumNumberOfColors(6);
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetTitle("RelativeDose(%)");
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetLabelFormat(" %s");

	// it's a 2d actor, position it in screen space by percentages
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetPosition(0.1, 0.1);
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetWidth(0.1);
	this->ScalarBarWidget2DYellow->GetScalarBarActor()->SetHeight(0.8);

	this->ScalarBarWidget2DGreen = vtkScalarBarWidget::New();
	this->ScalarBarActor2DGreen = vtkSlicerRTScalarBarActor::New();
	this->ScalarBarWidget2DGreen->SetScalarBarActor(this->ScalarBarActor2DGreen);
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetOrientationToVertical();
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetNumberOfLabels(6);
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetMaximumNumberOfColors(6);
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetTitle("RelativeDose(%)");
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetLabelFormat(" %s");

	// it's a 2d actor, position it in screen space by percentages
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetPosition(0.1, 0.1);
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetWidth(0.1);
	this->ScalarBarWidget2DGreen->GetScalarBarActor()->SetHeight(0.8);

}



void qSRPlanPathPlanModuleWidget::updateScalarBarsFromSelectedColorTable()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	vtkMRMLIsodoseNode* paramNode = this->getIsodoseLogic()->GetIsodoseNode();

	if (!this->mrmlScene() || !paramNode)
	{
		return;
	}

	vtkMRMLColorTableNode* selectedColorNode = paramNode->GetColorTableNode();
	if (!selectedColorNode)
	{
		qDebug() << "qSlicerIsodoseModuleWidgetPrivate::updateScalarBarsFromSelectedColorTable: No color table node is selected";
		return;
	}

	d->tableView_IsodoseLevels->setMRMLColorNode(selectedColorNode);

	
	// 3D scalar bar
	int numberOfColors = selectedColorNode->GetNumberOfColors();
	this->ScalarBarWidget->GetScalarBarActor()->SetLookupTable(selectedColorNode->GetLookupTable());

	for (int colorIndex = 0; colorIndex<numberOfColors; ++colorIndex)
	{
		this->ScalarBarActor->GetLookupTable()->SetAnnotation(colorIndex, vtkStdString(selectedColorNode->GetColorName(colorIndex)));
	}
	// 2D scalar bar
	this->ScalarBarActor2DRed->SetLookupTable(selectedColorNode->GetLookupTable());
	this->ScalarBarActor2DYellow->SetLookupTable(selectedColorNode->GetLookupTable());
	this->ScalarBarActor2DGreen->SetLookupTable(selectedColorNode->GetLookupTable());

	for (int colorIndex = 0; colorIndex<numberOfColors; ++colorIndex)
	{
		this->ScalarBarActor2DRed->GetLookupTable()->SetAnnotation(colorIndex, vtkStdString(selectedColorNode->GetColorName(colorIndex)));
		this->ScalarBarActor2DYellow->GetLookupTable()->SetAnnotation(colorIndex, vtkStdString(selectedColorNode->GetColorName(colorIndex)));
		this->ScalarBarActor2DGreen->GetLookupTable()->SetAnnotation(colorIndex, vtkStdString(selectedColorNode->GetColorName(colorIndex)));
	}
}

//set up the Iso Dose Evalution guis
void qSRPlanPathPlanModuleWidget::SetupIsoDoseGroupGUIConnections()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	// Each time the node is modified, the qt widgets are updated
	//qvtkReconnect(d->logic()->GetIsodoseNode(), paramNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()));

	// Make connections
	

//	connect(d->spinBox_NumberOfLevels, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfLevels(int)));
//	connect(d->checkBox_Isoline, SIGNAL(toggled(bool)), this, SLOT(setIsolineVisibility(bool)));
//	connect(d->checkBox_Isosurface, SIGNAL(toggled(bool)), this, SLOT(setIsosurfaceVisibility(bool)));
//	connect(d->checkBox_ScalarBar, SIGNAL(toggled(bool)), this, SLOT(setScalarBarVisibility(bool)));
	connect(d->checkBox_ScalarBar2D, SIGNAL(toggled(bool)), this, SLOT(setScalarBar2DVisibility(bool)));

	connect(d->pushButton_Apply, SIGNAL(clicked()), this, SLOT(applyClicked()));



	qSlicerApplication * app = qSlicerApplication::application();
	if (app && app->layoutManager())
	{
		qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
		vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
		if (activeRenderer)
		{
			this->ScalarBarWidget->SetInteractor(activeRenderer->GetRenderWindow()->GetInteractor());
		}
//		connect(d->checkBox_ScalarBar, SIGNAL(stateChanged(int)), threeDView, SLOT(scheduleRender()));

		QStringList sliceViewerNames = app->layoutManager()->sliceViewNames();
		qMRMLSliceWidget* sliceViewerWidgetRed = app->layoutManager()->sliceWidget(sliceViewerNames[0]);
		const qMRMLSliceView* sliceViewRed = sliceViewerWidgetRed->sliceView();
		this->ScalarBarWidget2DRed->SetInteractor(sliceViewerWidgetRed->interactorStyle()->GetInteractor());
		qMRMLSliceWidget* sliceViewerWidgetYellow = app->layoutManager()->sliceWidget(sliceViewerNames[1]);
		const qMRMLSliceView* sliceViewYellow = sliceViewerWidgetYellow->sliceView();
		this->ScalarBarWidget2DYellow->SetInteractor(sliceViewerWidgetYellow->interactorStyle()->GetInteractor());
		qMRMLSliceWidget* sliceViewerWidgetGreen = app->layoutManager()->sliceWidget(sliceViewerNames[2]);
		const qMRMLSliceView* sliceViewGreen = sliceViewerWidgetGreen->sliceView();
		this->ScalarBarWidget2DGreen->SetInteractor(sliceViewerWidgetGreen->interactorStyle()->GetInteractor());

		connect(d->checkBox_ScalarBar2D, SIGNAL(stateChanged(int)), sliceViewRed, SLOT(scheduleRender()));
		connect(d->checkBox_ScalarBar2D, SIGNAL(stateChanged(int)), sliceViewYellow, SLOT(scheduleRender()));
		connect(d->checkBox_ScalarBar2D, SIGNAL(stateChanged(int)), sliceViewGreen, SLOT(scheduleRender()));
	}

	// Handle scene change event if occurs
	//qvtkConnect(this->logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified()));

	// Select the default color node
	this->updateScalarBarsFromSelectedColorTable();

	//this->updateButtonsState();








}



void qSRPlanPathPlanModuleWidget::updateIsoDoseGroupWidgetFromMRML()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	vtkMRMLIsodoseNode* paramNode   = this->getIsodoseLogic()->GetIsodoseNode();
	if (paramNode && this->mrmlScene())
	{
	
		this->updateScalarBarsFromSelectedColorTable();

		vtkMRMLColorTableNode* colorTableNode = paramNode->GetColorTableNode();
		if (!colorTableNode)
		{
			qCritical() << "qSlicerIsodoseModuleWidget::updateWidgetFromMRML: Invalid color table node!";
			return;
		}
//		d->spinBox_NumberOfLevels->setValue(colorTableNode->GetNumberOfColors());
//		d->checkBox_Isoline->setChecked(paramNode->GetShowIsodoseLines());
//		d->checkBox_Isosurface->setChecked(paramNode->GetShowIsodoseSurfaces());
	}
}









void qSRPlanPathPlanModuleWidget::onRealTracePushButtonClicked()
{
	Q_D(qSRPlanPathPlanModuleWidget);
	if (!d->isoDoseGroup->isHidden())
    	d->isoDoseGroup->hide();

	d->traceMarkGroup->show();
	 

	bool checked = d->realTracePushButton->isChecked();

	QString timestring = QString(vtksys::SystemTools::GetEnv("SNAKE_UPDATE_TIME_MS"));
 
	int timeSnap = timestring.toInt();




	//if checked ,start to tracing the snake motion,else stop the motion tracing
	if (checked)
	{
		d->doseCalculatePushButton->setDisabled(true);

		// get the active node
		vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
		vtkMRMLMarkupsNode *listNode = NULL;
		if (mrmlNode)
		{
			listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
		}
		if (listNode)
		{
			const char * realTracLabel = vtkMRMLMarkupsNode::GetRealTraceMarkupLabel();

			//Scale out for 3D Tracing show
			//vtkMRMLMarkupsDisplayNode * displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(listNode->GetDisplayNode());

			//displayNode->SetGlyphScale(6.0);

			//make sure there is a tracing flag
			bool exist = listNode->ExistMarkup(realTracLabel);

			if (exist)
			{
				if (this->tracingTimer == NULL)
				{
					this->tracingTimer = new QTimer(this);
					connect(tracingTimer, SIGNAL(timeout()), this, SLOT(UpdateTraceMarkPosition()));
				}
				

			
				this->tracingTimer->start(timeSnap);


			}
			else
			{
				// for now, assume a fiducial
				listNode->AddMarkupWithNPoints(1, realTracLabel);
				if (this->tracingTimer == NULL)
				{
					this->tracingTimer = new QTimer(this);
					connect(tracingTimer, SIGNAL(timeout()), this, SLOT(UpdateTraceMarkPosition()));

				}
			

				this->tracingTimer->start(timeSnap);

			}
			 
		
			
		}

	}
	else
	{
		
		this->tracingTimer->stop();

		d->doseCalculatePushButton->setEnabled(true);

	//	disconnect(tracingTimer, SIGNAL(timeout()), this, SLOT(UpdateTraceMarkPosition()));

		//Scale in ,restore the markup to defalt display

		/*
		vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
		vtkMRMLMarkupsNode *listNode = NULL;
		if (mrmlNode)
		{
			listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
		}
		if (listNode)
		{
		
			vtkMRMLMarkupsDisplayNode * displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(listNode->GetDisplayNode());

			displayNode->SetGlyphScale( 2.1); //Default Vale is 2.1

		}
		*/

	}







}


void qSRPlanPathPlanModuleWidget::UpdateTraceMarkPosition()
{
	Q_D(qSRPlanPathPlanModuleWidget);
	// get the active node
	vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
	vtkMRMLMarkupsNode *listNode = NULL;

	

	if (mrmlNode)
	{
		listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
	}
	if (listNode)
	{
		const char * realTracLabel = vtkMRMLMarkupsNode::GetRealTraceMarkupLabel();

		Markup * markup = listNode->GetMarkupByLabel(realTracLabel);

		 

		//All the Markups have been deleted,
		if (!markup)
		{
			this->tracingTimer->stop();
			disconnect(tracingTimer, SIGNAL(timeout()), this, SLOT(UpdateTraceMarkPosition()));
			return;
		}
		


		//the TMark index
		int index = listNode->GetMarkupIndexByByLabel(realTracLabel);

		static unsigned int file_count = 0;

		std::string realTracFile;

		std::stringstream count;
		count << file_count;
		std::string a;
		count >> a;



		realTracFile = std::string(vtksys::SystemTools::GetEnv("OPTIC_TRAC") + a + std::string(".raw"));




	//	realTracFile = std::string(vtksys::SystemTools::GetEnv("OPTIC_TRAC_FILE"));

	//	if (vtksys::SystemTools::GetEnv("OPTIC_TRAC") != NULL)
	//	{
	//		realTracFile = std::string(vtksys::SystemTools::GetEnv("OPTIC_TRAC") + std::string("\\RealtimePosition.txt"));
	//	}

		//QFile opticTracFile("D:/00-SRPlan/SRPlan4SnakeVC2015Bin/Output/share/NetworkShare/RealtimePosition.txt");
		//QFile opticTracFile("E:/CodeLearn/RealtimePosition.txt");

		QFile opticTracFile(realTracFile.c_str());
		
		bool suc = false;
		if (opticTracFile.exists())
		{
			qDebug(realTracFile.c_str());
			suc = opticTracFile.open(QIODevice::ReadOnly);
		}
		else
		{
			qDebug("don't have this file:%d.raw", file_count);
			//   bool suc = opticTracFile.open(QIODevice::ReadOnly);

		}

		if (suc)
		{


			QByteArray bytes = opticTracFile.readAll();
			float x, y, z;
			double x1, y1, z1;
           
			double Direction[3];

			file_count++;

			// Get two point coordinates
			x = ((unsigned char)bytes[1] * 256 + (unsigned char)bytes[0] - 35000.0) / 100.0;
			y = ((unsigned char)bytes[3] * 256 + (unsigned char)bytes[2] - 35000.0) / 100.0;
			z = ((unsigned char)bytes[5] * 256 + (unsigned char)bytes[4] - 35000.0) / 100.0;
			x1 = ((unsigned char)bytes[7] * 256 + (unsigned char)bytes[6] - 35000.0) / 100.0;
			y1 = ((unsigned char)bytes[9] * 256 + (unsigned char)bytes[8] - 35000.0) / 100.0;
			z1 = ((unsigned char)bytes[11] * 256 + (unsigned char)bytes[10] - 35000.0) / 100.0;

		

			//Start (x1,y1,z1) point to End (x,y,z)

			Direction[0] = x - x1;
			Direction[1] = y - y1;
			Direction[2] = z - z1;

			int pointIndex = 0;


			
 
			
			//**********************************************
			//Only for Debug
			//rand()%(max-min + 1) + min (range [min max])
			
			if (x < -96 || x > 55)
			{
				cout << "error: " << "The optic tracing  x value out of range:  x = " << x << endl;
				x = rand() % 24 + 5;
				cout << "Force to :  " << " x = " << x << endl;

			}
		
			if (y < -55 || y > 55)
		    {
				cout << "error: " << "The optic tracing  y value out of range:  y = " << y << endl;
				y = rand() % 24 + 5;
				cout << "Force to :  " << " x = " << x << endl;

			}
    
			if (z < -55 || z > 55)
			{
				cout << "error: " << "The optic tracing  y value out of range:  y = " << y << endl;
				z = rand() % 24 + 5;

				cout << "Force to :  " << " x = " << x << endl;
			}
			
			//Only for Debug 
			//**********************************************
			
			/*
			//***************************************************************
			//Real needed by Real tracing
			bool outofrange = this->isOutofPrimaryImageRange(x, y, z);

			if (outofrange)
			{
				opticTracFile.close();
				return;

			}
			// End Real needed by Real tracing
			//*****************************************************************
			*/

			markup->points[pointIndex].SetX(x);
			markup->points[pointIndex].SetY(y);
			markup->points[pointIndex].SetZ(z);

			// throw an event to let listeners know the position has changed
			listNode->Modified();
			listNode->InvokeCustomModifiedEvent(vtkMRMLMarkupsNode::PointModifiedEvent, (void*)&index);


			

			this->SaveSnakeHeadDirectionToParametersNode(Direction);

		//	vtkSRPlanPathPlanModuleLogic * Mlogic = vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic());
			
		//	Mlogic->GetMarkupsLogic()->JumpSlicesToNthPointInMarkup(fidList->GetID(), index, true);

		//	Mlogic->GetMarkupsLogic()->JumpSlicesToLocation(x, y, z, true);
			cout << x << ' ' << y << ' ' << z << endl;
		
		}

		opticTracFile.close();
		//temp comment out for debug
	//	opticTracFile.remove(realTracFile.c_str());  
	}
	
	

}

//Test the (x,y,z) Whether in the Primary Image Range,
bool qSRPlanPathPlanModuleWidget::isOutofPrimaryImageRange(double x, double y, double z)
{
	vtkMRMLScalarVolumeNode * planPrimaryVolume = this->getBDoseCalculateLogic()->GetPlanPrimaryVolumeNode();

	if (!planPrimaryVolume)
	{

		vtkMRMLGeneralParametersNode*  parametersNode = vtkMRMLSceneUtility::GetParametersNode(this->mrmlScene());
		std::string primaryVolumeID = parametersNode->GetParameter("PrimaryPlanVolumeNodeID");

		planPrimaryVolume = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(primaryVolumeID.c_str()));
		if (!planPrimaryVolume)
		{
			return true;

		}
	}



	double rasBounds[6];
	planPrimaryVolume->GetRASBounds(rasBounds);


	double rmin, rmax, amin, amax, smin, smax;
	rmin = rasBounds[0];
	rmax = rasBounds[1];
	amin = rasBounds[2];
	amax = rasBounds[3];
	smin = rasBounds[4];
	smax = rasBounds[5];


	if (x < rmin || x > rmax)
	{
		cout << "error: " << "The optic tracing  x value out of range:  x = " << x << endl;
		
		return true;

	}

	if (y < amin || y > amax)
	{
		cout << "error: " << "The optic tracing  y value out of range:  y = " << y << endl;
		 
		return true;
	}

	if (z < smin || z > smax)
	{
		cout << "error: " << "The optic tracing  z value out of range:  z = " << z << endl;
		 
		return true;
	}

	return false;

}









void qSRPlanPathPlanModuleWidget::SaveSnakeHeadDirectionToParametersNode(double * directionxyz)
{
	if (!m_parametersNode)
	{
		vtkMRMLGeneralParametersNode*  parametersNode = vtkMRMLSceneUtility::GetParametersNode(this->mrmlScene());
		if (!parametersNode)
		{
			parametersNode = vtkMRMLSceneUtility::CreateParametersNode(this->mrmlScene());
		}

		m_parametersNode = parametersNode;

		m_parametersNode->SetParameter("SnakeHeadDirectionX", QString::number(directionxyz[0]).toStdString());
		m_parametersNode->SetParameter("SnakeHeadDirectionY", QString::number(directionxyz[1]).toStdString());
		m_parametersNode->SetParameter("SnakeHeadDirectionZ", QString::number(directionxyz[2]).toStdString());

	}
	else
	{
		m_parametersNode->SetParameter("SnakeHeadDirectionX", QString::number(directionxyz[0]).toStdString());
		m_parametersNode->SetParameter("SnakeHeadDirectionY", QString::number(directionxyz[1]).toStdString());
		m_parametersNode->SetParameter("SnakeHeadDirectionZ", QString::number(directionxyz[2]).toStdString());


	}


}


void qSRPlanPathPlanModuleWidget::updateTMarkOpacitytoParametersNode(double opacity)
{
	if (!m_parametersNode)
	{
		vtkMRMLGeneralParametersNode*  parametersNode = vtkMRMLSceneUtility::GetParametersNode(this->mrmlScene());
		if (!parametersNode)
		{
			parametersNode = vtkMRMLSceneUtility::CreateParametersNode(this->mrmlScene());
		}

		m_parametersNode = parametersNode;
		m_parametersNode->SetParameter("SnakeHeadOpacity", QString::number(opacity).toStdString());
	}
	else
	{
		m_parametersNode->SetParameter("SnakeHeadOpacity", QString::number(opacity).toStdString());

	}

}



//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onDeleteMarkupPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the selected rows
  QList<QTableWidgetItem *> selectedItems = d->activeMarkupTableWidget->selectedItems();

  // first, check if nothing is selected
  if (selectedItems.isEmpty())
    {
    return;
    }

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (!listNode)
    {
    qDebug() << "Delete markup: no active list from which to delete";
    return;
    }

  // iterate over the selected items and save their row numbers (there are
  // selected indices for each column in a row, so jump by the number of
  // columns), so can delete without relying on the table
  QList<int> rows;
  for (int i = 0; i < selectedItems.size(); i += d->numberOfColumns())
    {
    // get the row
    int row = selectedItems.at(i)->row();
    // qDebug() << "Saving: i = " << i << ", row = " << row;
    rows << row;
    }
  // sort the list
  qSort(rows);

  ctkMessageBox deleteAllMsgBox;
  deleteAllMsgBox.setWindowTitle("Delete Markups in this list?");
  QString labelText = QString("Delete ")
    + QString::number(rows.size())
    + QString(" Markups from this list?");
  // don't show again check box conflicts with informative text, so use
  // a long text
  deleteAllMsgBox.setText(labelText);

  QPushButton *deleteButton =
    deleteAllMsgBox.addButton(tr("Delete"), QMessageBox::AcceptRole);
  deleteAllMsgBox.addButton(QMessageBox::Cancel);
  deleteAllMsgBox.setDefaultButton(deleteButton);
  deleteAllMsgBox.setIcon(QMessageBox::Question);
  deleteAllMsgBox.setDontShowAgainVisible(true);
  deleteAllMsgBox.setDontShowAgainSettingsKey("Markups/AlwaysDeleteMarkups");
  deleteAllMsgBox.exec();
  if (deleteAllMsgBox.clickedButton() == deleteButton)
    {
    // delete from the end
    for (int i = rows.size() - 1; i >= 0; --i)
      {
      int index = rows.at(i);
      // qDebug() << "Deleting: i = " << i << ", index = " << index;
      // remove the markup at that row
      listNode->RemoveMarkup(index);
      }
    }

  // clear the selection on the table
  d->activeMarkupTableWidget->clearSelection();

  this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();

  emit DoseInvalided();


 

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onDeleteAllMarkupsInListPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);
  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (listNode)
    {
    // qDebug() << "Removing markups from list " << listNode->GetName();
    ctkMessageBox deleteAllMsgBox;
    deleteAllMsgBox.setWindowTitle("Delete All Markups in this list?");
    QString labelText = QString("Delete all ")
      + QString::number(listNode->GetNumberOfMarkups())
      + QString(" Markups in this list?");
    // don't show again check box conflicts with informative text, so use
    // a long text
    deleteAllMsgBox.setText(labelText);

    QPushButton *deleteButton =
      deleteAllMsgBox.addButton(tr("Delete All"), QMessageBox::AcceptRole);
    deleteAllMsgBox.addButton(QMessageBox::Cancel);
    deleteAllMsgBox.setDefaultButton(deleteButton);
    deleteAllMsgBox.setIcon(QMessageBox::Question);
    deleteAllMsgBox.setDontShowAgainVisible(true);
    deleteAllMsgBox.setDontShowAgainSettingsKey("Markups/AlwaysDeleteAllMarkups");
    deleteAllMsgBox.exec();
    if (deleteAllMsgBox.clickedButton() == deleteButton)
      {
      listNode->RemoveAllMarkups();
      }
    }

  this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();

  emit DoseInvalided();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupMRMLNodeChanged(vtkMRMLNode *markupsNode)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  //qDebug() << "onActiveMarkupMRMLNodeChanged, markupsNode is " << (markupsNode ? markupsNode->GetID() : "null");

  // update the selection node
  vtkMRMLSelectionNode *selectionNode = NULL;
  if (this->mrmlScene() && this->markupsLogic())
    {
    selectionNode = vtkMRMLSelectionNode::SafeDownCast(
          this->mrmlScene()->GetNodeByID(this->markupsLogic()->GetSelectionNodeID().c_str()));
    }
  if (selectionNode)
    {
    // check if changed
    const char *selectionNodeActivePlaceNodeID = selectionNode->GetActivePlaceNodeID();

    const char *activeID = NULL;
    if (markupsNode)
      {
      activeID = markupsNode->GetID();
      }

    // get the current node from the combo box
    //QString activeMarkupsNodeID = d->activeMarkupMRMLNodeComboBox->currentNodeID();
    //qDebug() << "setActiveMarkupsNode: combo box says: " << qPrintable(activeMarkupsNodeID) << ", input node says " << (activeID ? activeID : "null");

    // don't update the selection node if the active ID is null (can happen
    // when entering the module)
    if (activeID != NULL)
      {
      if (!selectionNodeActivePlaceNodeID || !activeID ||
          strcmp(selectionNodeActivePlaceNodeID, activeID) != 0)
        {
        selectionNode->SetReferenceActivePlaceNodeID(activeID);
        }
      }
    else
      {
      if (selectionNodeActivePlaceNodeID != NULL)
        {
        //std::cout << "Setting combo box from selection node " << selectionNodeActivePlaceNodeID << std::endl;
        d->activeMarkupMRMLNodeComboBox->setCurrentNodeID(selectionNodeActivePlaceNodeID);
        }
      }
    }

  // update the GUI
  this->updateWidgetFromMRML();

  this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
  emit DoseInvalided();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupMRMLNodeAdded(vtkMRMLNode *markupsNode)
{
	Q_D(qSRPlanPathPlanModuleWidget);
  // qDebug() << "onActiveMarkupMRMLNodeAdded, markupsNode is " << (markupsNode ? markupsNode->GetID() : "null");

  if (this->markupsLogic())
    {
    this->markupsLogic()->AddNewDisplayNodeForMarkupsNode(markupsNode);
    }
  // update the visibility button
  vtkMRMLMarkupsNode *displayableNode = NULL;
  if (markupsNode)
    {
    displayableNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNode);
    }
  if (displayableNode)
    {
    int visibleFlag = displayableNode->GetDisplayVisibility();
    this->updateListVisibileInvisiblePushButton(visibleFlag);
    }

  // make sure it's set up for the mouse mode tool bar to easily add points to
  // it by making it active in the selection node
  if (this->markupsLogic())
    {
    this->markupsLogic()->SetActiveListID(displayableNode);
    }

  d->doseCalculatePushButton->setEnabled(true);
  d->realTracePushButton->setEnabled(true);

  //this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();


}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onSelectionNodeActivePlaceNodeIDChanged()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  //qDebug() << "onSelectionNodeActivePlaceNodeIDChanged";
  std::string listID = (this->markupsLogic() ? this->markupsLogic()->GetActiveListID() : std::string(""));
  QString activePlaceNodeID = QString(listID.c_str());
  if (activePlaceNodeID.size() == 0)
    {
    qDebug() << "onSelectionNodeActivePlaceNodeIDChanged: No current active place node id";
    d->activeMarkupMRMLNodeComboBox->setCurrentNodeID("");
    }
  else
    {
    QString currentNodeID = d->activeMarkupMRMLNodeComboBox->currentNodeID();
    //std::cout << "\t\tcombo box current node id = " << qPrintable(currentNodeID) << std::endl;
    if (currentNodeID == "" ||
        (currentNodeID != activePlaceNodeID &&
         activePlaceNodeID.contains("vtkMRMLMarkups")))
      {
      d->activeMarkupMRMLNodeComboBox->setCurrentNodeID(activePlaceNodeID);
      }
    }

  this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
  emit DoseInvalided();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onListVisibileInvisiblePushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);
  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (!listNode)
    {
    return;
    }
  // toggle the visibility
  int visibleFlag = listNode->GetDisplayVisibility();
  visibleFlag = !visibleFlag;
  listNode->SetDisplayVisibility(visibleFlag);
  this->updateListVisibileInvisiblePushButton(visibleFlag);
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateListVisibileInvisiblePushButton(int visibleFlag)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // update the list visibility button icon and tool tip
  if (visibleFlag)
    {
    d->listVisibileInvisiblePushButton->setIcon(QIcon(":Icons/Medium/SlicerVisible.png"));
    d->listVisibileInvisiblePushButton->setToolTip(QString("Click to hide this markup list"));
    }
  else
    {
    d->listVisibileInvisiblePushButton->setIcon(QIcon(":Icons/Medium/SlicerInvisible.png"));
    d->listVisibileInvisiblePushButton->setToolTip(QString("Click to show this markup list"));
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onListLockedUnlockedPushButtonClicked()
{
  Q_D(qSRPlanPathPlanModuleWidget);
  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (!listNode)
    {
    return;
    }
  // toggle the lock
  int locked = listNode->GetLocked();
  locked = !locked;
  listNode->SetLocked(locked);

  // update the button
  if (locked)
    {
    d->listLockedUnlockedPushButton->setIcon(QIcon(":Icons/Medium/SlicerLock.png"));
    d->listLockedUnlockedPushButton->setToolTip(QString("Click to unlock this markup list so that the markups can be moved by the mouse"));
    }
  else
    {
    d->listLockedUnlockedPushButton->setIcon(QIcon(":Icons/Medium/SlicerUnlock.png"));
    d->listLockedUnlockedPushButton->setToolTip(QString("Click to lock this markup list so that the markups cannot be moved by the mouse"));
    }
}


//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupTableCellChanged(int row, int column)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  

//  qDebug() << QString("cell changed: row = ") + QString::number(row) + QString(", col = ") + QString::number(column);
  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  vtkMRMLMarkupsNode *listNode = NULL;
  if (mrmlNode)
    {
    listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
    }
  if (!listNode)
    {
    qDebug() << QString("Cell Changed: unable to get current list");
    return;
    }

  // row corresponds to the index in the list
  int n = row;

  // now switch on the property
  QTableWidgetItem *item = d->activeMarkupTableWidget->item(row, column);
  if (!item)
    {
    qDebug() << QString("Unable to find item in table at ") + QString::number(row) + QString(", ") + QString::number(column);
    return;
    }
  if (column == d->columnIndex("Selected"))
    {
    bool flag = (item->checkState() == Qt::Unchecked ? false : true);
    listNode->SetNthMarkupSelected(n, flag);
    }
  else if (column == d->columnIndex("Locked"))
    {
    bool flag = item->data(Qt::UserRole) == QVariant(true) ? true : false;
    // update the icon
    if (flag)
      {
      item->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerLock.png"));
      }
    else
      {
      item->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerUnlock.png"));
      }
    listNode->SetNthMarkupLocked(n, flag);
    }
  else if (column == d->columnIndex("Visible"))
    {
    bool flag = item->data(Qt::UserRole) == QVariant(true) ? true : false;
    // update the eye icon
    if (flag)
      {
      item->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerVisible.png"));
      }
    else
      {
      item->setData(Qt::DecorationRole, QPixmap(":/Icons/Small/SlicerInvisible.png"));
      }
    listNode->SetNthMarkupVisibility(n, flag);
    }
  else if (column ==  d->columnIndex("Name"))
    {
    std::string name = std::string(item->text().toLatin1());
    listNode->SetNthMarkupLabel(n, name);
    }
  else if (column ==  d->columnIndex("Weight"))
    {
     float weight =  item->text().toFloat();
     listNode->SetNthMarkupWeight(n, weight);

	 this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
	 emit DoseInvalided();

	 this->updateButtonsState();

    }
  else if (column == d->columnIndex("R") ||
           column == d->columnIndex("A") ||
           column == d->columnIndex("S"))
    {
    // get the new value
    double newPoint[3] = {0.0, 0.0, 0.0};
    if (d->activeMarkupTableWidget->item(row, d->columnIndex("R")) == NULL ||
        d->activeMarkupTableWidget->item(row, d->columnIndex("A")) == NULL ||
        d->activeMarkupTableWidget->item(row, d->columnIndex("S")) == NULL)
      {
      // init state, return
      return;
      }
    newPoint[0] = d->activeMarkupTableWidget->item(row, d->columnIndex("R"))->text().toDouble();
    newPoint[1] = d->activeMarkupTableWidget->item(row, d->columnIndex("A"))->text().toDouble();
    newPoint[2] = d->activeMarkupTableWidget->item(row, d->columnIndex("S"))->text().toDouble();

    // get the old value
    double point[3] = {0.0, 0.0, 0.0};
  
    listNode->GetMarkupPoint(n, 0, point);


    // changed?
    double minChange = 0.001;
    if (fabs(newPoint[0] - point[0]) > minChange ||
        fabs(newPoint[1] - point[1]) > minChange ||
        fabs(newPoint[2] - point[2]) > minChange)
      {
      vtkMRMLMarkupsFiducialNode *fidList = vtkMRMLMarkupsFiducialNode::SafeDownCast(listNode);
      if (fidList)
        {
        
          fidList->SetNthFiducialPositionFromArray(n, newPoint);
         
        }
      }
    else
      {
      //qDebug() << QString("Cell changed: no change in location bigger than ") + QString::number(minChange);
      }

	//this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
	//this->updateButtonsState();
    }
  else
    {
    qDebug() << QString("Cell Changed: unknown column: ") + QString::number(column);
    }
	
 

}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupTableCellClicked(QTableWidgetItem* item)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  if (item == 0)
    {
    return;
    }

  //int row = item->row();
  int column = item->column();
  //qDebug() << "onActiveMarkupTableCellClicked: row = " << row << ", col = " << column;

  if (column == d->columnIndex(QString("Visible")) ||
           column == d->columnIndex(QString("Locked")))
    {
    // toggle the user role, the icon update is triggered by this change
    if (item->data(Qt::UserRole) == QVariant(false))
      {
      item->setData(Qt::UserRole, QVariant(true));
      }
    else
      {
      item->setData(Qt::UserRole, QVariant(false));
      }
    }

}
//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupTableCurrentCellChanged(
     int currentRow, int currentColumn, int previousRow, int previousColumn)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  Q_UNUSED(currentColumn);
  Q_UNUSED(previousRow);
  Q_UNUSED(previousColumn);

  


  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  // offset or center?
  bool jumpCentered = false;
  //if (d->jumpCenteredRadioButton->isChecked())
 //   {
  //  jumpCentered = true;
 //   }
  // jump to it
  if (this->markupsLogic())
    {
    this->markupsLogic()->JumpSlicesToNthPointInMarkup(mrmlNode->GetID(), currentRow, jumpCentered);
    }

 // this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onRightClickActiveMarkupTableWidget(QPoint pos)
{
  Q_UNUSED(pos);

  // qDebug() << "onRightClickActiveMarkupTableWidget: pos = " << pos;

  QMenu menu;
  this->addSelectedCoordinatesToMenu(&menu);

  // Delete
  QAction *deleteFiducialAction =
    new QAction(QString("Delete highlighted fiducial(s)"), &menu);
  menu.addAction(deleteFiducialAction);
  QObject::connect(deleteFiducialAction, SIGNAL(triggered()),
                   this, SLOT(onDeleteMarkupPushButtonClicked()));

  // Jump slices
  QAction *jumpSlicesAction =
    new QAction(QString("Jump slices"), &menu);
  menu.addAction(jumpSlicesAction);
  QObject::connect(jumpSlicesAction, SIGNAL(triggered()),
                   this, SLOT(onJumpSlicesActionTriggered()));

  // Refocus 3D cameras
  QAction *refocusCamerasAction =
    new QAction(QString("Refocus all cameras"), &menu);
  menu.addAction(refocusCamerasAction);
  QObject::connect(refocusCamerasAction, SIGNAL(triggered()),
                   this, SLOT(onRefocusCamerasActionTriggered()));

  // If there's another list in the scene
  if (this->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLMarkupsNode") > 1)
    {
    // copy to another list
    QAction *copyToOtherListAction =
      new QAction(QString("Copy fiducial to another list"), &menu);
    menu.addAction(copyToOtherListAction);
    QObject::connect(copyToOtherListAction, SIGNAL(triggered()),
                     this, SLOT(onCopyToOtherListActionTriggered()));

    // move to another list
    QAction *moveToOtherListAction =
      new QAction(QString("Move fiducial to another list"), &menu);
    menu.addAction(moveToOtherListAction);
    QObject::connect(moveToOtherListAction, SIGNAL(triggered()),
                     this, SLOT(onMoveToOtherListActionTriggered()));
    }
  menu.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::addSelectedCoordinatesToMenu(QMenu *menu)
{
  Q_D(qSRPlanPathPlanModuleWidget);


  /*
  // get the selected rows
  QList<QTableWidgetItem *> selectedItems = d->activeMarkupTableWidget->selectedItems();

  // first, check if nothing is selected
  if (selectedItems.isEmpty())
    {
    return;
    }

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    return;
    }

  // get the list of selected rows to sort them in index order
  QList<int> rows;
  // The selected items list contains an item for each column in each row that
  // has been selected. Don't make any assumptions about the order of the
  // selected items, iterate through all of them and collect unique rows
  for (int i = 0; i < selectedItems.size(); ++i)
    {
    // get the row
    int row = selectedItems.at(i)->row();
    if (!rows.contains(row))
      {
      rows << row;
      }
    }
  // sort the list
  qSort(rows);

  // keep track of point to point distance
  double distance = 0.0;
  double lastPoint[3] = {0.0, 0.0, 0.0};

  // loop over the selected rows
  for (int i = 0; i < rows.size() ; i++)
    {
    int row = rows.at(i);
    int numPoints = markupsNode->GetNumberOfPointsInNthMarkup(row);
    // label this selected markup if more than one
    if (rows.size() > 1)
      {
      QString indexString =  QString(markupsNode->GetNthMarkupLabel(row).c_str()) +
        QString(":");
      menu->addAction(indexString);
      }
    for (int p = 0; p < numPoints; ++p)
      {
      double point[3] = {0.0, 0.0, 0.0};
      if (d->transformedCoordinatesCheckBox->isChecked())
        {
        double worldPoint[4] = {0.0, 0.0, 0.0, 1.0};
        markupsNode->GetMarkupPointWorld(row, p, worldPoint);
        for (int p = 0; p < 3; ++p)
          {
          point[p] = worldPoint[p];
          }
        }
      else
        {
        markupsNode->GetMarkupPoint(row, p, point);
        }
      // format the coordinates
      QString coordinate =
        QString::number(point[0]) + QString(",") +
        QString::number(point[1]) + QString(",") +
        QString::number(point[2]);
      menu->addAction(coordinate);

      // calculate the point to point accumulated distance for fiducials
      if (numPoints == 1 && rows.size() > 1)
        {
        if (i > 0)
          {
          double distanceToLastPoint = vtkMath::Distance2BetweenPoints(lastPoint, point);
          if (distanceToLastPoint != 0.0)
            {
            distanceToLastPoint = sqrt(distanceToLastPoint);
            }
          distance += distanceToLastPoint;
          }
        lastPoint[0] = point[0];
        lastPoint[1] = point[1];
        lastPoint[2] = point[2];
        }
      }
    }
  if (distance != 0.0)
    {
    menu->addAction(QString("Summed linear distance: %1").arg(distance));
    }
  menu->addSeparator();
  */
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onJumpSlicesActionTriggered()
{
 Q_D(qSRPlanPathPlanModuleWidget);

  // get the selected rows
  QList<QTableWidgetItem *> selectedItems = d->activeMarkupTableWidget->selectedItems();

  // first, check if nothing is selected
  if (selectedItems.isEmpty())
    {
    return;
    }

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }


  /*
  // offset or center?
  bool jumpCentered = false;
  if (d->jumpCenteredRadioButton->isChecked())
    {
    jumpCentered = true;
    }

  // jump to it
  if (this->markupsLogic())
    {
    // use the first selected
    this->markupsLogic()->JumpSlicesToNthPointInMarkup(mrmlNode->GetID(), selectedItems.at(0)->row(), jumpCentered);
    }
	*/
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onRefocusCamerasActionTriggered()
{
 Q_D(qSRPlanPathPlanModuleWidget);

  // get the selected rows
  QList<QTableWidgetItem *> selectedItems = d->activeMarkupTableWidget->selectedItems();

  // first, check if nothing is selected
  if (selectedItems.isEmpty())
    {
    return;
    }

  // get the active node
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }

  // refocus on this point
  if (this->markupsLogic())
    {
    // use the first selected
    this->markupsLogic()->FocusCamerasOnNthPointInMarkup(mrmlNode->GetID(), selectedItems.at(0)->row());
    }
}

//-----------------------------------------------------------------------------
QStringList qSRPlanPathPlanModuleWidget::getOtherMarkupNames(vtkMRMLNode *thisMarkup)
{
  QStringList otherMarkups;

  // check for other markups nodes in the scene
  if (!this->mrmlScene())
    {
    return otherMarkups;
    }

  vtkCollection *col = this->mrmlScene()->GetNodesByClass(thisMarkup->GetClassName());
  unsigned int numNodes = col->GetNumberOfItems();
  if (numNodes < 2)
    {
    col->RemoveAllItems();
    col->Delete();
    return otherMarkups;
    }

  for (unsigned int n = 0; n < numNodes; n++)
    {
    vtkMRMLNode *listNodeN = vtkMRMLNode::SafeDownCast(col->GetItemAsObject(n));
    if (strcmp(listNodeN->GetID(), thisMarkup->GetID()) != 0)
      {
      otherMarkups.append(QString(listNodeN->GetName()));
      }
    }
  col->RemoveAllItems();
  col->Delete();

  return otherMarkups;
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onCopyToOtherListActionTriggered()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  if (this->mrmlScene() == 0)
    {
    return;
    }

  // qDebug() << "onCopyToOtherListActionTriggered: " << destinationPosition;

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    return;
    }

  QStringList otherLists = this->getOtherMarkupNames(mrmlNode);

  // sanity check: is there another list to copy to?
  if (otherLists.size() == 0)
    {
    qWarning() << "No other list to copy it to! Define another list first.";
    return;
    }

  // make a dialog with the other lists to select
  QInputDialog listDialog;
  listDialog.setWindowTitle("Pick destination list");
  listDialog.setLabelText("Destination list:");
  listDialog.setComboBoxItems(otherLists);
  listDialog.setInputMode(QInputDialog::TextInput);
  QObject::connect(&listDialog, SIGNAL(textValueSelected(const QString &)),
                   this,SLOT(copySelectedToNamedList(const QString &)));
  listDialog.exec();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::copySelectedToNamedList(QString listName)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // qDebug() << "copySelectedToNamedList: " << listName;

  QString destinationPosition = QString("Same");
  if (sender() != 0)
    {
    destinationPosition = sender()->objectName();
    }
  // qDebug() << "\tdestinationPosition: " << destinationPosition;

  // get the selected point
  QList<QTableWidgetItem *> selectedItems = d->activeMarkupTableWidget->selectedItems();
  int rowNumber = selectedItems.at(0)->row();
  if (selectedItems.size() / d->numberOfColumns() > 1)
    {
    QMessageBox msgBox;
    msgBox.setText(QString("Copy is only implemented for one row."));
    msgBox.setInformativeText(QString("Click Ok to copy single markup from row ") + QString::number(rowNumber));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
      {
      // bail out
      return;
      }
    }

  if (!this->markupsLogic())
    {
    qWarning() << "No markups logic class, unable to copy markup";
    return;
    }

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    return;
    }
  // get the new list
  vtkMRMLNode *newNode = this->mrmlScene()->GetFirstNodeByName(listName.toLatin1());
  if (!newNode)
    {
    qWarning() << "Unable to find list named " << listName << " in scene!";
    return;
    }
  vtkMRMLMarkupsNode *newMarkupsNode = vtkMRMLMarkupsNode::SafeDownCast(newNode);

  // and copy
  bool retval = false;
  if (this->markupsLogic())
    {
    retval = this->markupsLogic()->CopyNthMarkupToNewList(rowNumber, markupsNode, newMarkupsNode);
    }
  if (!retval)
    {
    qWarning() << "Failed to copy " << rowNumber << " markup to list named " << listName;
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onMoveToOtherListActionTriggered()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  if (this->mrmlScene() == 0)
    {
    return;
    }

  // qDebug() << "onMoveToOtherListActionTriggered";

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    return;
    }

  QStringList otherLists = this->getOtherMarkupNames(mrmlNode);

  // sanity check: is there another list to copy to?
  if (otherLists.size() == 0)
    {
    qWarning() << "No other list to move it to! Define another list first.";
    return;
    }

  // make a dialog with the other lists to select
  QInputDialog listDialog;
  listDialog.setWindowTitle("Pick destination list");
  listDialog.setLabelText("Destination list:");
  listDialog.setComboBoxItems(otherLists);
  listDialog.setInputMode(QInputDialog::TextInput);
  QObject::connect(&listDialog, SIGNAL(textValueSelected(const QString &)),
                   this,SLOT(moveSelectedToNamedList(const QString &)));
  listDialog.exec();
}

//-----------------------------------------------------------------------------
  void qSRPlanPathPlanModuleWidget::moveSelectedToNamedList(QString listName)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // qDebug() << "moveSelectedToNamedList: " << listName;

  // get the selected point
  QList<QTableWidgetItem *> selectedItems = d->activeMarkupTableWidget->selectedItems();
  int rowNumber = selectedItems.at(0)->row();
  if (selectedItems.size() / d->numberOfColumns() > 1)
    {
    QMessageBox msgBox;
    msgBox.setText(QString("Move is only implemented for one row."));
    msgBox.setInformativeText(QString("Click Ok to move single markup from row ") + QString::number(rowNumber));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
      {
      // bail out
      return;
      }
    }

  if (!this->markupsLogic())
    {
    qWarning() << "No markups logic class, unable to move markup";
    return;
    }

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    return;
    }
  // get the new list
  vtkMRMLNode *newNode = this->mrmlScene()->GetFirstNodeByName(listName.toLatin1());
  if (!newNode)
    {
    qWarning() << "Unable to find list named " << listName << " in scene!";
    return;
    }
  vtkMRMLMarkupsNode *newMarkupsNode = vtkMRMLMarkupsNode::SafeDownCast(newNode);

  // calculate the index based on the destination position
  int newIndex = newMarkupsNode->GetNumberOfMarkups();

  // and move
  bool retval = false;
  if (this->markupsLogic())
    {
    retval = this->markupsLogic()->MoveNthMarkupToNewListAtIndex(rowNumber, markupsNode, newMarkupsNode, newIndex);
    }
  if (!retval)
    {
    qWarning() << "Failed to move " << rowNumber << " markup to list named " << listName;
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::observeMarkupsNode(vtkMRMLNode *markupsNode)
{
  if (this->mrmlScene())
    {
    // remove all connections
    vtkCollection *col = this->mrmlScene()->GetNodesByClass("vtkMRMLMarkupsNode");
    unsigned int numNodes = col->GetNumberOfItems();
    // qDebug() << "observeMarkupsNode: have " << numNodes << " markups nodes";
    for (unsigned int n = 0; n < numNodes; n++)
      {
      vtkMRMLNode *node = vtkMRMLNode::SafeDownCast(col->GetItemAsObject(n));
      if (node)
        {
        if (markupsNode)
          {
          // is this the markups node?
          if (node->GetID() && markupsNode->GetID() && strcmp(node->GetID(), markupsNode->GetID()) == 0)
            {
            // don't disconnect
            // qDebug() << "\tskipping disconnecting " << node->GetID();
            continue;
            }
          }
        // qDebug() << "\tdisconnecting " << node->GetID();
        this->qvtkDisconnect(node, vtkMRMLMarkupsNode::LockModifiedEvent,
                             this, SLOT(onActiveMarkupsNodeLockModifiedEvent()));
        this->qvtkDisconnect(node, vtkMRMLMarkupsNode::LabelFormatModifiedEvent,
                             this, SLOT(onActiveMarkupsNodeLabelFormatModifiedEvent()));
        this->qvtkDisconnect(node, vtkMRMLMarkupsNode::PointModifiedEvent,
                             this, SLOT(onActiveMarkupsNodePointModifiedEvent(vtkObject*,vtkObject*)));
        this->qvtkDisconnect(node, vtkMRMLMarkupsNode::NthMarkupModifiedEvent,
                             this, SLOT(onActiveMarkupsNodeNthMarkupModifiedEvent(vtkObject*,vtkObject*)));
        this->qvtkDisconnect(node, vtkMRMLMarkupsNode::MarkupAddedEvent,
                             this, SLOT(onActiveMarkupsNodeMarkupAddedEvent()));
        this->qvtkDisconnect(node, vtkMRMLMarkupsNode::MarkupRemovedEvent,
                             this, SLOT(onActiveMarkupsNodeMarkupRemovedEvent()));

        // display node
        vtkMRMLMarkupsNode *displayableNode = vtkMRMLMarkupsNode::SafeDownCast(node);
        if (displayableNode)
          {
          this->qvtkDisconnect(displayableNode,
                               vtkMRMLDisplayableNode::DisplayModifiedEvent,
                               this,
                               SLOT(onActiveMarkupsNodeDisplayModifiedEvent()));
          }
        // transforms
        vtkMRMLTransformableNode *transformableNode =
          vtkMRMLTransformableNode::SafeDownCast(node);
        if (transformableNode)
          {
          this->qvtkDisconnect(transformableNode,
                               vtkMRMLTransformableNode::TransformModifiedEvent,
                               this,
                               SLOT(onActiveMarkupsNodeTransformModifiedEvent()));
          }
        }
      }
    col->RemoveAllItems();
    col->Delete();
    }
  else
    {
    qWarning() << "observeMarkupsNode: no scene";
    }
  if (markupsNode)
    {
    // is the node already connected?
    if (this->qvtkIsConnected(markupsNode, vtkMRMLMarkupsNode::LockModifiedEvent,
                                     this, SLOT(onActiveMarkupsNodeLockModifiedEvent())))
      {
      // qDebug() << "\tmarkups node is already connected: " << markupsNode->GetID();
      }
    else
      {
      // add connections for this node
      this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::LockModifiedEvent,
                        this, SLOT(onActiveMarkupsNodeLockModifiedEvent()));
      this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::LabelFormatModifiedEvent,
                        this, SLOT(onActiveMarkupsNodeLabelFormatModifiedEvent()));
      this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::PointModifiedEvent,
                        this, SLOT(onActiveMarkupsNodePointModifiedEvent(vtkObject*,vtkObject*)));
      this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::NthMarkupModifiedEvent,
                        this, SLOT(onActiveMarkupsNodeNthMarkupModifiedEvent(vtkObject*,vtkObject*)));
      this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::MarkupAddedEvent,
                        this, SLOT(onActiveMarkupsNodeMarkupAddedEvent()));
      this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::MarkupRemovedEvent,
                        this, SLOT(onActiveMarkupsNodeMarkupRemovedEvent()));
      // qDebug() << "\tconnected markups node " << markupsNode->GetID();
      // display node
      vtkMRMLMarkupsNode *displayableNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNode);
      if (displayableNode)
        {
        this->qvtkConnect(displayableNode,
                          vtkMRMLDisplayableNode::DisplayModifiedEvent,
                          this,
                          SLOT(onActiveMarkupsNodeDisplayModifiedEvent()));
        }
      // transforms
      vtkMRMLTransformableNode *transformableNode =
        vtkMRMLTransformableNode::SafeDownCast(markupsNode);
      if (transformableNode)
        {
        this->qvtkConnect(transformableNode,
                          vtkMRMLTransformableNode::TransformModifiedEvent,
                          this,
                          SLOT(onActiveMarkupsNodeTransformModifiedEvent()));
        }
      }
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::clearGUI()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  d->activeMarkupTableWidget->clearContents();
  d->activeMarkupTableWidget->setRowCount(0);

  // setting a null node requires casting (and triggers a memory leak),
  // so disable it instead
//  d->listDisplayNodeViewComboBox->setEnabled(false);
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeLockModifiedEvent()
{
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    return;
    }
  if (markupsNode->GetLocked())
    {
    // disable the table
    d->activeMarkupTableWidget->setEnabled(false);
    }
  else
    {
    // enable it
    d->activeMarkupTableWidget->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeLabelFormatModifiedEvent()
{
	/*
  Q_D(qSRPlanPathPlanModuleWidget);

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    return;
    }
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (markupsNode)
    {
    d->nameFormatLineEdit->setText(markupsNode->GetMarkupLabelFormat().c_str());
    }
	*/
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeNthMarkupModifiedEvent(vtkObject *caller, vtkObject *callData)
{
  //qDebug() << "onActiveMarkupsNodeNthMarkupModifiedEvent\n";

  // the call data should be the index n
  if (caller == NULL || callData == NULL)
    {
    return;
    }

  int *nPtr = NULL;
  int n = -1;
  nPtr = reinterpret_cast<int *>(callData);
  if (nPtr)
    {
    n = *nPtr;
    }
  this->updateRow(n);



}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodePointModifiedEvent(vtkObject *caller, vtkObject *callData)
{
  //qDebug() << "onActiveMarkupsNodePointModifiedEvent";

  // the call data should be the index n
  if (caller == NULL || callData == NULL)
    {
    return;
    }
  // qDebug() << "\tcaller class = " << caller->GetClassName();
  int *nPtr = NULL;
  int n = -1;
  nPtr = reinterpret_cast<int *>(callData);
  if (nPtr)
    {
    n = *nPtr;
    }
  // qDebug() << "\tn = " << QString::number(n);
  this->updateRow(n);

  //If There has dosevolume, invalidate the dosevolume added by zoulian

  this->getBDoseCalculateLogic()->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
  emit DoseInvalided();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeMarkupAddedEvent()//vtkMRMLNode *markupsNode)
{
  Q_D(qSRPlanPathPlanModuleWidget);

  //qDebug() << "onActiveMarkupsNodeMarkupAddedEvent";
  

  QString activeMarkupsNodeID = d->activeMarkupMRMLNodeComboBox->currentNodeID();

  //qDebug() << QString("active markups node id from combo box = ") + activeMarkupsNodeID;

  int newRow = d->activeMarkupTableWidget->rowCount();
  //qDebug() << QString("\tnew row / row count = ") + QString::number(newRow);
  d->activeMarkupTableWidget->insertRow(newRow);
  //qDebug() << QString("\t after inserting a row, row count = ") + QString::number(d->activeMarkupTableWidget->rowCount());

  this->updateRow(newRow);

  // scroll to the new row only if jump slices is not selected
  // (if jump slices on click in table is selected, selecting the new
  // row before the point coordinates are updated will cause the slices
  // to jump to 0,0,0)

 // if (!d->jumpSlicesGroupBox->isChecked())
 //   {
 //   d->activeMarkupTableWidget->setCurrentCell(newRow, 0);
 //   }

	
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeMarkupRemovedEvent()//vtkMRMLNode *markupsNode)
{
  // do a general update
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeDisplayModifiedEvent()
{
  // update the display properties
  this->updateWidgetFromDisplayNode();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onActiveMarkupsNodeTransformModifiedEvent()
{
  // update the transform check box label
  // update the coordinates in the table
  this->updateWidgetFromMRML();
}


//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onNewMarkupWithCurrentDisplayPropertiesTriggered()
{
  Q_D(qSRPlanPathPlanModuleWidget);

//  qDebug() << "onNewMarkupWithCurrentDisplayPropertiesTriggered";

  // get the active list
  vtkMRMLNode *mrmlNode = d->activeMarkupMRMLNodeComboBox->currentNode();
  if (!mrmlNode)
    {
    // if there's no currently active markups list, trigger the default add
    // node
    d->activeMarkupMRMLNodeComboBox->addNode();
    return;
    }

  // otherwise make a new one of the same class
  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
  if (!markupsNode)
    {
    qDebug() << "Unable to get the currently active markups node";
    return;
    }

  // get the display node
  vtkMRMLDisplayNode *displayNode = markupsNode->GetDisplayNode();
  if (!displayNode)
    {
    qDebug() << "Unable to get the display node on the markups node";
    }

  // create a new one
  vtkMRMLNode *newDisplayNode = this->mrmlScene()->CreateNodeByClass(displayNode->GetClassName());
  // copy the old one
  newDisplayNode->Copy(displayNode);
  /// add to the scene
  this->mrmlScene()->AddNode(newDisplayNode);

  // now create the new markups node
  const char *className = markupsNode->GetClassName();
  vtkMRMLNode *newMRMLNode = this->mrmlScene()->CreateNodeByClass(className);
  // copy the name and let them rename it
  newMRMLNode->SetName(markupsNode->GetName());
  this->mrmlScene()->AddNode(newMRMLNode);
  // and observe the copied display node
  vtkMRMLDisplayableNode *newDisplayableNode = vtkMRMLDisplayableNode::SafeDownCast(newMRMLNode);
  newDisplayableNode->SetAndObserveDisplayNodeID(newDisplayNode->GetID());

  // set it active
  d->activeMarkupMRMLNodeComboBox->setCurrentNodeID(newMRMLNode->GetID());
  // let the user rename it
  d->activeMarkupMRMLNodeComboBox->renameCurrentNode();

  // update the display properties manually since the display node wasn't
  // observed when it was added
  this->updateWidgetFromMRML();

  // clean up
  newDisplayNode->Delete();
  newMRMLNode->Delete();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateLogicFromSettings()
{
  // update logic from settings
  if (this->logic() == NULL)
    {
    qWarning() << "updateLogicFromSettings: no logic to set";
    return;
    }

  if (!qSlicerApplication::application() ||
      !qSlicerApplication::application()->settingsDialog())
    {
    qWarning() << "updateLogicFromSettings: null application or settings dialog";
    return;
    }

  QSettings *settings = qSlicerApplication::application()->settingsDialog()->settings();
  if (!settings)
    {
    qWarning() << "updateLogicFromSettings: null settings";
    return;
    }

  // have settings been saved before?
  if (!settings->contains("Markups/GlyphType") ||
      !settings->contains("Markups/SelectedColor") ||
      !settings->contains("Markups/UnselectedColor") ||
      !settings->contains("Markups/GlyphScale") ||
      !settings->contains("Markups/TextScale") ||
      !settings->contains("Markups/Opacity") ||
      !settings->contains("Markups/SliceProjection") ||
      !settings->contains("Markups/SliceProjectionColor") ||
      !settings->contains("Markups/SliceProjectionOpacity"))
    {
    // display settings not saved yet, use defaults
    return;
    }

  QString glyphType = settings->value("Markups/GlyphType").toString();
  QColor qcolor;
  QVariant variant = settings->value("Markups/SelectedColor");
  qcolor = variant.value<QColor>();
  double selectedColor[3];
  qMRMLUtils::qColorToColor(qcolor, selectedColor);
  variant = settings->value("Markups/UnselectedColor");
  QColor qcolorUnsel = variant.value<QColor>();
  double unselectedColor[3];
  qMRMLUtils::qColorToColor(qcolorUnsel, unselectedColor);
  double glyphScale = settings->value("Markups/GlyphScale").toDouble();
  double textScale = settings->value("Markups/TextScale").toDouble();
  double opacity = settings->value("Markups/Opacity").toDouble();

  int sliceProjection = settings->value("Markups/SliceProjection").toInt();
  variant = settings->value("Markups/SliceProjectionColor");
  QColor qcolorProjection = variant.value<QColor>();
  double projectionColor[3];
  qMRMLUtils::qColorToColor(qcolorProjection, projectionColor);
  double projectionOpacity = settings->value("Markups/SliceProjectionOpacity").toDouble();

  if (!this->markupsLogic())
    {
    qWarning() << "Unable to get markups logic";
    return;
    }
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeGlyphTypeFromString(glyphType.toLatin1());
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeGlyphScale(glyphScale);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeTextScale(textScale);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeSelectedColor(selectedColor);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeColor(unselectedColor);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeOpacity(opacity);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeSliceProjection(sliceProjection);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeSliceProjectionColor(projectionColor);
  this->markupsLogic()->SetDefaultMarkupsDisplayNodeSliceProjectionOpacity(projectionOpacity);
}

//-----------------------------------------------------------------------------
bool qSRPlanPathPlanModuleWidget::sliceIntersectionsVisible()
{
  if (!this->markupsLogic())
    {
    qWarning() << "Unable to get markups logic";
    return false;
    }
  int flag = this->markupsLogic()->GetSliceIntersectionsVisibility();
  if (flag == 0 || flag == -1)
    {
    return false;
    }
  else
    {
    // if all or some are visible, return true
    return true;
    }
}







/*


// given the Flash Dose Distribution Thresholder
void qSRPlanPathPlanModuleWidget::updateDoseGridFlashShowLowerThresholder(int low)
{
	//Get the Final Dose distribution for ISO Dose Node 
	vtkMRMLScalarVolumeNode* DoseDistribution = this->getBDoseCalculateLogic()->GetCalculatedDoseVolume();

	if (DoseDistribution)
	{
		 vtkMRMLScalarVolumeDisplayNode * volumeDisplayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(DoseDistribution->GetDisplayNode());

		 if (volumeDisplayNode)
		 {
			 volumeDisplayNode->SetLowerThreshold(low);
		 }
	}

}
*/


vtkSRPlanBDoseCalculateLogic* qSRPlanPathPlanModuleWidget::getBDoseCalculateLogic()
{
	return vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic())->GetBDoseCalculateLogic();
}


vtkSlicerMarkupsLogic*  qSRPlanPathPlanModuleWidget::getMarkupsLogic()
{
	return vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic())->GetMarkupsLogic();
}


vtkSlicerIsodoseLogic * qSRPlanPathPlanModuleWidget::getIsodoseLogic()
{
	return vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic())->GetISODoseLogic();
}



vtkSlicerDoseVolumeHistogramLogic * qSRPlanPathPlanModuleWidget::getDVHLogic()
{
	return vtkSRPlanPathPlanModuleLogic::SafeDownCast(this->logic())->GetDVHLogic();
	
}





//****************************************************************************************************
//ISO Dose evalution block
//
//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::setNumberOfLevels(int newNumber)
{
	Q_D(qSRPlanPathPlanModuleWidget);
	if (!d->spinBox_NumberOfLevels->isEnabled() || !this->getIsodoseLogic()->GetIsodoseNode())
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setNumberOfLevels: Invalid parameter set node!";
		return;
	}
	this->getIsodoseLogic()->SetNumberOfIsodoseLevels(newNumber);

	

	vtkMRMLColorTableNode* selectedColorNode = this->getIsodoseLogic()->GetIsodoseNode()->GetColorTableNode();

	if (!selectedColorNode)
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setNumberOfLevels: Invalid color table node!";
		return;
	}

	int numberOfColors = selectedColorNode->GetNumberOfColors();
	this->ScalarBarActor->SetMaximumNumberOfColors(numberOfColors);
	this->ScalarBarActor->SetNumberOfLabels(numberOfColors);

	this->ScalarBarActor2DRed->SetMaximumNumberOfColors(numberOfColors);
	this->ScalarBarActor2DRed->SetNumberOfLabels(numberOfColors);
	this->ScalarBarActor2DYellow->SetMaximumNumberOfColors(numberOfColors);
	this->ScalarBarActor2DYellow->SetNumberOfLabels(numberOfColors);
	this->ScalarBarActor2DGreen->SetMaximumNumberOfColors(numberOfColors);
	this->ScalarBarActor2DGreen->SetNumberOfLabels(numberOfColors);

	
	//Update the Dose ScalarVolume color show, change the DisplayVolumeNode. added by zoulian

	vtkMRMLScalarVolumeNode* doseVolume = this->getIsodoseLogic()->GetIsodoseNode()->GetDoseVolumeNode();

	this->getIsodoseLogic()->UpdateDoseVolumeDisplayNode(doseVolume, selectedColorNode);


}




//------------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::setIsolineVisibility(bool visible)
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!this->mrmlScene())
	{
		qCritical() << "qSlicerIsodoseModuleWidget::setIsolineVisibility: Invalid scene!";
		return;
	}

	vtkMRMLIsodoseNode* paramNode = this->getIsodoseLogic()->GetIsodoseNode();
	if (!paramNode)
	{
		return;
	}

	paramNode->DisableModifiedEventOn();
	paramNode->SetShowIsodoseLines(visible);
	paramNode->DisableModifiedEventOff();

	vtkMRMLModelHierarchyNode* modelHierarchyNode = this->getIsodoseLogic()->GetRootModelHierarchyNode();
	if (!modelHierarchyNode)
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setIsolineVisibility: Invalid isodose surface models parent hierarchy node!";
		return;
	}

	vtkSmartPointer<vtkCollection> childModelNodes = vtkSmartPointer<vtkCollection>::New();
	modelHierarchyNode->GetChildrenModelNodes(childModelNodes);
	childModelNodes->InitTraversal();
	for (int i = 0; i<childModelNodes->GetNumberOfItems(); ++i)
	{
		vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(childModelNodes->GetItemAsObject(i));
		modelNode->GetDisplayNode()->SetSliceIntersectionVisibility(visible);
	}
}

//------------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::setIsosurfaceVisibility(bool visible)
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!this->mrmlScene())
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setIsosurfaceVisibility: Invalid scene!";
		return;
	}

	vtkMRMLIsodoseNode* paramNode = this->getIsodoseLogic()->GetIsodoseNode();
	if (!paramNode)
	{
		return;
	}

	paramNode->DisableModifiedEventOn();
	paramNode->SetShowIsodoseSurfaces(visible);
	paramNode->DisableModifiedEventOff();

	vtkMRMLModelHierarchyNode* modelHierarchyNode = this->getIsodoseLogic()->GetRootModelHierarchyNode();
	if (!modelHierarchyNode)
	{
		return;
	}

	vtkSmartPointer<vtkCollection> childModelNodes = vtkSmartPointer<vtkCollection>::New();
	modelHierarchyNode->GetChildrenModelNodes(childModelNodes);
	childModelNodes->InitTraversal();
	for (int i = 0; i<childModelNodes->GetNumberOfItems(); ++i)
	{
		vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(childModelNodes->GetItemAsObject(i));
		modelNode->GetDisplayNode()->SetVisibility(visible);
	}
}

//------------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::setScalarBarVisibility(bool visible)
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!this->mrmlScene())
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setScalarBarVisibility: Invalid scene!";
		return;
	}

	if (this->ScalarBarWidget == 0)
	{
		return;
	}
	if (visible)
	{
		this->ScalarBarActor->UseAnnotationAsLabelOn();
	}
	vtkMRMLColorTableNode* selectedColorNode = this->getIsodoseLogic()->GetIsodoseNode()->GetColorTableNode();
	if (!selectedColorNode)
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setScalarBarVisibility: Invalid color table node!";
		return;
	}
	int numberOfColors = selectedColorNode->GetNumberOfColors();
	for (int i = 0; i<numberOfColors; i++)
	{
		this->ScalarBarActor->GetLookupTable()->SetAnnotation(i, vtkStdString(selectedColorNode->GetColorName(i)));
	}

	this->ScalarBarWidget->SetEnabled(visible);
}

//------------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::setScalarBar2DVisibility(bool visible)
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!this->mrmlScene())
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setScalarBar2DVisibility: Invalid scene!";
		return;
	}

	if (this->ScalarBarWidget2DRed == 0 || this->ScalarBarWidget2DYellow == 0 || this->ScalarBarWidget2DGreen == 0)
	{
		return;
	}
	if (visible)
	{
		this->ScalarBarActor2DRed->UseAnnotationAsLabelOn();
		this->ScalarBarActor2DYellow->UseAnnotationAsLabelOn();
		this->ScalarBarActor2DGreen->UseAnnotationAsLabelOn();
	}
	vtkMRMLColorTableNode* selectedColorNode = this->getIsodoseLogic()->GetIsodoseNode()->GetColorTableNode();
	if (!selectedColorNode)
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::setScalarBar2DVisibility: Invalid color table node!";
		return;
	}
	int numberOfColors = selectedColorNode->GetNumberOfColors();
	for (int i = 0; i<numberOfColors; i++)
	{
		this->ScalarBarActor2DRed->GetLookupTable()->SetAnnotation(i, vtkStdString(selectedColorNode->GetColorName(i)));
		this->ScalarBarActor2DYellow->GetLookupTable()->SetAnnotation(i, vtkStdString(selectedColorNode->GetColorName(i)));
		this->ScalarBarActor2DGreen->GetLookupTable()->SetAnnotation(i, vtkStdString(selectedColorNode->GetColorName(i)));
	}

	this->ScalarBarWidget2DRed->SetEnabled(visible);
	this->ScalarBarWidget2DYellow->SetEnabled(visible);
	this->ScalarBarWidget2DGreen->SetEnabled(visible);
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::applyClicked()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!this->mrmlScene())
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::applyClicked: Invalid scene!";
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	// Compute the isodose surface for the selected dose volume
	//this->getIsodoseLogic()->CreateIsodoseSurfaces();



	//Update the Dose ScalarVolume color show, change the DisplayVolumeNode. added by zoulian

	vtkMRMLScalarVolumeNode* doseVolume = this->getIsodoseLogic()->GetIsodoseNode()->GetDoseVolumeNode();
	vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(doseVolume->GetDisplayNode());
	displayNode->Modified();

	/*
	vtkMRMLColorTableNode* selectedColorNode = this->getIsodoseLogic()->GetIsodoseNode()->GetColorTableNode();
	vtkMRMLScalarVolumeNode* doseVolume = this->getIsodoseLogic()->GetIsodoseNode()->GetDoseVolumeNode();

	this->getIsodoseLogic()->UpdateDoseVolumeDisplayNode(doseVolume, selectedColorNode);

	*/


	QApplication::restoreOverrideCursor();
}



void qSRPlanPathPlanModuleWidget::updateButtonsState()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	bool applyEnabled = this->getIsodoseLogic()->GetIsodoseNode()
		&& this->getIsodoseLogic()->GetIsodoseNode()->GetDoseVolumeNode()
		&& this->getIsodoseLogic()->GetIsodoseNode()->GetColorTableNode()
		&& this->getIsodoseLogic()->GetIsodoseNode()->GetColorTableNode()->GetNumberOfColors() > 0;

	d->pushButton_Apply->setEnabled(applyEnabled);

	d->pushButton_SwitchToTableFourUpQuantitativeLayout->setEnabled(applyEnabled);
 
 
		
	d->pushButton_SwitchToFourUpQuantitativeLayout->setEnabled( applyEnabled && validDVH);
		
	d->pushButton_SwitchToOneUpQuantitativeLayout->setEnabled( applyEnabled && validDVH);

 
	// SET BOOL index for other purpose.
	this->showDoseEvalulationWhenEnter = applyEnabled;

    
}



//End ISO Dose evalution block
//****************************************************************************************************




//****************************************************************************************************
//DVH evalution block
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void qSRPlanPathPlanModuleWidget::switchToToTableFourUpQuantitativeLayout()
{
	qSlicerApplication::application()->layoutManager()->setLayout(vtkMRMLLayoutNode::SlicerLayoutTableFourUpQuantitativeView);


	if (!validDVH && this->ActiveDoseDistribution && this->ActivesegmentationNode)
	{
		this->enterDVHDoseEvalutaionFunction(this->ActiveDoseDistribution, this->ActivesegmentationNode);
		//validDVH = true;
	}

}


void qSRPlanPathPlanModuleWidget::switchToFourUpQuantitativeLayout()
{
	qSlicerApplication::application()->layoutManager()->setLayout(vtkMRMLLayoutNode::SlicerLayoutFourUpQuantitativeView);
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::switchToOneUpQuantitativeLayout()
{
	qSlicerApplication::application()->layoutManager()->setLayout(vtkMRMLLayoutNode::SlicerLayoutOneUpQuantitativeView);
}


//End DVH evalution block
//****************************************************************************************************


void qSRPlanPathPlanModuleWidget::enterDVHDoseEvalutaionFunction(vtkMRMLScalarVolumeNode* DoseDistribution, vtkMRMLSegmentationNode*segmentationNode)
{
	vtkMRMLScene * scene = this->mrmlScene();


	//Step 1, Get the DVHLogic's DVH ParameterNode, if Null create it, added 
	vtkMRMLDoseVolumeHistogramNode * DVHNode = 	this->getDVHLogic()->GetDoseVolumeHistogramNode();

	if (DoseDistribution && segmentationNode)
	{
		if (DVHNode == NULL)
		{
			vtkMRMLChartNode* chartNode = vtkMRMLChartNode::New();
			chartNode->SetSaveWithScene(false);

			scene->AddNode(chartNode);
				
			DVHNode = vtkMRMLDoseVolumeHistogramNode::New();

			DVHNode->DisableModifiedEventOn();
			DVHNode->SetAndObserveChartNode(chartNode);
			DVHNode->DisableModifiedEventOff();

			DVHNode->SetSaveWithScene(false);
			scene->AddNode(DVHNode);
		}

		DVHNode->DisableModifiedEventOn();

		DVHNode->SetAndObserveDoseVolumeNode(DoseDistribution);
		DVHNode->SetAndObserveSegmentationNode(segmentationNode);

		//std::vector<std::string> selectedSegmentIDs;
		//segmentationNode->GetSegmentation()->GetSegmentIDs(selectedSegmentIDs);
		
		//DVHNode->SetSelectedSegmentIDs(selectedSegmentIDs);

		DVHNode->DisableModifiedEventOff();
	}



	// Each time the node is modified, the qt widgets are updated

	qvtkConnect(DVHNode, vtkCommand::ModifiedEvent, this, SLOT(updateDVHWidgetFromMRML()));

	this->getDVHLogic()->SetAndObserveDoseVolumeHistogramNode(DVHNode);

	this->computeDvh();




	this->updateDVHWidgetFromMRML();

	
	
}


void qSRPlanPathPlanModuleWidget::updateDVHWidgetFromMRML()
{
	this->refreshDvhTable();
}



//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::refreshDvhTable(bool force/*=false*/)
{
	Q_D(qSRPlanPathPlanModuleWidget);

	
	vtkMRMLDoseVolumeHistogramNode* paramNode = this->getDVHLogic()->GetDoseVolumeHistogramNode();
	if (!paramNode || !this->mrmlScene())
	{
	//	return;
	}

	// Get DVH double array nodes for parameter set node
	std::vector<vtkMRMLNode*> dvhNodes;
	paramNode->GetDvhDoubleArrayNodes(dvhNodes);

	// If number of nodes is the same in the table and the list of nodes, then we don't need refreshing the table
	// (this function is called after each node event, so it cannot occur that a node has been removed and another added)
	if (!force && d->PlotCheckboxToStructureNameMap.size() == static_cast<int>(dvhNodes.size()))
	{
	//	return;
	}

	int ntable= qSlicerApplication::application()->layoutManager()->tableViewCount();


	qMRMLSimpleTableWidget * simpleTableWidget = qSlicerApplication::application()->layoutManager()->simpleTableWidget(0);

	// Clear the table
	QTableWidget * tableWidget_ChartStatistics = simpleTableWidget->getTableWidget();

	tableWidget_ChartStatistics->setRowCount(0);
	tableWidget_ChartStatistics->setColumnCount(0);
	tableWidget_ChartStatistics->clearContents();

	// Clear checkbox to segmentation name map
	QMapIterator<QCheckBox*, QString> it(d->PlotCheckboxToStructureNameMap);
	while (it.hasNext())
	{
		it.next();

		QCheckBox* checkbox = it.key();
		disconnect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(showInChartCheckStateChanged(int)));
		delete checkbox;
	}

	d->PlotCheckboxToStructureNameMap.clear();

	// We won't have table items if the list of nodes is empty
	if (dvhNodes.size() < 1)
	{
	//	return;
	}

	// Collect metrics for found DVH nodes from their attributes
	std::vector<std::string> metricList;
	this->getDVHLogic()->CollectMetricsForDvhNodes(dvhNodes, metricList);

	/*
	// Get requested V metrics
	std::vector<double> vDoseValues;
	if (d->checkBox_ShowVMetricsCc->isChecked() || d->checkBox_ShowVMetricsPercent->isChecked())
	{
		this->getNumbersFromLineEdit(d->lineEdit_VDose, vDoseValues);
	}
	int vColumnCount = (d->checkBox_ShowVMetricsCc->isChecked() ? vDoseValues.size() : 0)
		+ (d->checkBox_ShowVMetricsPercent->isChecked() ? vDoseValues.size() : 0);

	// Get requested D metrics
	std::vector<double> dVolumeValuesCc;
	std::vector<double> dVolumeValuesPercent;
	if (d->checkBox_ShowDMetrics->isChecked())
	{
		this->getNumbersFromLineEdit(d->lineEdit_DVolumeCc, dVolumeValuesCc);
		this->getNumbersFromLineEdit(d->lineEdit_DVolumePercent, dVolumeValuesPercent);
	}
	*/



	// Set up the table columns
	tableWidget_ChartStatistics->setColumnCount(3 + metricList.size());


	QStringList headerLabels;
	headerLabels << "" << "Structure" << "DoseGrid name";
	for (std::vector<std::string>::iterator it = metricList.begin(); it != metricList.end(); ++it)
	{
		QString metricName(it->c_str());
		metricName = metricName.right(metricName.length()
			- vtkSlicerDoseVolumeHistogramLogic::DVH_METRIC_ATTRIBUTE_NAME_PREFIX.size());
		headerLabels << metricName;
	}

	/*
	for (std::vector<double>::iterator it = vDoseValues.begin(); it != vDoseValues.end(); ++it)
	{
		if (d->checkBox_ShowVMetricsCc->isChecked())
		{
			QString metricName = QString("V%1 (cc)").arg(*it);
			headerLabels << metricName;
			d->tableWidget_ChartStatistics->setColumnWidth(headerLabels.size() - 1, 64);
		}
		if (d->checkBox_ShowVMetricsPercent->isChecked())
		{
			QString metricName = QString("V%1 (%)").arg(*it);
			headerLabels << metricName;
			d->tableWidget_ChartStatistics->setColumnWidth(headerLabels.size() - 1, 64);
		}
	}
	for (std::vector<double>::iterator it = dVolumeValuesCc.begin(); it != dVolumeValuesCc.end(); ++it)
	{
		QString metricName = QString("D%1cc (Gy)").arg(*it);
		headerLabels << metricName;
		d->tableWidget_ChartStatistics->setColumnWidth(headerLabels.size() - 1, 64);
	}
	for (std::vector<double>::iterator it = dVolumeValuesPercent.begin(); it != dVolumeValuesPercent.end(); ++it)
	{
		QString metricName = QString("D%1% (Gy)").arg(*it);
		headerLabels << metricName;
		d->tableWidget_ChartStatistics->setColumnWidth(headerLabels.size() - 1, 64);
	}

	*/

	tableWidget_ChartStatistics->setColumnWidth(0, 24);
	tableWidget_ChartStatistics->setHorizontalHeaderLabels(headerLabels);
	tableWidget_ChartStatistics->setRowCount(dvhNodes.size());

	// Fill the table
	std::vector<vtkMRMLNode*>::iterator dvhIt;
	int dvhIndex = 0;
	for (dvhIt = dvhNodes.begin(); dvhIt != dvhNodes.end(); ++dvhIt, ++dvhIndex)
	{
		if (!(*dvhIt))
		{
			continue;
		}

		// Create checkbox
		QCheckBox* checkbox = new QCheckBox(tableWidget_ChartStatistics);
		checkbox->setToolTip(tr("Show/hide DVH plot of structure '%1' in selected chart").arg(
			QString((*dvhIt)->GetAttribute(vtkSlicerDoseVolumeHistogramLogic::DVH_STRUCTURE_NAME_ATTRIBUTE_NAME.c_str()))));
		connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(showInChartCheckStateChanged(int)));

		// Store checkbox with the double array ID
		d->PlotCheckboxToStructureNameMap[checkbox] = QString((*dvhIt)->GetID());

		tableWidget_ChartStatistics->setCellWidget(dvhIndex, 0, checkbox);

		tableWidget_ChartStatistics->setItem(dvhIndex, 1, new QTableWidgetItem(
			QString((*dvhIt)->GetAttribute(vtkSlicerDoseVolumeHistogramLogic::DVH_STRUCTURE_NAME_ATTRIBUTE_NAME.c_str()))));

		vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(
			(*dvhIt)->GetNodeReference(vtkSlicerDoseVolumeHistogramLogic::DVH_DOSE_VOLUME_NODE_REFERENCE_ROLE.c_str()));
		if (volumeNode)
		{
			tableWidget_ChartStatistics->setItem(dvhIndex, 2, new QTableWidgetItem(QString(volumeNode->GetName())));
		}

		// Add default metric values
		int col = 3;
		for (std::vector<std::string>::iterator it = metricList.begin(); it != metricList.end(); ++it)
		{
			std::vector<std::string> attributeNames = (*dvhIt)->GetAttributeNames();
			std::string foundAttributeName;
			for (std::vector<std::string>::iterator attributeIt = attributeNames.begin(); attributeIt != attributeNames.end(); ++attributeIt)
			{
				if (vtksys::SystemTools::LowerCase(*attributeIt).compare(vtksys::SystemTools::LowerCase(*it)) == 0)
				{
					foundAttributeName = *attributeIt;
				}
			}
			QString metricValue((*dvhIt)->GetAttribute(foundAttributeName.c_str()));
			if (foundAttributeName.empty() || metricValue.isEmpty())
			{
				++col;
				continue;
			}

			tableWidget_ChartStatistics->setItem(dvhIndex, col, new QTableWidgetItem(metricValue));
			++col;
		}

		/*
		// Add V metric values
		if (vDoseValues.size() > 0)
		{
			std::vector<double> volumes;
			std::vector<double> percents;
			d->logic()->ComputeVMetrics(vtkMRMLDoubleArrayNode::SafeDownCast(*dvhIt), vDoseValues, volumes, percents);
			if (volumes.size() != percents.size())
			{
				std::cerr << "Error: V metric result mismatch!" << std::endl;
				continue;
			}
			col = 3 + metricList.size();

			for (unsigned int volumeIndex = 0; volumeIndex<volumes.size(); ++volumeIndex)
			{
				if (d->checkBox_ShowVMetricsCc->isChecked())
				{
					QString metricValue;
					metricValue.setNum(volumes[volumeIndex], 'f', 2);
					d->tableWidget_ChartStatistics->setItem(dvhIndex, col, new QTableWidgetItem(metricValue));
					++col;
				}
				if (d->checkBox_ShowVMetricsPercent->isChecked())
				{
					QString metricValue;
					metricValue.setNum(percents[volumeIndex], 'f', 2);
					d->tableWidget_ChartStatistics->setItem(dvhIndex, col, new QTableWidgetItem(metricValue));
					++col;
				}
			}
		}

		// Add D metric values
		if (dVolumeValuesCc.size() > 0)
		{
			std::vector<double> doses;
			d->logic()->ComputeDMetrics(vtkMRMLDoubleArrayNode::SafeDownCast(*dvhIt), dVolumeValuesCc, doses, false);
			col = 3 + metricList.size() + vColumnCount;
			for (std::vector<double>::iterator it = doses.begin(); it != doses.end(); ++it)
			{
				QString metricValue;
				metricValue.setNum((*it), 'f', 2);
				d->tableWidget_ChartStatistics->setItem(dvhIndex, col, new QTableWidgetItem(metricValue));
				++col;
			}
		}
		if (dVolumeValuesPercent.size() > 0)
		{
			std::vector<double> doses;
			d->logic()->ComputeDMetrics(vtkMRMLDoubleArrayNode::SafeDownCast(*dvhIt), dVolumeValuesPercent, doses, true);
			col = 3 + metricList.size() + vColumnCount + dVolumeValuesCc.size();
			for (std::vector<double>::iterator doseIt = doses.begin(); doseIt != doses.end(); ++doseIt)
			{
				QString metricValue;
				metricValue.setNum((*doseIt), 'f', 2);
				d->tableWidget_ChartStatistics->setItem(dvhIndex, col, new QTableWidgetItem(metricValue));
				++col;
			}
		}
	*/

}

	tableWidget_ChartStatistics->resizeColumnsToContents();

	this->updateChartCheckboxesState();
	this->updateButtonsState();
}



//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::updateChartCheckboxesState()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!this->mrmlScene())
	{
		qCritical() << "qSRPlanPathPlanModuleWidget::updateChartCheckboxesState: Invalid scene!";
		return;
	}

	
	vtkMRMLDoseVolumeHistogramNode* paramNode = this->getDVHLogic()->GetDoseVolumeHistogramNode();
	if (!paramNode)
	{
		return;
	}

	vtkMRMLChartNode* chartNode = paramNode->GetChartNode();

	// If there is no chart node selected, disable all checkboxes
	if (chartNode == NULL)
	{
		QMapIterator<QCheckBox*, QString> it(d->PlotCheckboxToStructureNameMap);
		while (it.hasNext())
		{
			it.next();
			it.key()->setEnabled(false);
		}
	
		return;
	}

	// Set state of the checkboxes of the individual structures and create list of these states to store in the parameter set node
	vtkStringArray* arraysInSelectedChart = chartNode->GetArrays();
	std::vector<bool> checkStates;

	QMapIterator<QCheckBox*, QString> it(d->PlotCheckboxToStructureNameMap);
	while (it.hasNext())
	{
		it.next();

		it.key()->setEnabled(true);
		it.key()->blockSignals(true); // block signals for the checkboxes so that changing it do not toggle the visibility of the plot
		it.key()->setChecked(false);

		for (int i = 0; i<arraysInSelectedChart->GetNumberOfValues(); ++i)
		{
			// Check checkbox if the plot for the array is present in the chart
			if (arraysInSelectedChart->GetValue(i).compare(it.value().toLatin1().constData()) == 0)
			{
				it.key()->setChecked(true);
				break;
			}
		}

		// Add checkbox state to the list
		checkStates.push_back(it.key()->isChecked());

		it.key()->blockSignals(false); // unblock signal for the checkbox in question
	}

	// Set check states list to the parameter node
	paramNode->SetShowInChartCheckStates(checkStates);

	
	if (arraysInSelectedChart->GetNumberOfValues() == 0)
	{
		
		paramNode->DisableModifiedEventOn();
		paramNode->SetShowHideAll(Qt::Unchecked);
		paramNode->DisableModifiedEventOff();
	}
	else if (arraysInSelectedChart->GetNumberOfValues() == d->PlotCheckboxToStructureNameMap.size())
	{
		
		paramNode->DisableModifiedEventOn();
		paramNode->SetShowHideAll(Qt::Checked);
		paramNode->DisableModifiedEventOff();
	}
	else
	{
		
		paramNode->DisableModifiedEventOn();
		paramNode->SetShowHideAll(Qt::PartiallyChecked);
		paramNode->DisableModifiedEventOff();
	}
	
}


//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::computeDvh()
{
	Q_D(qSRPlanPathPlanModuleWidget);

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	// Initialize progress bar
	qvtkConnect(this->getDVHLogic(), SlicerRtCommon::ProgressUpdated, this, SLOT(onProgressUpdated(vtkObject*, void*, unsigned long, void*)));
	d->ConvertProgressDialog = new QProgressDialog(this);
	d->ConvertProgressDialog->setModal(true);
	d->ConvertProgressDialog->setMinimumDuration(150);
	d->ConvertProgressDialog->setLabelText("Computing DVH for all selected segments...");
	d->ConvertProgressDialog->show();
	QApplication::processEvents();

	// Compute the DVH for each selected segment set using the selected dose volume
	std::string errorMessage = this->getDVHLogic()->ComputeDvh();
	
	if (errorMessage.empty())
	{
		this->validDVH = true;
	}

	qvtkDisconnect(this->getDVHLogic(), SlicerRtCommon::ProgressUpdated, this, SLOT(onProgressUpdated(vtkObject*, void*, unsigned long, void*)));
	delete d->ConvertProgressDialog;
	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
void qSRPlanPathPlanModuleWidget::onProgressUpdated(vtkObject* vtkNotUsed(caller), void* callData, unsigned long vtkNotUsed(eid), void* vtkNotUsed(clientData))
{
	Q_D(qSRPlanPathPlanModuleWidget);

	if (!d->ConvertProgressDialog)
	{
		return;
	}

	double* progress = reinterpret_cast<double*>(callData);
	d->ConvertProgressDialog->setValue((int)((*progress)*100.0));
}


void qSRPlanPathPlanModuleWidget::onDoseInvalid()
{
	

	if (this->validDVH)
	{
		// Do some clear out jobs


		this->validDVH = false;
	}

}
