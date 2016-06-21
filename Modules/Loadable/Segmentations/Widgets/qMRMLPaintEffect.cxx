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
#include "qMRMLPaintEffect.h"
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

#include "vtkPolyDataMapper2D.h"




// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


qMRMLPaintEffect::qMRMLPaintEffect()
{
	this->brushSize = 10 ; //defalt size is 10 mm
	this->shape = qMRMLPaintEffect::Circle;
	this->pixelMode = true;
	
	//initialize the brush data
	this->brush = vtkPolyData::New();
	this->CreateGlyph(this->brush);

	
	vtkPolyDataMapper2D * mapper = vtkPolyDataMapper2D::New();
	this->brushActor = vtkActor2D::New();
	mapper->SetInputData(this->brush);
	this->brushActor->SetMapper(mapper);
	this->brushActor->VisibilityOff();

	this->renderer->AddActor2D(this->brushActor);
	this->actors->AddItem(this->brushActor);

	//python code call default parameters
	//this->ProcessEvent();
	
}



qMRMLPaintEffect::~qMRMLPaintEffect()
{
	this->actors->Delete();

} 

qMRMLPaintEffect::qMRMLPaintEffect(qMRMLSliceWidget* sliceWidget):Superclass(sliceWidget)
{
	qMRMLPaintEffect::qMRMLPaintEffect();

}


//clean up actors and observers
void qMRMLPaintEffect::CleanUp()
{
	int number = this->feedbackActors->GetNumberOfItems();
	this->feedbackActors->InitTraversal();
	for (int i = 0; i < number; i++)
	{
		this->renderer->RemoveActor2D(this->feedbackActors->GetNextActor2D());
	}
	this->sliceView->scheduleRender();
	
	Superclass::CleanUp();
}


void qMRMLPaintEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
{
	//events from the interactor

	if (event == vtkCommand::LeftButtonPressEvent)
	{
		this->actionState = "painting";
		int *xy = this->interactor->GetEventPosition();
		this->PaintAddPoint(xy[0], xy[1]);
		this->AbortEvent(event);
	}
	else if (event == vtkCommand::LeftButtonReleaseEvent)
	{
		this->PaintApply();
		this->actionState = "";
		this->CursorOn();
	}
	else if (event == vtkCommand::RightButtonPressEvent)
	{
		
	}
	else if (event == vtkCommand::RightButtonReleaseEvent)
	{
		

	}
	else if (event == vtkCommand::MouseMoveEvent)
	{
		this->brushActor->VisibilityOn();
		if (this->actionState == "painting")
		{
			int *xy = this->interactor->GetEventPosition();
			this->PaintAddPoint(xy[0], xy[1]);
			this->AbortEvent(event);
		}
	
	}
	else if (event == vtkCommand::EnterEvent)
	{
		this->brushActor->VisibilityOn();
	}
	else if (event == vtkCommand::LeaveEvent)
	{
		this->brushActor->VisibilityOff();
	}
	else if (event == vtkCommand::KeyPressEvent)
	{
		
		char * key = this->interactor->GetKeySym();
		if (!strcmp(key, "plus") || !strcmp(key, "equal"))
		{
			this->ScaleBrushSize(1.2);
		}
		if (!strcmp(key, "minus") || !strcmp(key, "underscore"))
		{
			this->ScaleBrushSize(0.8);
		}
	}
	else
	{
		Superclass::ProcessEvent(caller, event, callData);
	}

	//# events from the slice node
	if (caller && caller->IsA("vtkMRMLSliceNode"))
	{
		
		if (this->brush)
		{
			this->CreateGlyph(this->brush);	
		}
	}

	this->PositionActors();

}



//create a brush circle of the right radius in XY space
//- assume uniform scaling between XY and RAS which
//is enforced by the view interactors
void qMRMLPaintEffect::CreateGlyph(vtkPolyData * brush)
{

}

//update paint feedback glyph to follow mouse
void qMRMLPaintEffect::PositionActors()
{
	if (this->brushActor)
	{
		int* xy = this->interactor->GetEventPosition();
		double d_xy[2] = { double(xy[0]),doubel(xy[1])};
		this->brushActor->SetPosition(d_xy);
		this->sliceView->scheduleRender();
	}
			
}


void qMRMLPaintEffect::ScaleBrushSize(double scaleFactor)
{
	this->brushSize = this->brushSize * scaleFactor;
}


void qMRMLPaintEffect::PaintAddPoint(int x, int y)
{

}


void qMRMLPaintEffect::PaintApply()
{

}
