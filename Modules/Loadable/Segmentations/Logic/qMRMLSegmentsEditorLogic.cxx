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
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLApplicationLogic.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkSlicerApplicationLogic.h"

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

#include "vtkPointData.h"
#include "qMRMLEffect.h"
#include "qMRMLPaintEffect.h"
#include "qMRMLDrawEffect.h"
#include "qMRMLThresholdEffect.h"


class qMRMLSegmentsEditorWidget;

// STD includes

//----------------------------------------------------------------------------
// Convenient macros


//----------------------------------------------------------------------------
vtkStandardNewMacro(qMRMLSegmentsEditorLogic);

//----------------------------------------------------------------------------
qMRMLSegmentsEditorLogic::qMRMLSegmentsEditorLogic()
{
	vtkMRMLApplicationLogic * appLogic = this->GetMRMLApplicationLogic();
	this->PropagationMode = vtkMRMLApplicationLogic::BackgroundLayer | vtkMRMLApplicationLogic::LabelLayer;
	this->StoredLabel = 0;
	this->CurrentLable = 0;

//	this->EditorWidget = NULL;

	
	//Make PaintEffect and setup PaintEffect
	qMRMLPaintEffect * paintEffect = qMRMLPaintEffect::New();

	//paintEffect->SetSliceWidget(this->GetSliceWidget());
	//paintEffect->SetupEventsObservation();
	//paintEffect->SetEditorLogic(this);
	//paintEffect->SetupBrush();

	

	//Make DrawEffect and setup DrawEffect
	qMRMLDrawEffect * drawEffect = qMRMLDrawEffect::New();

	//drawEffect->SetSliceWidget(this->GetSliceWidget());
	//drawEffect->SetupEventsObservation();
	//drawEffect->SetEditorLogic(this);
	//drawEffect->SetupDraw();


	//Make ThresholdEffect  and setup ThresholdEffect
	qMRMLThresholdEffect* thresholdEffect = qMRMLThresholdEffect::New();

	//thresholdEffect->SetSliceWidget(this->GetSliceWidget());
	//thresholdEffect->SetupEventsObservation();
    //thresholdEffect->SetEditorLogic(this);
	//thresholdEffect->SetupThreshold();


	this->editorEffectMap.insert(qMRMLSegmentsEditorLogic::PaintBrush, paintEffect);
	this->editorEffectMap.insert(qMRMLSegmentsEditorLogic::FreeDraw, drawEffect);
	this->editorEffectMap.insert(qMRMLSegmentsEditorLogic::Threshold, thresholdEffect);

	//Don't Setup the Current Effect 
	//this->SetCurrentEffectMode(qMRMLSegmentsEditorLogic::PaintBrush);

	this->CurrentEffect = NULL;
	this->CurrentEffectMode = None;


}

//----------------------------------------------------------------------------
qMRMLSegmentsEditorLogic::~qMRMLSegmentsEditorLogic()
{
	
}

//----------------------------------------------------------------------------
// TODO: Remove from API


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
	
	Superclass::SetMRMLSceneInternal(newScene);
	
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




void qMRMLSegmentsEditorLogic::SetCurrentSegment(vtkSegment * segment)
{
	if (segment != NULL)
	{
		this->CurrentSegment = segment;
	}
}

void qMRMLSegmentsEditorLogic::SetCurrentSegmentation(vtkSegmentation* segmentation)
{
	if (segmentation != NULL)
	{
		this->CurrentSegmentation = segmentation;
	}
}

qMRMLSegmentsEditorLogic::EffectMode qMRMLSegmentsEditorLogic::GetCurrentEffectMode()
{
	return this->CurrentEffectMode;

}

