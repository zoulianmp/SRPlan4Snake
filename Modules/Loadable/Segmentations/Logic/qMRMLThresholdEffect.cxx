/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.11 $

=========================================================================auto=*/

// MRML includes
#include "qMRMLThresholdEffect.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include "vtkInteractorObserver.h"
#include "vtkRendererCollection.h"
#include <vtkActorCollection.h>
#include <vtkActor.h>
#include "vtkIntArray.h"
#include "vtkStdString.h"
#include "vtkImageMapper.h"
#include "vtkActor2D.h"
#include "vtkImageThreshold.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToRGBA.h"
#include "vtkMRMLSliceLayerLogic.h"
#include "vtkImageReslice.h"

#include "vtkMRMLSliceNode.h"
#include "vtkGeneralTransform.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort



vtkStandardNewMacro(qMRMLThresholdEffect);

qMRMLThresholdEffect::qMRMLThresholdEffect()
{
	
	
}
void qMRMLThresholdEffect::SetupThreshold()
{
	this->InitFeedbackActor();
}


qMRMLThresholdEffect::~qMRMLThresholdEffect()
{
	

} 

qMRMLThresholdEffect::qMRMLThresholdEffect(qMRMLSliceWidget* sliceWidget)
{


}

void qMRMLThresholdEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
{
	

}



//clean up actors and observers
void qMRMLThresholdEffect::CleanUp()
{
	Superclass::CleanUp();

}



int qMRMLThresholdEffect::GetThresholdMin()
{
	return this->min;
}
int qMRMLThresholdEffect::GetThresholdMax()
{
	return this->max;
}

void qMRMLThresholdEffect::SetThresholdMin(int min)
{
	this->min = min;
}

void qMRMLThresholdEffect::SetThresholdMax(int max)
{
	this->max = max;
}

// feedback actor
void qMRMLThresholdEffect::InitFeedbackActor()
{
	
	this->cursorMapper = vtkImageMapper::New();
	vtkImageData* cursorDummyImage = vtkImageData::New();
	cursorDummyImage->AllocateScalars(VTK_UNSIGNED_INT, 1);
	this->cursorMapper->SetInputData(cursorDummyImage);

	this->cursorActor = vtkActor2D::New();
	this->cursorActor->VisibilityOff();
	this->cursorActor->SetMapper(cursorMapper);
	this->cursorMapper->SetColorWindow(255);
	this->cursorMapper->SetColorLevel(128);

	this->actors->AddItem(cursorActor);
	this->renderer->AddActor2D(cursorActor);


}

void qMRMLThresholdEffect::ApplyThreshold(ThresholdMode type)
{
	if (!this->editorLogic->GetBackgroundImage() || !this->editorLogic->GetLabelImage())
	{
		return;
	}

	vtkImageThreshold* thresh = vtkImageThreshold::New();
	thresh->SetInputData(this->editorLogic->GetBackgroundImage());

	switch (type)
	{
	case ByLower:
		thresh->ThresholdByLower(this->max) ;
		break;
	case Between:
		thresh->ThresholdBetween(this->min, this->max);
		break;
	case ByUpper:
		thresh->ThresholdByUpper(this->min);
		break;
	}

	thresh->SetInValue(this->editorLogic->GetLabel());
	thresh->SetOutValue(0);
	thresh->SetOutputScalarType(this->editorLogic->GetLabelImage()->GetScalarType());
	thresh->Update();

	this->editorLogic->GetLabelImage()->DeepCopy(thresh->GetOutput());
	this->editorLogic->markVolumeNodeAsModified(this->editorLogic->GetLabelVolume());

}


void qMRMLThresholdEffect::PreviewThreshold(ThresholdMode type)
{
	if (!this->editorLogic->GetBackgroundImage() || !this->editorLogic->GetLabelImage())
	{
		return;
	}

	float* color = this->GetPaintColor();

	vtkLookupTable * lut = vtkLookupTable::New();
	lut->SetRampToLinear();
	lut->SetNumberOfTableValues(2);
	lut->SetTableRange(0, 1);
	lut->SetTableValue(0, 0, 0, 0, 0);
	lut->SetTableValue(1, color[0], color[1], color[2], 0.8);

	vtkImageMapToRGBA * map = vtkImageMapToRGBA::New();

	map->SetOutputFormatToRGBA();
	map->SetLookupTable(lut);

	vtkImageThreshold* thresh = vtkImageThreshold::New();

	
	vtkMRMLSliceLayerLogic * backgroundLogic = this->sliceLogic->GetBackgroundLayer();

	thresh->SetInputConnection(backgroundLogic->GetReslice()->GetOutputPort());



	switch (type)
	{
	case ByLower:
		thresh->ThresholdByLower(this->max);
		break;
	case Between:
		thresh->ThresholdBetween(this->min, this->max);
		break;
	case ByUpper:
		thresh->ThresholdByUpper(this->min);
		break;
	}
	//thresh->ThresholdBetween(this->min, this->max);
	thresh->SetInValue(1);
	thresh->SetOutValue(0);
	thresh->SetOutputScalarTypeToUnsignedChar();
	map->SetInputConnection(thresh->GetOutputPort());
	this->cursorMapper->SetInputConnection(map->GetOutputPort());

	this->cursorActor->VisibilityOn();

	this->sliceView->scheduleRender();

}
