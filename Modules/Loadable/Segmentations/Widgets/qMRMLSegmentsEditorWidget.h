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

#ifndef __qMRMLSegmentsEditorWidget_h
#define __qMRMLSegmentsEditorWidget_h

// MRMLWidgets includes
#include "qMRMLWidget.h"
#include "qMRMLSegmentsEditorLogic.h"


#include "qSRPlanSegmentationsModuleWidgetsExport.h"

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

class vtkMRMLNode;
class qMRMLSegmentsEditorWidgetPrivate;
class QTableWidgetItem;
class QItemSelection;

/// \brief Qt widget for selecting a single segment from a segmentation.
///   If multiple segments are needed, then use \sa qMRMLSegmentsTableView instead in SimpleListMode
/// \ingroup SlicerRt_QtModules_Segmentations_Widgets
class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLSegmentsEditorWidget : public qMRMLWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  //Q_PROPERTY(bool noneEnabled READ noneEnabled WRITE setNoneEnabled)

public:
  /// Constructor
  explicit qMRMLSegmentsEditorWidget(QWidget* parent = 0);
  /// Destructor
  virtual ~qMRMLSegmentsEditorWidget();


   //set the segments editor logic to widget
  bool SetSegmentsEditorLogic(qMRMLSegmentsEditorLogic * editorLogic) ;
  qMRMLSegmentsEditorLogic * GetSegmentsEditorLogic();

  void EnableEffectButtons();

 // int GetBrushSize();
  void ConvertWLtoMinMax(int w, int l, int & min, int &max);

  void ConvertMinMaxtoWL(int min, int max, int & w, int &l);

public slots:
  /// Set segmentation MRML node
  //Q_INVOKABLE void setCurrentNode(vtkMRMLNode* node);
  /// Set segmentation MRML node by its ID
 // Q_INVOKABLE void setCurrentNodeID(const QString& nodeID);

  /// Get segment ID of selected segment
//  Q_INVOKABLE void setCurrentSegmentID(QString segmentID);

void onPaintBrushClicked();

//void onPolyClicked();


void onThresholdClicked();

// PaintBrush Effect Slots
//void onBrushStyleRoundClicked();
//void onBrushStyleSquareClicked();
//void onBrushSizeChanged(int size);



void onWLAbdomenSelected();
void onWLBrainSelected();
void onWLAirSelected();
void onWLBoneSelected();
void onWLLungSelected();
//void onWLCustomizedSelected();



void onWindowLevelChanged(int);


void onWLDoubleRangeSliderValueChanged(double value);


void onWLApplyPushButtonClicked();




protected slots:
  /// Handles changing of current segmentation MRML node
  //Q_INVOKABLE void onCurrentNodeChanged(vtkMRMLNode* node);
  /// Handles changing of selection in the segment combobox
  //void onCurrentSegmentChanged(int index);

  /// Populate segment combobox according to the segmentation node
//  void populateSegmentCombobox();

void  doThreshouldPreview();

signals:
//  void currentNodeChanged(vtkMRMLNode*);
//  void currentSegmentChanged(QString);

protected:
  QScopedPointer<qMRMLSegmentsEditorWidgetPrivate> d_ptr;

  qMRMLSegmentsEditorLogic * editorLogic;

private:
  Q_DECLARE_PRIVATE(qMRMLSegmentsEditorWidget);
  Q_DISABLE_COPY(qMRMLSegmentsEditorWidget);
};

#endif