void qMRMLSegmentsEditorLogic::SetCurrentEffectMode(EffectMode effect)
{
	if (this->CurrentEffectMode != effect) 
	{
		this->CurrentEffectMode = effect;

		if (this->editorEffectMap.contains(effect))
		{
			this->CurrentEffect = this->editorEffectMap.value(effect);
		}
		switch(this->CurrentEffectMode)
		{
		case PaintBrush:
			qMRMLPaintEffect * paintEffect;
			paintEffect = qMRMLPaintEffect::SafeDownCast(this->CurrentEffect);

			//Set the paint effect label 

			paintEffect->SetPaintLabel(this->GetLabel());

			paintEffect->SetSliceWidget(this->GetSliceWidget());
			paintEffect->SetupEventsObservation();
			paintEffect->SetEditorLogic(this);
			paintEffect->SetupBrush();

			if (this->editorEffectMap[FreeDraw]->IsObserving())
			{
				this->editorEffectMap[FreeDraw]->RemoveEventsObservation();
			}

			if (this->editorEffectMap[Threshold]->IsObserving())
			{
				this->editorEffectMap[Threshold]->RemoveEventsObservation();
			}

			break;

		case FreeDraw:

			qMRMLDrawEffect * drawEffect;
			drawEffect  = qMRMLDrawEffect::SafeDownCast(this->CurrentEffect);

			drawEffect->SetSliceWidget(this->GetSliceWidget());
			drawEffect->SetupEventsObservation();
			drawEffect->SetEditorLogic(this);
			drawEffect->SetupDraw();

			if (this->editorEffectMap[PaintBrush]->IsObserving())
			{
				this->editorEffectMap[PaintBrush]->CleanUp();

			}

			if (this->editorEffectMap[Threshold]->IsObserving())
			{
				this->editorEffectMap[Threshold]->CleanUp();
			}

			break;

		case Threshold:
			qMRMLThresholdEffect* thresholdEffect;
			thresholdEffect  = qMRMLThresholdEffect::SafeDownCast(this->CurrentEffect);

			thresholdEffect->SetSliceWidget(this->GetSliceWidget());
			thresholdEffect->SetupEventsObservation();
			thresholdEffect->SetEditorLogic(this);
			thresholdEffect->SetupThreshold();

			if (this->editorEffectMap[PaintBrush]->IsObserving())
			{
				this->editorEffectMap[PaintBrush]->CleanUp();
			}

			if (this->editorEffectMap[FreeDraw]->IsObserving())
			{
				this->editorEffectMap[FreeDraw]->CleanUp();
			}

			break;

		case None:
			{
			this->GetCurrentEffect()->CleanUp();
			this->CurrentEffect = NULL;
			this->CurrentEffectMode = None;
			}
		 
		
		}
	}
	
	
}



qMRMLEffect * qMRMLSegmentsEditorLogic::GetCurrentEffect()
{

	return	this->CurrentEffect;
}



qMRMLEffect * qMRMLSegmentsEditorLogic::GetEditorEffect(EffectMode effect)
{
	return this->editorEffectMap.value(effect);
}



vtkMRMLSliceCompositeNode * qMRMLSegmentsEditorLogic::GetCompositeNode(char * layoutName )
{

	int count = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
	for (int i = 0; i < count; i++)
	{
		vtkMRMLNode * compNode = this->GetMRMLScene()->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode");


	//	char* lname = vtkMRMLSliceCompositeNode::SafeDownCast(compNode)->GetLayoutName();

	//	bool re = strcmp(lname, layoutName);

		if (!strcmp(vtkMRMLSliceCompositeNode::SafeDownCast(compNode)->GetLayoutName(),layoutName))
		{
			return vtkMRMLSliceCompositeNode::SafeDownCast(compNode);
		}
		
	}
}




void qMRMLSegmentsEditorLogic::SetLabelMapNodetoLayoutCompositeNode(char * layoutName, vtkMRMLLabelMapVolumeNode* labeMap)
{
	vtkMRMLSliceCompositeNode* compNode = this->GetCompositeNode(layoutName);
	if (compNode)
	{
		compNode->SetLabelVolumeID(labeMap->GetID());
	
	}

}


qMRMLSliceWidget * qMRMLSegmentsEditorLogic::GetSliceWidget(char * layoutName)
{
	qSlicerLayoutManager * layoutmanager = qSlicerApplication::application()->layoutManager();

	return layoutmanager->sliceWidget(layoutName);

}

vtkMRMLSliceLogic * qMRMLSegmentsEditorLogic::GetSliceLogic(char * layoutName)
{
	qMRMLSliceWidget * sliceWidget = this->GetSliceWidget(layoutName);
	return sliceWidget->sliceLogic();

}

vtkMRMLVolumeNode * qMRMLSegmentsEditorLogic::GetBackgroundVolume()
{
	vtkMRMLSliceCompositeNode* compNode = this->GetCompositeNode();
	if (compNode)
	{
		char* backgroundID = compNode->GetBackgroundVolumeID();
		if (backgroundID)
		{
			return vtkMRMLVolumeNode::SafeDownCast(
				this->GetMRMLScene()->GetNodeByID(backgroundID));
		}
	}

}

vtkImageData * qMRMLSegmentsEditorLogic::GetBackgroundImage()
{
	vtkMRMLVolumeNode * volumeNode = this->GetBackgroundVolume();
	if (volumeNode)
	{
		return volumeNode->GetImageData();
	}
	return NULL;

}

char * qMRMLSegmentsEditorLogic::GetBackgroundID()
{
	vtkMRMLSliceCompositeNode* compNode = this->GetCompositeNode();
	if (compNode)
	{
		return compNode->GetBackgroundVolumeID();
	}
	return NULL;

}


