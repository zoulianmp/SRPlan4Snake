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

//Editor Effect include
#include "qMRMLPaintEffect.h"
#include "qMRMLDrawEffect.h"
#include "qMRMLThresholdEffect.h"


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

 // qMRMLWindowLevelPanel* wlpanel = new qMRMLWindowLevelPanel(this->page_threshould_options);

//  wlpanel->setLayout(this->page_threshould_options->layout());
  
//  wlpanel->SetSegmentsEditorLogic(q->GetSegmentsEditorLogic());
    
  // Make connections
  QObject::connect( this->pushButton_paintbrush, SIGNAL(clicked()),
    q, SLOT(onPaintBrushClicked( )) );

 

  QObject::connect(this->pushButton_threshold, SIGNAL(clicked()),
	  q, SLOT(onThresholdClicked()));

  


  this->spinBox_l->setReadOnly(true);
  this->spinBox_w->setReadOnly(true);




  QObject::connect(this->toolButton_abdomen, SIGNAL(clicked()),
	  q, SLOT(onWLAbdomenSelected()));


  QObject::connect(this->toolButton_brain, SIGNAL(clicked()),
	  q, SLOT(onWLBrainSelected()));

  QObject::connect(this->toolButton_air, SIGNAL(clicked()),
	  q, SLOT(onWLAirSelected()));

  QObject::connect(this->toolButton_bone, SIGNAL(clicked()),
	  q, SLOT(onWLBoneSelected()));

  QObject::connect(this->toolButton_lung, SIGNAL(clicked()),
	  q, SLOT(onWLLungSelected()));

    



  QObject::connect(this->spinBox_w, SIGNAL(valueChanged(int)),
	  q, SLOT(onWindowLevelChanged(int)));

  QObject::connect(this->spinBox_l, SIGNAL(valueChanged(int)),
	  q, SLOT(onWindowLevelChanged(int)));


  QObject::connect(this->wlApplyPushButton, SIGNAL(clicked()),
	  q, SLOT(onWLApplyPushButtonClicked()));

  QObject::connect(this->wlDoubleRangeSlider, SIGNAL(minimumPositionChanged(double )),
	  q, SLOT(onWLDoubleRangeSliderValueChanged(double )));
   
  QObject::connect(this->wlDoubleRangeSlider, SIGNAL(maximumPositionChanged(double )),
	  q, SLOT(onWLDoubleRangeSliderValueChanged(double )));

 
  

  
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

 
//	d->stackedWidget_options->setCurrentIndex(0);

//	if (d->pushButton_poly->isChecked())
//	{
//		d->pushButton_poly->setChecked(false);
//	}

	if (d->pushButton_threshold->isChecked())
	{
		d->pushButton_threshold->setChecked(false);
	}

	qMRMLSegmentsEditorLogic * editorLogic = this->GetSegmentsEditorLogic();
    editorLogic->SetCurrentEffectMode(qMRMLSegmentsEditorLogic::PaintBrush);

	d->groupBox_threshould->setEnabled(false);
}

/*
void qMRMLSegmentsEditorWidget::onPolyClicked()
{
	Q_D(qMRMLSegmentsEditorWidget);
//	d->stackedWidget_options->setCurrentIndex(1);


	if (d->pushButton_paintbrush->isChecked())
	{
		d->pushButton_paintbrush->setChecked(false);
	}

	if (d->pushButton_threshold->isChecked())
	{
		d->pushButton_threshold->setChecked(false);
	}


	this->editorLogic->SetCurrentEffectMode(qMRMLSegmentsEditorLogic::FreeDraw);


}

*/
//***************************************************************************
//***********    Threshold Effect Panel**********************
//***************************************************************************
void qMRMLSegmentsEditorWidget::onThresholdClicked()
{
	Q_D(qMRMLSegmentsEditorWidget);
//	d->stackedWidget_options->setCurrentIndex(2);


	if (d->pushButton_paintbrush->isChecked())
	{
		d->pushButton_paintbrush->setChecked(false);
	}

//	if (d->pushButton_poly->isChecked())
//	{
//		d->pushButton_poly->setChecked(false);
//	}
	if (d->pushButton_threshold->isChecked())
	{
		this->editorLogic->SetCurrentEffectMode(qMRMLSegmentsEditorLogic::Threshold);
		d->groupBox_threshould->setEnabled(true);
	}
	else
	{
		d->groupBox_threshould->setEnabled(false);
	}
	
	
}


bool qMRMLSegmentsEditorWidget::SetSegmentsEditorLogic(qMRMLSegmentsEditorLogic * editorLogic)
{
	Q_D(qMRMLSegmentsEditorWidget);

	if (editorLogic != NULL)
	{
		this->editorLogic = editorLogic;
//		if (editorLogic->GetSegmentsEditorWidget() != this)
//			editorLogic->SetSegmentsEditorWidget(this);
	//	d->wlpanel->SetSegmentsEditorLogic(editorLogic);
		return true;
	}
	return false;
}

qMRMLSegmentsEditorLogic * qMRMLSegmentsEditorWidget::GetSegmentsEditorLogic()
{
	return this->editorLogic;
}


void qMRMLSegmentsEditorWidget::EnableEffectButtons()
{
	Q_D(qMRMLSegmentsEditorWidget);
	d->pushButton_paintbrush->setEnabled(true);
//	d->pushButton_poly->setEnabled(true);
	d->pushButton_threshold->setEnabled(true);

}



