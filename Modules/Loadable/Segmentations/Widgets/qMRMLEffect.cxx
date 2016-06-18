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

#include "vtkMRMLSliceNode.h"
#include "vtkGeneralTransform.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


qMRMLEffect::qMRMLEffect()
{
	this->actionState = "";
	this->effectScope = qMRMLEffect::All;

	this->actors = vtkActorCollection::New();
	this->interactorObserverTags = vtkUnsignedLongArray::New();

	this->savedCursor = QCursor(Qt::ArrowCursor); //the default cursor

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
	callback->SetCallback(qMRMLEffect::EffectEventCallback);



	for (int i = 0; i < events->GetSize(); i++)
	{
		int e = this->events->GetValue(i);

		unsigned long tag = this->interactor->AddObserver(e, callback, 1.0);
		this->interactorObserverTags->InsertNextValue(tag);
	}

	this->sliceNodeTags = vtkUnsignedLongArray::New();

	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

	unsigned long tag = sliceNode->AddObserver(vtkCommand::ModifiedEvent, callback, 1.0);
	this->sliceNodeTags->InsertNextValue(tag);
	 
	this->scopedImageBuffer = vtkImageData::New();
	this->scopedSlicePaint = vtkImageSlicePaint::New();
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

	this->sliceWidget = sliceWidget;
	this->sliceLogic = this->sliceWidget->sliceLogic();
	this->sliceView =  this->sliceWidget->sliceView();

	this->interactor = this->sliceView->interactorStyle()->GetInteractor();
	this->renderWindow = this->sliceView->renderWindow();
	this->renderer = vtkRenderer::SafeDownCast(
		this->renderWindow->GetRenderers()->GetItemAsObject(0));
	

}




void qMRMLEffect::SetEditorLogic(qMRMLSegmentsEditorLogic* editorLogic)
{
	this->editorLogic = editorLogic;
}

void qMRMLEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
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
			float xypoint[2] = { float(xy[0]) ,float(xy[1]) };
			if (this->interactor->FindPokedRenderer(xy[0], xy[1]))
			{
				this->editorLogic->SetLabel(this->LabelAtXY(xypoint));
			}
			else
			{
				std::cout << "'not in viewport";
			}

			this->AbortEvent(event);
			
		}
	}

}




void qMRMLEffect::EffectEventCallback (vtkObject *caller,
	unsigned long eid,
	void *clientData,
	void *callData)
{
	qMRMLEffect* self =
		reinterpret_cast<qMRMLEffect *>(clientData);
	self->ProcessEvent(caller, eid, callData);
}


	
void qMRMLEffect::AbortEvent(unsigned long event)	
{
	/*Set the AbortFlag on the vtkCommand associated
	 with the event - causes other things listening to the
	 interactor not to receive the events"""
	*/
	
	for (int i = 0; i < this->interactorObserverTags->GetSize(); i++)
	{
		int tag = this->interactorObserverTags->GetValue(i);
		
		vtkCommand * cmd= this->interactor->GetCommand(tag);

		cmd->SetAbortFlag(1);
	}

}

//Turn off and save the current cursor so the user can see the background image during editing
void qMRMLEffect::CursorOff()
{
	this->savedCursor = this->sliceWidget->cursor();	
	this->sliceWidget->setCursor(QCursor(Qt::BlankCursor));
}

//Restore the saved cursor if it exists, otherwise just restore the default cursor
void qMRMLEffect::CursorOn()
{
	this->sliceWidget->setCursor(this->savedCursor);
}

//clean up actors and observers
void qMRMLEffect::CleanUp()
{
	int number = this->actors->GetNumberOfItems();
	this->actors->InitTraversal();
	for (int i = 0; i < number; i++)
	{
		this->renderer->RemoveActor2D(this->actors->GetNextActor());
	}
	this->sliceView->scheduleRender();

	for (int i = 0; i < this->interactorObserverTags->GetSize(); i++)
	{
		int tag = this->interactorObserverTags->GetValue(i);
		this->interactor->RemoveObserver(tag);
	}

	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();
	for (int i = 0; i < this->sliceNodeTags->GetSize(); i++)
	{
		int tag = this->interactorObserverTags->GetValue(i);
		sliceNode->RemoveObserver(tag);
	}

}


void qMRMLEffect::RASToXY(float* rasPoint, float * xy)
{
	float xyz[3] = { 0,0,0 };
		
	this->RASToXYZ(rasPoint,xyz);

	//float xy[2];
	xy[0] = xyz[0];
	xy[1] = xyz[1];
}

