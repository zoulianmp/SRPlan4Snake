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

#ifndef __qMRMLWindowLevelPanel_h
#define __qMRMLWindowLevelPanel_h

// MRMLWidgets includes
#include "QWidget.h"
#include "QPushButton.h"

#include "qMRMLSegmentsEditorLogic.h"
#include "qSRPlanSegmentationsModuleWidgetsExport.h"


// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>


class qMRMLWindowLevelPanelPrivate;

/// \brief Qt widget for selecting a single segment from a segmentation.
///   If multiple segments are needed, then use \sa qMRMLSegmentsTableView instead in SimpleListMode
/// \ingroup SlicerRt_QtModules_Segmentations_Widgets
class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLWindowLevelPanel : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  //Q_PROPERTY(bool noneEnabled READ noneEnabled WRITE setNoneEnabled)

public:
  /// Constructor
  explicit qMRMLWindowLevelPanel(QWidget* parent = 0);
  /// Destructor
  virtual ~qMRMLWindowLevelPanel();

  QPushButton * wlApplyPushButton();

  //set the segments editor logic to widget
  bool SetSegmentsEditorLogic(qMRMLSegmentsEditorLogic * editorLogic);
  qMRMLSegmentsEditorLogic * GetSegmentsEditorLogic();


public slots:


void onWLAbdomenSelected();
void onWLBrainSelected();
void onWLAirSelected();
void onWLBoneSelected();
void onWLLungSelected();
void onWLCustomizedSelected();



void onWindowChanged(int);
void onLevelChanged(int);

void onWLApplyPushButtonClicked();

protected slots:
  

protected:
  QScopedPointer<qMRMLWindowLevelPanelPrivate> d_ptr;
  qMRMLSegmentsEditorLogic * editorLogic;
private:
  Q_DECLARE_PRIVATE(qMRMLWindowLevelPanel);
  Q_DISABLE_COPY(qMRMLWindowLevelPanel);
};

#endif