void qMRMLSegmentsEditorWidget::onWLAbdomenSelected()
{
	Q_D(qMRMLSegmentsEditorWidget);
	//Abdomen W/L = 350/40
//	d->spinBox_l->setReadOnly(true);
//	d->spinBox_w->setReadOnly(true);


	d->spinBox_l->setValue(350);
	d->spinBox_w->setValue(40);

	
}


void qMRMLSegmentsEditorWidget::onWLBrainSelected()
{
	Q_D(qMRMLSegmentsEditorWidget);
	//Brain W/L = 100/50
//	d->spinBox_l->setReadOnly(true);
//	d->spinBox_w->setReadOnly(true);

	d->spinBox_w->setValue(100);
	d->spinBox_l->setValue(50);

	
}


void qMRMLSegmentsEditorWidget::onWLAirSelected()
{
	Q_D(qMRMLSegmentsEditorWidget);
	//Air W/L = 1000/-426
//	d->spinBox_l->setReadOnly(true);
//	d->spinBox_w->setReadOnly(true);

	d->spinBox_w->setValue(1000);
	d->spinBox_l->setValue(-426);

	
}


void qMRMLSegmentsEditorWidget::onWLBoneSelected()
{
	Q_D(qMRMLSegmentsEditorWidget);
	//Bone W/L = 1000/400
//	d->spinBox_l->setReadOnly(true);
//	d->spinBox_w->setReadOnly(true);

	d->spinBox_w->setValue(1000);
	d->spinBox_l->setValue(400);

	
}

void qMRMLSegmentsEditorWidget::onWLLungSelected()
{
	Q_D(qMRMLSegmentsEditorWidget);


//	d->spinBox_l->setReadOnly(true);
//	d->spinBox_w->setReadOnly(true);

	//Lung W/L = 1400/-500
	d->spinBox_w->setValue(1400);
	d->spinBox_l->setValue(-500);


}
 
 


void qMRMLSegmentsEditorWidget::onWindowLevelChanged(int)
{
	Q_D(qMRMLSegmentsEditorWidget);
	int min;
	int max;

	int w = d->spinBox_w->value();
	int l = d->spinBox_l->value();

	this->ConvertWLtoMinMax(w, l, min, max);

	d->wlDoubleRangeSlider->setValues(min, max);

	
	this->doThreshouldPreview();
}





void  qMRMLSegmentsEditorWidget::doThreshouldPreview()
{
	Q_D(qMRMLSegmentsEditorWidget);
 
	int min = int(d->wlDoubleRangeSlider->minimumValue());
	int max = int(d->wlDoubleRangeSlider->maximumValue());


	qMRMLThresholdEffect * thresholdEffect;
	qMRMLSegmentsEditorLogic::EffectMode currentMode = this->editorLogic->GetCurrentEffectMode();

	if (currentMode == qMRMLSegmentsEditorLogic::Threshold)
	{
		thresholdEffect = qMRMLThresholdEffect::SafeDownCast(this->editorLogic->GetCurrentEffect());
	}

	if (thresholdEffect)
	{
		thresholdEffect->SetThresholdMin(min);
		thresholdEffect->SetThresholdMax(max);

		if (min < -980) 
		{
			thresholdEffect->PreviewThreshold(qMRMLThresholdEffect::ByLower);
		}
		else if (max > 980)
		{
			thresholdEffect->PreviewThreshold(qMRMLThresholdEffect::ByUpper);
		}
		else
		{
			thresholdEffect->PreviewThreshold(qMRMLThresholdEffect::Between);
		}

		
	}


}


void qMRMLSegmentsEditorWidget::onWLApplyPushButtonClicked()
{
	Q_D(qMRMLSegmentsEditorWidget);

	int min = int(d->wlDoubleRangeSlider->minimumValue());
	int max = int(d->wlDoubleRangeSlider->maximumValue());


	qMRMLThresholdEffect * thresholdEffect;
	qMRMLSegmentsEditorLogic::EffectMode currentMode = this->editorLogic->GetCurrentEffectMode();

	if (currentMode == qMRMLSegmentsEditorLogic::Threshold)
	{
		thresholdEffect = qMRMLThresholdEffect::SafeDownCast(this->editorLogic->GetCurrentEffect());
	}

	if (thresholdEffect)
	{
		thresholdEffect->SetThresholdMin(min);
		thresholdEffect->SetThresholdMax(max);

		if (min < -980)
		{
			thresholdEffect->ApplyThreshold(qMRMLThresholdEffect::ByLower);
		}
		else if (max > 980)
		{
			thresholdEffect->ApplyThreshold(qMRMLThresholdEffect::ByUpper);
		}
		else
		{
			thresholdEffect->ApplyThreshold(qMRMLThresholdEffect::Between);
		}
		 
	}


}

void qMRMLSegmentsEditorWidget::ConvertWLtoMinMax(int w, int l, int & min, int &max)
{
 	min = l - w / 2;
	max = l + w / 2;
}

void qMRMLSegmentsEditorWidget::ConvertMinMaxtoWL(int min, int max, int & w, int &l)
{
	w = (max - min);
    l = (min + max) / 2;
}

void qMRMLSegmentsEditorWidget::onWLDoubleRangeSliderValueChanged(double value)
{
	Q_D(qMRMLSegmentsEditorWidget);

	double min = int(d->wlDoubleRangeSlider->minimumValue());
	double max = int(d->wlDoubleRangeSlider->maximumValue());

	int w;
	int l;

	this->ConvertMinMaxtoWL(min, max, w, l);
	d->spinBox_w->setValue(w);
	d->spinBox_l->setValue(l);

	this->doThreshouldPreview();
}