void qMRMLEffect::RASToXYZ(float* rasPoint, float *xyz)
{
	vtkMRMLSliceNode *sliceNode = this->sliceLogic->GetSliceNode();
	vtkMatrix4x4 * rasToXY = vtkMatrix4x4::New();
	rasToXY->DeepCopy(sliceNode->GetXYToRAS());
	rasToXY->Invert();
	float point[4];
	float* xyzw;

	point[0] = rasPoint[0];
	point[1] = rasPoint[1];
	point[2] = rasPoint[2];
	point[3] = 1.0;

	xyzw = rasToXY->MultiplyPoint(point);

	//float xyz[3];
	xyz[0] = xyzw[0];
	xyz[1] = xyzw[1];
	xyz[2] = xyzw[2];

	
}

void qMRMLEffect::XYToRAS(float* xyPoint, float * ras)
{
	vtkMRMLSliceNode *sliceNode = this->sliceLogic->GetSliceNode();

	float point[4];
	float* rast;

	point[0] = xyPoint[0];
	point[1] = xyPoint[1];
	point[2] = 0.0;
	point[3] = 1.0;

	rast = sliceNode->GetXYToRAS()->MultiplyPoint(point);

	//float ras[3];

	ras[0] = rast[0];
	ras[1] = rast[1];
	ras[2] = rast[2];
 
	
}

void  qMRMLEffect::LayerXYToIJK(vtkMRMLSliceLayerLogic * layerLogic, float * xyPoint, int * IJK)
{
	vtkGeneralTransform * xyToIJK = layerLogic->GetXYToIJKTransform();

	double point[3];

	point[0] = xyPoint[0];
	point[1] = xyPoint[1];
	point[2] = 0.0;

	double*  dijk = xyToIJK->TransformDoublePoint(point);

	//int IJK[3];

	IJK[0] = int(round(dijk[0]));
	IJK[1] = int(round(dijk[1]));
	IJK[2] = int(round(dijk[2]));


}

void  qMRMLEffect::BackgroundXYToIJK(float* xyPoint, int * IJK)
{
    vtkMRMLSliceLayerLogic *  sliceLayerLogic= this->sliceLogic->GetBackgroundLayer();
	this->LayerXYToIJK(sliceLayerLogic, xyPoint,IJK);
}


void  qMRMLEffect::LabelXYToIJK(float * xyPoint, int * IJK)
{
	vtkMRMLSliceLayerLogic *  sliceLayerLogic = this->sliceLogic->GetLabelLayer();
	this->LayerXYToIJK(sliceLayerLogic, xyPoint,IJK);
}

int     qMRMLEffect::LabelAtXY(float * xyPoint)
{
	int ijk[3] = { 0,0,0 };
		
	this->LabelXYToIJK(xyPoint,ijk);

	vtkMRMLSliceLayerLogic *  sliceLayerLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLVolumeNode * volumeNode =sliceLayerLogic->GetVolumeNode();
	if (!volumeNode) return 0;
	vtkImageData * imageData = volumeNode->GetImageData();
	if (!imageData) return 0;
	int * dims = imageData->GetDimensions();

	for (int i = 0; i < 3; i++)
	{
		if (ijk[i] < 0 || ijk[i] >= dims[i]) return 0;
		
	}

	return int(imageData->GetScalarComponentAsDouble(ijk[0], ijk[1], ijk[2], 0));

}

void qMRMLEffect::XYZToRAS(float *xyzPoint, float * ras)
{
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

 
		
	float point[4];
	float* rast;

	point[0] = xyzPoint[0];
	point[1] = xyzPoint[1];
	point[2] = xyzPoint[2];
	point[3] = 1.0;

	rast = sliceNode->GetXYToRAS()->MultiplyPoint(point);


	//float ras[3];

	ras[0] = rast[0];
	ras[1] = rast[1];
	ras[2] = rast[2];

	

}

float * qMRMLEffect::GetPaintColor() //get the rgba
{
	//vtkMRMLSliceLayerLogic *  sliceLayerLogic = this->sliceLogic->GetLayerLogic();
	
	
	float color[4];
	
	return color;
	
}


