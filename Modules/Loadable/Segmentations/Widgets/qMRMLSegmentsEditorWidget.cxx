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

  // Make connections
  QObject::connect( this->pushButton_paintbrush, SIGNAL(clicked()),
    q, SLOT(onPaintBrushClicked( )) );

  QObject::connect(this->pushButton_poly, SIGNAL(clicked()),
	  q, SLOT(onPolyClicked()));


  QObject::connect(this->pushButton_threshold, SIGNAL(clicked()),
	  q, SLOT(onThresholdClicked()));

  QObject::connect(this->comboBox_wllist, SIGNAL(currentIndexChanged(int)),
	  q, SLOT(onPredefinedWLChanged(int index)));
  
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
void qMRMLSegmentsEditorWidget::onPredefinedWLChanged(int index)
{
	Q_D(qMRMLSegmentsEditorWidget);
	//The Default Slider Range= -1000,1000 HU

	//AutoBody,Window Level 
	int min = -350;
	int max = 1000;

	switch (index) 
	{
		case 0:
			//AutoBody,Window Level 
			 min = -350;
			 max = 1000;
			 break;
		case 1:
		 	 min = -350;
			 max = 1000;
			 break;
	    case 2:
			min = -350;
			max = 1000;
			break;   
	    default:;
	}

	int level = (min + max) / 2;
	int window = max - min;

	//update GUI widget
	d->RangeSlider_wl->setMinimumValue(min);
	d->RangeSlider_wl->setMaximumValue(max);

	d->spinBox_l->setValue(level);
	d->spinBox_w->setValue(window);

}
void qMRMLSegmentsEditorWidget::onWindowChanged()
{

}

void qMRMLSegmentsEditorWidget::onLevelChanged()
{


}

void qMRMLSegmentsEditorWidget::onWLSliderChanged()
{


}