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

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// Segmentations includes
#include "qMRMLSegmentsEditorWidget.h"

#include "ui_qMRMLSegmentsEditorWidget.h"

#include "vtkMRMLSegmentationNode.h"
#include "vtkMRMLSegmentationDisplayNode.h"
#include "vtkSegmentation.h"
#include "vtkSegment.h"

// MRML includes
#include <vtkMRMLLabelMapVolumeNode.h>

#include "qMRMLWindowLevelPanel.h"
// Qt includes
#include <QDebug>

//-----------------------------------------------------------------------------
class qMRMLSegmentsEditorWidgetPrivate: public Ui_qMRMLSegmentsEditorWidget
{
  Q_DECLARE_PUBLIC(qMRMLSegmentsEditorWidget);

protected:
  qMRMLSegmentsEditorWidget* const q_ptr;
public:
  qMRMLSegmentsEditorWidgetPrivate(qMRMLSegmentsEditorWidget& object);
  void init();

public:
  /// Segmentation MRML node containing shown segments
  vtkMRMLSegmentationNode* SegmentationNode;

  /// Selected segment ID
  QString SelectedSegmentID;
};

//-----------------------------------------------------------------------------
qMRMLSegmentsEditorWidgetPrivate::qMRMLSegmentsEditorWidgetPrivate(qMRMLSegmentsEditorWidget& object)
  : q_ptr(&object)
{
  this->SegmentationNode = NULL;
}

//-----------------------------------------------------------------------------
void qMRMLSegmentsEditorWidgetPrivate::init()
{
  Q_Q(qMRMLSegmentsEditorWidget);
  this->setupUi(q);

  qMRMLWindowLevelPanel* wlpanel = new qMRMLWindowLevelPanel(this->page_threshould_options);

  wlpanel->setLayout(this->page_threshould_options->layout());
  
    
  // Make connections
  QObject::connect( this->pushButton_paintbrush, SIGNAL(clicked()),
    q, SLOT(onPaintBrushClicked( )) );

  QObject::connect(this->pushButton_poly, SIGNAL(clicked()),
	  q, SLOT(onPolyClicked()));


  QObject::connect(this->pushButton_threshold, SIGNAL(clicked()),
	  q, SLOT(onThresholdClicked()));

 
  
  
}


//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// qMRMLSegmentsEditorWidget methods

//-----------------------------------------------------------------------------
qMRMLSegmentsEditorWidget::qMRMLSegmentsEditorWidget(QWidget* _parent)
  : qMRMLWidget(_parent)
  , d_ptr(new qMRMLSegmentsEditorWidgetPrivate(*this))
{
  Q_D(qMRMLSegmentsEditorWidget);
  d->init();
  this->editorLogic = NULL;

}

//-----------------------------------------------------------------------------
qMRMLSegmentsEditorWidget::~qMRMLSegmentsEditorWidget()
{
}



//--------------------------------------------------------------------------
void qMRMLSegmentsEditorWidget::onPaintBrushClicked()
{
	Q_D(qMRMLSegmentsEditorWidget);

 
	d->stackedWidget_options->setCurrentIndex(0);

	if (d->pushButton_poly->isChecked())
	{
		d->pushButton_poly->setChecked(false);
	}

	if (d->pushButton_threshold->isChecked())
	{
		d->pushButton_threshold->setChecked(false);
	}


}

void qMRMLSegmentsEditorWidget::onPolyClicked()
{
	Q_D(qMRMLSegmentsEditorWidget);
	d->stackedWidget_options->setCurrentIndex(1);


	if (d->pushButton_paintbrush->isChecked())
	{
		d->pushButton_paintbrush->setChecked(false);
	}

	if (d->pushButton_threshold->isChecked())
	{
		d->pushButton_threshold->setChecked(false);
	}




}
//***************************************************************************
//***********    Threshold Effect Panel**********************
//***************************************************************************
void qMRMLSegmentsEditorWidget::onThresholdClicked()
{
	Q_D(qMRMLSegmentsEditorWidget);
	d->stackedWidget_options->setCurrentIndex(2);


	if (d->pushButton_paintbrush->isChecked())
	{
		d->pushButton_paintbrush->setChecked(false);
	}

	if (d->pushButton_poly->isChecked())
	{
		d->pushButton_poly->setChecked(false);
	}
	
	


}


bool qMRMLSegmentsEditorWidget::SetSegmentsEditorLogic(qMRMLSegmentsEditorLogic * editorLogic)
{
	if (editorLogic != NULL)
	{
		this->editorLogic = editorLogic;
		return true;
	}
	return false;
}

qMRMLSegmentsEditorLogic * qMRMLSegmentsEditorWidget::GetSegmentsEditorLogic()
{
	return this->editorLogic;
}