vtkImageData* qMRMLEffect::GetScopedLayer(vtkMRMLSliceLayerLogic * layerLogic)
{
	vtkMRMLVolumeNode * volumeNode = layerLogic->GetVolumeNode();
	if (!volumeNode) return NULL;
	vtkImageData * imageData = volumeNode->GetImageData();
	if (!imageData) return NULL;

	if (this->effectScope == qMRMLEffect::All)
	{
		return imageData;
	}
	else if (this->effectScope == qMRMLEffect::Visible)
	{
		this->scopedSlicePaint->SetWorkingImage(imageData);
		this->scopedSlicePaint->SetExtractImage(this->scopedImageBuffer);

		QList<int*> * corners = new QList<int*>;
		this->GetVisibleCorners(layerLogic, corners);

	
		//Set the scoped corners,and update
		this->scopedSlicePaint->SetTopLeft(corners->value(0));
		this->scopedSlicePaint->SetTopRight(corners->value(1));
		this->scopedSlicePaint->SetBottomLeft(corners->value(2));
		this->scopedSlicePaint->SetBottomRight(corners->value(3));

		this->scopedSlicePaint->Paint();
		return this->scopedImageBuffer;
	}
	else
	{
		std::cout << "Invalid scope option %s" << this->effectScope;
	}

	return NULL;
  
}

vtkImageData* qMRMLEffect::GetScopedBackground()
{
	vtkMRMLSliceLayerLogic *  sliceLayerLogic = this->sliceLogic->GetBackgroundLayer();
 
	return this->GetScopedLayer(sliceLayerLogic);
}

vtkImageData*  qMRMLEffect::GetScopedLabelInput()
{
	vtkMRMLSliceLayerLogic *  sliceLayerLogic = this->sliceLogic->GetLabelLayer();

	return this->GetScopedLayer(sliceLayerLogic);
}

vtkImageData*  qMRMLEffect::GetScopedLabelOutput()
{
	return this->scopedImageBuffer;
}

void qMRMLEffect::ApplyScopedLabel()
{
	vtkMRMLSliceLayerLogic *  layerLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLVolumeNode * volumeNode = layerLogic->GetVolumeNode();

	vtkImageData * targetImage = volumeNode->GetImageData();
	if (this->effectScope == qMRMLEffect::All)
	{
		targetImage->DeepCopy(this->scopedImageBuffer);
	}
	else if (this->effectScope == qMRMLEffect::Visible)
	{
		this->scopedSlicePaint->SetWorkingImage(targetImage);
		this->scopedSlicePaint->SetExtractImage(this->scopedImageBuffer);

		QList<int*> * corners = new QList<int*>;
		this->GetVisibleCorners(layerLogic, corners);


		//Set the scoped corners,and update
		this->scopedSlicePaint->SetTopLeft(corners->value(0));
		this->scopedSlicePaint->SetTopRight(corners->value(1));
		this->scopedSlicePaint->SetBottomLeft(corners->value(2));
		this->scopedSlicePaint->SetBottomRight(corners->value(3));

		this->scopedSlicePaint->Paint();
	}
	else
	{
		std::cout << "Invalid scope option %s" << this->effectScope;
	}

	this->editorLogic->markVolumeNodeAsModified(volumeNode);

}

void qMRMLEffect::GetVisibleCorners(vtkMRMLSliceLayerLogic * layerLogic, QList<int*> * out_ijkCorners)
{
	vtkGeneralTransform * xyToIJK = layerLogic->GetXYToIJKTransform();

	//w:dims[0],h:dims[1],d:dims[2]
	int * dims = layerLogic->GetImageData()->GetDimensions();

	//Initial the Corner's XY 
    double  tlxyCorner[3] = { 0, 0, 0 };
	double  trxyCorner[3] = {double(dims[0]),0,0};
	double  blxyCorner[3] = {0,double(dims[1]),0};
	double  brxyCorner[3] = {double(dims[0]),double(dims[1])};

	//tlijkCorner[3], trijkCorner[3], blijkCorner[3], brijkCorner[3];
	double * tlCorner, *trCorner, *blCorner, *brCorner;

	tlCorner = xyToIJK->TransformDoublePoint(tlxyCorner);
	
	trCorner = xyToIJK->TransformDoublePoint(trxyCorner);

	blCorner = xyToIJK->TransformDoublePoint(blxyCorner);
	
	brCorner = xyToIJK->TransformDoublePoint(brxyCorner);
 
	int tlijkCorner[3] = {int(round(tlCorner[0])), int(round(tlCorner[1])), int(round(tlCorner[2])) };
	int	trijkCorner[3] = { int(round(trCorner[0])), int(round(trCorner[1])), int(round(trCorner[2])) };
	int	blijkCorner[3] = { int(round(blCorner[0])), int(round(blCorner[1])), int(round(blCorner[2])) };
	int brijkCorner[3] = { int(round(brCorner[0])), int(round(brCorner[1])), int(round(brCorner[2])) };


	//QList<double*> ijkCorners;
	out_ijkCorners->append(tlijkCorner);
	out_ijkCorners->append(trijkCorner);
	out_ijkCorners->append(blijkCorner);
	out_ijkCorners->append(brijkCorner);

}

