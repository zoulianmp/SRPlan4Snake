/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLScene.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.18 $

=========================================================================auto=*/


#include "vtkMRMLSceneUtility.h"


#include "vtkMRMLScene.h"


// added by zoulian
#include "vtkMRMLPatientInfoNode.h"
#include "vtkMRMLGeneralParametersNode.h"

#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>


vtkStandardNewMacro(vtkMRMLSceneUtility);

//------------------------------------------------------------------------------
vtkMRMLSceneUtility::vtkMRMLSceneUtility()
{
  
}

//------------------------------------------------------------------------------
vtkMRMLSceneUtility::~vtkMRMLSceneUtility()
{
  
}




vtkMRMLGeneralParametersNode * vtkMRMLSceneUtility::CreateParametersNode(vtkMRMLScene* scene)
{
	vtkMRMLGeneralParametersNode* parametersNode = vtkMRMLGeneralParametersNode::New();
	parametersNode->SetSingletonTag("SRPlan");
	parametersNode->SetModuleName("SRPlan");

	parametersNode->SetParameter("label", "0"); //current label value	
	parametersNode->SetParameter("effect", "None"); //current enum EffectMode { None, PaintBrush, FreeDraw, Threshold };
	parametersNode->SetParameter("segmentation", ""); //the current vtkMRMLSegmentationNode uid, Used to get segmentation from scence.
	parametersNode->SetParameter("segment", ""); //segment current id for get segment from current segmentation Node

	parametersNode->SetParameter("mergedLabelmap", ""); //the current mergedLabelmapNode for vtkMRMLSegmentationNode uid, Used to get segmentation from scence.
	parametersNode->SetParameter("segmentLabelmap", ""); //the current segmentLabelmap uid for a segment, Used to for effect do

	parametersNode->SetParameter("LabelmapColorTableNode", ""); //the current CorlorTableNode uid in scene, us it to get the ColorTableNode.

	parametersNode->SetParameter("PrimaryPlanVolumeNodeID", ""); //the ScalarVolumeNode as a primary plan volume

	parametersNode->SetParameter("SnakeHeadDirectionX", "1.0"); //the SnakeHead Direction X component.
	parametersNode->SetParameter("SnakeHeadDirectionY", "0.0"); //the SnakeHead Direction Y component.
	parametersNode->SetParameter("SnakeHeadDirectionZ", "0.0"); //the SnakeHead Direction Z component.

	parametersNode->SetParameter("SnakeHeadOpacity", "0.6"); //the SnakeHead Opacity


	//	parametersNode->SetParameter("propagationMode", str(slicer.vtkMRMLApplicationLogic.BackgroundLayer | slicer.vtkMRMLApplicationLogic.LabelLayer))
	scene->AddNode(parametersNode);

	return parametersNode;

}


vtkMRMLGeneralParametersNode* vtkMRMLSceneUtility::GetParametersNode(vtkMRMLScene* scene)
{
	vtkMRMLGeneralParametersNode* Node = NULL;
	int size = scene->GetNumberOfNodesByClass("vtkMRMLGeneralParametersNode");

	// if the parametersNode is not exist, create it
	if (size == 0)
	{
		return vtkMRMLSceneUtility::CreateParametersNode(scene);
	}

	// Iteration over the pool
	for (int i=0; i < size; i++)
	{
		
		Node = vtkMRMLGeneralParametersNode::SafeDownCast(scene->GetNthNodeByClass(i, "vtkMRMLGeneralParametersNode"));
		if (!strcmp(Node->GetModuleName(), "SRPlan") && !strcmp(Node->GetSingletonTag(), "SRPlan"))
		{
			return Node;
		}

	}

	// If there are parameters node ,but not SRPlan
	return vtkMRMLSceneUtility::CreateParametersNode(scene);

}


  
vtkMRMLSliceCompositeNode * vtkMRMLSceneUtility::GetCompositeNode(char * layoutName,vtkMRMLScene* scene )
{

	int count = scene->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
	for (int i = 0; i < count; i++)
	{
		vtkMRMLNode * compNode = scene->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode");


	//	char* lname = vtkMRMLSliceCompositeNode::SafeDownCast(compNode)->GetLayoutName();

	//	bool re = strcmp(lname, layoutName);

		if (!strcmp(vtkMRMLSliceCompositeNode::SafeDownCast(compNode)->GetLayoutName(),layoutName))
		{
			return vtkMRMLSliceCompositeNode::SafeDownCast(compNode);
		}
	}

	return NULL;
}


vtkMRMLSelectionNode * vtkMRMLSceneUtility::GetSelectionNode(vtkMRMLScene* scene)
{
   return vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
}

  
