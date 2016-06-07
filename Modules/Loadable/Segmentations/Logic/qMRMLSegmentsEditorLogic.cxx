/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLSegmentsEditorLogic.cxx,v $
Date:      $Date$
Version:   $Revision$

=========================================================================auto=*/

// MRMLLogic includes
#include "qMRMLSegmentsEditorLogic.h"
#include "vtkMRMLSliceLayerLogic.h"

// MRML includes
#include <vtkEventBroker.h>
#include <vtkMRMLCrosshairNode.h>
#include <vtkMRMLDiffusionTensorVolumeSliceDisplayNode.h>
#include <vtkMRMLGlyphableVolumeDisplayNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceCompositeNode.h>

// VTK includes
#include <vtkAlgorithmOutput.h>
#include <vtkCallbackCommand.h>
#include <vtkCollection.h>
#include <vtkImageBlend.h>
#include <vtkImageResample.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMathematics.h>
#include <vtkImageReslice.h>
#include <vtkInformation.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataCollection.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkVersion.h>

// STD includes

//----------------------------------------------------------------------------
// Convenient macros
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//----------------------------------------------------------------------------
vtkStandardNewMacro(qMRMLSegmentsEditorLogic);

//----------------------------------------------------------------------------
qMRMLSegmentsEditorLogic::qMRMLSegmentsEditorLogic()
{
	
}

//----------------------------------------------------------------------------
qMRMLSegmentsEditorLogic::~qMRMLSegmentsEditorLogic()
{
	
}

//----------------------------------------------------------------------------
// TODO: Remove from API
bool qMRMLSegmentsEditorLogic::IsInitialized()
{
	return this->Initialized;
}

//----------------------------------------------------------------------------
// TODO: Remove from API
void qMRMLSegmentsEditorLogic::Initialize(vtkMRMLSliceNode* newSliceNode)
{
	/*
	if (this->Initialized)
	{
		vtkWarningMacro(<< "qMRMLSegmentsEditorLogic already initialized");
		return;
	}

	// Sanity checks
	if (!newSliceNode)
	{
		vtkWarningMacro(<< "Initialize - newSliceNode is NULL");
		return;
	}

	this->SetSliceNode(newSliceNode);

	this->Initialized = true;
	*/
}

//----------------------------------------------------------------------------
void qMRMLSegmentsEditorLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
	/*
	// Sanity checks
	if (!this->GetName() || strlen(this->GetName()) == 0)
	{
		vtkErrorMacro(<< "Name is NULL - Make sure you call SetName before SetMRMLScene !");
		return;
	}

	// List of events the slice logics should listen
	vtkNew<vtkIntArray> events;
	events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
	events->InsertNextValue(vtkMRMLScene::StartCloseEvent);
	events->InsertNextValue(vtkMRMLScene::EndImportEvent);
	events->InsertNextValue(vtkMRMLScene::EndRestoreEvent);
	events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
	events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);

	this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());

	this->ProcessMRMLLogicsEvents();

	this->BackgroundLayer->SetMRMLScene(newScene);
	this->ForegroundLayer->SetMRMLScene(newScene);
	this->LabelLayer->SetMRMLScene(newScene);

	this->ProcessMRMLSceneEvents(newScene, vtkMRMLScene::EndBatchProcessEvent, 0);
	*/
}

//--------


//----------------------------------------------------------------------------
void qMRMLSegmentsEditorLogic
::ProcessMRMLLogicsEvents(vtkObject* vtkNotUsed(caller),
	unsigned long vtkNotUsed(event),
	void* vtkNotUsed(callData))
{
	this->ProcessMRMLLogicsEvents();
}

//----------------------------------------------------------------------------
void qMRMLSegmentsEditorLogic::ProcessMRMLLogicsEvents()
{
	/*

	//
	// if we don't have layers yet, create them
	//
	if (this->BackgroundLayer == 0)
	{
		vtkNew<vtkMRMLSliceLayerLogic> layer;
		this->SetBackgroundLayer(layer.GetPointer());
	}
	if (this->ForegroundLayer == 0)
	{
		vtkNew<vtkMRMLSliceLayerLogic> layer;
		this->SetForegroundLayer(layer.GetPointer());
	}
	if (this->LabelLayer == 0)
	{
		vtkNew<vtkMRMLSliceLayerLogic> layer;
		// turn on using the label outline only in this layer
		layer->IsLabelLayerOn();
		this->SetLabelLayer(layer.GetPointer());
	}
	// Update slice plane geometry
	if (this->SliceNode != 0
		&& this->GetSliceModelNode() != 0
		&& this->GetMRMLScene() != 0
		&& this->GetMRMLScene()->GetNodeByID(this->SliceModelNode->GetID()) != 0
		&& this->SliceModelNode->GetPolyData() != 0)
	{
		int *dims1 = 0;
		int dims[3];
		vtkMatrix4x4 *textureToRAS = 0;
		if (this->SliceNode->GetSliceResolutionMode() != vtkMRMLSliceNode::SliceResolutionMatch2DView)
		{
			textureToRAS = this->SliceNode->GetUVWToRAS();
			dims1 = this->SliceNode->GetUVWDimensions();
			dims[0] = dims1[0] - 1;
			dims[1] = dims1[1] - 1;
		}
		else
		{
			textureToRAS = this->SliceNode->GetXYToRAS();
			dims1 = this->SliceNode->GetDimensions();
			dims[0] = dims1[0];
			dims[1] = dims1[1];
		}
		// set the plane corner point for use in a model
		double inPt[4] = { 0,0,0,1 };
		double outPt[4];
		double *outPt3 = outPt;

		// set the z position to be the active slice (from the lightbox)
		inPt[2] = this->SliceNode->GetActiveSlice();

		vtkPlaneSource* plane = vtkPlaneSource::SafeDownCast(
			this->SliceModelNode->GetPolyDataConnection()->GetProducer());

		textureToRAS->MultiplyPoint(inPt, outPt);
		plane->SetOrigin(outPt3);
		inPt[0] = dims[0];
		textureToRAS->MultiplyPoint(inPt, outPt);
		plane->SetPoint1(outPt3);

		inPt[0] = 0;
		inPt[1] = dims[1];
		textureToRAS->MultiplyPoint(inPt, outPt);
		plane->SetPoint2(outPt3);


		this->UpdatePipeline();
		/// \tbd Ideally it should not be fired if the output polydata is not
		/// modified.
		plane->Modified();

		vtkMRMLModelDisplayNode *modelDisplayNode = this->SliceModelNode->GetModelDisplayNode();
		if (modelDisplayNode)
		{
			if (this->LabelLayer && this->LabelLayer->GetImageDataConnectionUVW())
			{
				modelDisplayNode->SetInterpolateTexture(0);
			}
			else
			{
				modelDisplayNode->SetInterpolateTexture(1);
			}
			if (this->SliceCompositeNode != 0)
			{
				modelDisplayNode->SetSliceIntersectionVisibility(this->SliceCompositeNode->GetSliceIntersectionVisibility());
			}
		}
	}

	// This is called when a slice layer is modified, so pass it on
	// to anyone interested in changes to this sub-pipeline
	this->Modified();
	*/
}

