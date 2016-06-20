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
#include "qMRMLDrawEffect.h"
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

#include "vtkMRMLSliceNode.h"
#include "vtkGeneralTransform.h"
#include "vtkProperty2D.h"
#include "vtkTransform.h"

#include "vtkPolyDataMapper2D.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


qMRMLDrawEffect::qMRMLDrawEffect()
{
	this->initialized = false;

	//# interaction state variables
	this->activeSlice = NULL;
	this->lastInsertSliceNodeMTime = NULL;
	this->actionState = NULL;

	//	# initialization
	this->xyPoints = vtkPoints::New();
	this->rasPoints = vtkPoints::New();
	this->polyData = this->CreatePolyData();

	vtkPolyDataMapper2D *  mapper =  vtkPolyDataMapper2D::New();

	this->actor = vtkActor2D::New();
	mapper->SetInputData(this->polyData);
	this->actor->SetMapper(mapper);

	vtkProperty2D *property = this->actor->GetProperty();
	property->SetColor(1, 1, 0);
	property->SetLineWidth(1);
	this->renderer->AddActor2D(this->actor);
	this->actors->AddItem(this->actor);

	this->initialized = true;
}



qMRMLDrawEffect::~qMRMLDrawEffect()
{
	this->actors->Delete();

} 

qMRMLDrawEffect::qMRMLDrawEffect(qMRMLSliceWidget* sliceWidget)
{
	

}



//clean up actors and observers
void qMRMLDrawEffect::CleanUp()
{
	Superclass::CleanUp();
}
 
// use "solid" and "dashed" for mode index
void qMRMLDrawEffect::SetLineMode(char* mode)
{
	vtkProperty2D *property = this->actor->GetProperty();
	
	if (!strcmp(mode, "solid"))
	{
		property->SetLineStipplePattern(65535);
	}
	else if (!strcmp(mode, "dashed"))
	{
		property->SetLineStipplePattern(0xff00);
	}
}

void qMRMLDrawEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
{
	
	if (!this->initialized) return;

	//events from the interactor

	if (event == vtkCommand::LeftButtonPressEvent)
	{
		this->actionState = "drawing";
		this->CursorOff();
		int * xy = this->interactor->GetEventPosition();
         
		float xyPoint[2] = {xy[0],xy[1]};

		float rasPoint[3] = { 0,0,0 };

		this->XYToRAS(xyPoint, rasPoint);

		this->AddPoint(rasPoint);
		this->AbortEvent(event);
	}
	else if (event == vtkCommand::LeftButtonReleaseEvent)
	{
		this->actionState = "";
		this->CursorOn();
	}
	else if (event == vtkCommand::RightButtonPressEvent)
	{
		vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();
		this->lastInsertSliceNodeMTime = sliceNode->GetMTime();
	}
	else if (event == vtkCommand::RightButtonReleaseEvent)
	{
		vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();
		if (this->lastInsertSliceNodeMTime == sliceNode->GetMTime())
		{
			this->Apply();
			this->actionState = "";
		}

	}
	else if (event == vtkCommand::MouseMoveEvent)
	{
		if (!strcmp(this->actionState, "drawing")) 
		{
	 
			int * xy = this->interactor->GetEventPosition();

			float xyPoint[2] = { xy[0],xy[1] };

			float rasPoint[3] = { 0,0,0 };

			this->XYToRAS(xyPoint, rasPoint);

			this->AddPoint(rasPoint);
			this->AbortEvent(event);
		}
	}
	else if (event == vtkCommand::KeyPressEvent)
	{
		char * key = this->interactor->GetKeySym();
		if (!strcmp(key,"a") || !strcmp(key,"Return"))
		{
			this->Apply();
			this->AbortEvent(event);
		}
		if (!strcmp(key, "x"))
		{
			this->DeleteLastPoint();
			this->AbortEvent(event);
		}
			
				
	}
	else
	{
		Superclass::ProcessEvent(caller, event, callData);
	}

	///events from the slice node
		
	if (caller && caller->IsA("vtkMRMLSliceNode"))
	{
		char * lineMode = "solid";
		double currentSlice = this->sliceLogic->GetSliceOffset();
		if (this->activeSlice)
		{
			double offset = abs(currentSlice - this->activeSlice);
			if (offset > 0.01) lineMode = "dashed";
		}
		this->SetLineMode(lineMode);

	}

	this->PositionActors();

}





