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
#include "qMRMLEffect.h"
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

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


qMRMLEffect::qMRMLEffect()
{
	this->actionState = "";
	this->EffectOption = qMRMLEffect::All;
	this->actors = vtkActorCollection::New();
	this->interactorObserverTags = vtkUnsignedLongArray::New();

	this->events = vtkIntArray::New();

	this->events->InsertNextValue(vtkCommand::LeftButtonPressEvent);
	this->events->InsertNextValue(vtkCommand::LeftButtonReleaseEvent);
	this->events->InsertNextValue(vtkCommand::MiddleButtonPressEvent);
	this->events->InsertNextValue(vtkCommand::MiddleButtonReleaseEvent);
	this->events->InsertNextValue(vtkCommand::RightButtonPressEvent);
	this->events->InsertNextValue(vtkCommand::RightButtonReleaseEvent);
	this->events->InsertNextValue(vtkCommand::MouseMoveEvent);
	this->events->InsertNextValue(vtkCommand::KeyPressEvent);
	this->events->InsertNextValue(vtkCommand::EnterEvent);
	this->events->InsertNextValue(vtkCommand::LeaveEvent);


	vtkSmartPointer<vtkCallbackCommand> callback =
		vtkSmartPointer<vtkCallbackCommand>::New();
	callback->SetCallback(qMRMLEffect::ProcessEvent);



	for (int i = 0; i < events->GetSize(); i++)
	{
		int e = this->events->GetValue(i);

		unsigned int tag = this->interactor->AddObserver(e, callback, 1.0);
		this->interactorObserverTags->InsertNextValue(tag);

	}

}



qMRMLEffect::~qMRMLEffect()
{
	this->actors->Delete();

} 

qMRMLEffect::qMRMLEffect(qMRMLSliceWidget* sliceWidget)
{
	qMRMLEffect::qMRMLEffect();

	this->SetSliceWidget(sliceWidget);

}

void qMRMLEffect::SetSliceWidget(qMRMLSliceWidget* sliceWidget)
{

	qMRMLEffect::sliceWidget = sliceWidget;
	qMRMLEffect::sliceLogic = qMRMLEffect::sliceWidget->sliceLogic();
	qMRMLEffect::sliceView =  qMRMLEffect::sliceWidget->sliceView();

	qMRMLEffect::interactor = qMRMLEffect::sliceView->interactorStyle()->GetInteractor();
	qMRMLEffect::renderWindow = qMRMLEffect::sliceView->renderWindow();
	qMRMLEffect::renderer = vtkRenderer::SafeDownCast(
		qMRMLEffect::renderWindow->GetRenderers()->GetItemAsObject(0));
	

}




void qMRMLEffect::SetEditorLogic(qMRMLSegmentsEditorLogic* editorLogic)
{
	qMRMLEffect::editorLogic = editorLogic;
}

void qMRMLEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *clientData, void *callData)
{
	// Event filter that lisens for certain key events that
	// should be responded to by all events.
	// Currently:
	// pick up paint color from current location(eyedropper)
	if (event == vtkCommand::KeyPressEvent)
	{
		QString key = QString(qMRMLEffect::interactor->GetKeySym()).toLower();
		if (key == QString("backslash"))
		{
			int * xy = qMRMLEffect::interactor->GetEventPosition();
			if (qMRMLEffect::interactor->FindPokedRenderer(xy[0], xy[1]))
			{
				qMRMLEffect::editorLogic->SetLabel(this->LabelAtXY(xy[0], xy[1]));
			}
			else
			{
				std::cout << "'not in viewport";
			}

			qMRMLEffect::AbortEvent(event);
			
		}
	}

}

/*


//----------------------------------------------------------------------------
void vtkMRMLAbstractDisplayableManager
::ProcessWidgetsEvents(vtkObject *vtkNotUsed(caller),
	unsigned long vtkNotUsed(event),
	void *vtkNotUsed(callData))
{
}

//----------------------------------------------------------------------------
// Description:
// the WidgetCallback is a static function to relay modified events from the
// observed vtk widgets back into the mrml node for further processing
void vtkMRMLAbstractDisplayableManager::WidgetsCallback(vtkObject *caller,
	unsigned long eid,
	void *clientData,
	void *callData)
{
	vtkMRMLAbstractDisplayableManager* self =
		reinterpret_cast<vtkMRMLAbstractDisplayableManager *>(clientData);
	assert(!caller->IsA("vtkMRMLNode"));
	self->ProcessWidgetsEvents(caller, eid, callData);
}

*/








	
	
void qMRMLEffect::AbortEvent(unsigned long event)	
{
	/*Set the AbortFlag on the vtkCommand associated
	 with the event - causes other things listening to the
	 interactor not to receive the events"""
	*/
	
	for (int i = 0; i < qMRMLEffect::interactorObserverTags->GetSize(); i++)
	{
		int tag = qMRMLEffect::interactorObserverTags->GetValue(i);
		
		vtkCommand * cmd= qMRMLEffect::interactor->GetCommand(tag);

		cmd->SetAbortFlag(1);
	}

}