vtkMRMLVolumeNode * qMRMLSegmentsEditorLogic::GetLabelVolume()
{
	vtkMRMLSliceCompositeNode* compNode = this->GetCompositeNode();
	if (compNode)
	{
		char* labelID = compNode->GetLabelVolumeID();
		if (labelID)
		{
			return vtkMRMLVolumeNode::SafeDownCast(
				this->GetMRMLScene()->GetNodeByID(labelID));
		}
		return NULL;
	}
}


vtkImageData * qMRMLSegmentsEditorLogic::GetLabelImage()
{
	vtkMRMLVolumeNode * volumeNode = this->GetLabelVolume();
	if (volumeNode)
	{
		return volumeNode->GetImageData();
	}
}



char * qMRMLSegmentsEditorLogic::GetLabelID()
{
	vtkMRMLSliceCompositeNode* compNode = this->GetCompositeNode();
	if (compNode)
	{
		return compNode->GetLabelVolumeID();
	}
}


void qMRMLSegmentsEditorLogic::SetPropagateMode(int Mode)
{
	this->PropagationMode = Mode;
}


int qMRMLSegmentsEditorLogic::GetPropagateMode()
{
	return this->PropagationMode;

}

void qMRMLSegmentsEditorLogic::SetActiveVolumes(const char * masterVolume, const char * mergeVolume)
{
	vtkMRMLVolumeNode * masterNode=NULL, *mergeNode=NULL;
	if (masterVolume)
	{
		masterNode = vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(masterVolume));

	}
	if (mergeVolume)
	{
		mergeNode = vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(mergeVolume));

	}

	vtkMRMLSelectionNode * selectionNode = this->GetMRMLApplicationLogic()->GetSelectionNode();

	selectionNode->SetReferenceActiveVolumeID(masterNode->GetID());

	if (mergeNode)
	{
		selectionNode->SetReferenceActiveLabelVolumeID(mergeNode->GetID());
	}

	this->PropagateVolumeSelection();
}

void qMRMLSegmentsEditorLogic::PropagateVolumeSelection()
{
	vtkSlicerApplicationLogic::SafeDownCast(this->GetMRMLApplicationLogic())->PropagateVolumeSelection(this->GetPropagateMode(),0);

}

int qMRMLSegmentsEditorLogic::GetLabel()
{
	return this->CurrentLable;
}


void qMRMLSegmentsEditorLogic::SetLabel(int label)
{
	this->CurrentLable = label;
}

void qMRMLSegmentsEditorLogic::BackupLabel()
{
 
	this->StoredLabel = this->CurrentLable; 

}

void qMRMLSegmentsEditorLogic::RestoreLabel()
{
	if (this->StoredLabel)
	{
		this->CurrentLable = this->StoredLabel;
	}
	

}

void qMRMLSegmentsEditorLogic::ToggleLabel()
{

}


bool qMRMLSegmentsEditorLogic::IsEraseEffectEnabled()
{
	return this->CurrentLable == 0;


}
void qMRMLSegmentsEditorLogic::SetEraseEffectEnabled( bool enabled)
{
	if (enabled & !this->IsEraseEffectEnabled())
	{
		this->BackupLabel();
		this->SetLabel(0);
	}
	else if (!enabled & this->IsEraseEffectEnabled())
	{
		this->RestoreLabel();
	}
}

void qMRMLSegmentsEditorLogic::ToggleCrosshair()
{
	vtkMRMLCrosshairNode*  crosshairNode = vtkMRMLCrosshairNode::SafeDownCast(
		                   this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLSliceCompositeNode"));

	if (crosshairNode)
	{
		if (crosshairNode->GetCrosshairMode() == 0)
		{
			crosshairNode->SetCrosshairMode(1);
		}
		else
		{
			crosshairNode->SetCrosshairMode(0);
		}

	}
}

//"""Swap the foreground and background volumes for all composite nodes in the scene"""
void qMRMLSegmentsEditorLogic::ToggleForegroundBackground()
{

	int count = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
	for (int i = 0; i < count; i++)
	{
		vtkMRMLSliceCompositeNode * compNode = vtkMRMLSliceCompositeNode::SafeDownCast(
			          this->GetMRMLScene()->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode"));

		char * oldForeground = compNode->GetForegroundVolumeID();

		compNode->SetForegroundVolumeID(compNode->GetBackgroundVolumeID());
		compNode->SetBackgroundVolumeID(oldForeground);

	}

}


void qMRMLSegmentsEditorLogic::markVolumeNodeAsModified(vtkMRMLVolumeNode* volumeNode)
{
	if (volumeNode->GetImageDataConnection())
	{
		volumeNode->GetImageDataConnection()->GetProducer()->Update();
	}

	vtkPointData *pointData = volumeNode->GetImageData()->GetPointData();
	if (pointData->GetScalars())
	{
		pointData->GetScalars()->Modified();
	}
	volumeNode->GetImageData()->Modified();
	volumeNode->Modified();

}