void qMRMLDrawEffect::AddPoint(float * ras)
{
	double currentSlice = this->sliceLogic->GetSliceOffset();
	if (!this->activeSlice)
	{
		this->activeSlice = currentSlice;
		this->SetLineMode("solid");
	}

	//# don't allow adding points on except on the active slice (where
	//# first point was laid down)
	if (this->activeSlice != currentSlice) return;

	//# keep track of node state(in case of pan / zoom)
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();
	this->lastInsertSliceNodeMTime = sliceNode->GetMTime();

	vtkIdType p = this->rasPoints->InsertNextPoint(ras);
	vtkCellArray *lines = this->polyData->GetLines();
	vtkIdTypeArray *idArray = lines->GetData();
	idArray->InsertNextTuple1(p);
	idArray->SetTuple1(0, idArray->GetNumberOfTuples() - 1);
	lines->SetNumberOfCells(1);

}

void qMRMLDrawEffect::DeleteLastPoint()
{
	vtkIdType pcount = this->rasPoints->GetNumberOfPoints();
	if (pcount <= 0) return;
	pcount = pcount - 1;
	this->rasPoints->SetNumberOfPoints(pcount);

	vtkCellArray *lines = this->polyData->GetLines();
	vtkIdTypeArray *idArray = lines->GetData();
	idArray->SetTuple1(0, pcount);
	idArray->SetNumberOfTuples(pcount + 1);

	this->PositionActors();

}

void qMRMLDrawEffect::Apply()
{
	vtkCellArray *lines = this->polyData->GetLines();
	if (lines->GetNumberOfCells() == 0) return;

	//# close the polyline back to the first point
	vtkIdTypeArray * idArray = lines->GetData();
	double p = idArray->GetTuple1(1);
	idArray->InsertNextTuple1(p);
	idArray->SetTuple1(0, idArray->GetNumberOfTuples() - 1);

		
	this->ApplyPolyMask(this->polyData);
	this->ResetPolyData();


}

void qMRMLDrawEffect::PositionActors()
{
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode(); 
	
	vtkTransform* rasToXY = vtkTransform::New();
	rasToXY->SetMatrix(sliceNode->GetXYToRAS());
	rasToXY->Inverse();
	this->xyPoints->Reset();
	rasToXY->TransformPoints(this->rasPoints, this->xyPoints);
	this->polyData->Modified();
	this->sliceView->scheduleRender();
}

vtkPolyData* qMRMLDrawEffect::CreatePolyData()
{
	this->polyData = vtkPolyData::New();
	this->polyData->SetPoints(this->xyPoints);

	vtkCellArray *lines = vtkCellArray::New();
	this->polyData->SetLines(lines);
	vtkIdTypeArray *idArray = lines->GetData();
	idArray->Reset();
	idArray->InsertNextTuple1(0);

	vtkCellArray * polygons = vtkCellArray::New();
	this->polyData->SetPolys(polygons);
	idArray = polygons->GetData();
	idArray->Reset();
	idArray->InsertNextTuple1(0);

	return polyData;
}


void qMRMLDrawEffect::ResetPolyData()
{
	vtkCellArray *lines = this->polyData->GetLines();
	vtkIdTypeArray * idArray = lines->GetData();
	
	idArray->Reset();
	idArray->InsertNextTuple1(0);
	this->xyPoints->Reset();
	this->rasPoints->Reset();
	lines->SetNumberOfCells(0);
	this->activeSlice = 0.0;

}