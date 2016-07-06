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
#include "qMRMLLabelEffect.h"
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
#include "vtkImageFillROI.h"
#include "vtkTransform.h"
#include "vtkMRMLTransformNode.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


vtkStandardNewMacro(qMRMLLabelEffect);

qMRMLLabelEffect::qMRMLLabelEffect()
{
	this->paintOver =true;
	this->paintLabel = 0;
	//bool usesThreshold = true;
	this->extractImage = 0;
	this->painter = vtkImageSlicePaint::New();
  
}



qMRMLLabelEffect::~qMRMLLabelEffect()
{
	this->actors->Delete();

} 

int qMRMLLabelEffect::GetPaintLabel()
{
	return this->paintLabel;
}

void qMRMLLabelEffect::SetPaintLabel(int label)
{
	this->paintLabel = label;

}

bool qMRMLLabelEffect::GetPaintOver()
{
	return this->paintOver;

}

void qMRMLLabelEffect::SetPaintOver()
{
	this->paintOver = true;
}
void qMRMLLabelEffect::UnsetPaintOver()
{
	this->paintOver = false;
}



void qMRMLLabelEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
{
	Superclass::ProcessEvent(caller, event, callData);

}



//clean up actors and observers
void qMRMLLabelEffect::CleanUp()
{
	Superclass::CleanUp();
}


//adjusts the slice node to align with the native space of the image data so that paint
//operations can happen cleanly between image space and screen space 
void qMRMLLabelEffect::RotateSliceToImage()
{
     vtkMRMLSliceLogic * sliceLogic = this->sliceWidget->sliceLogic();
	 vtkMRMLSliceNode * sliceNode = this->sliceWidget->mrmlSliceNode();
	 vtkMRMLVolumeNode * volumeNode = sliceLogic->GetBackgroundLayer()->GetVolumeNode();

	 sliceNode->RotateToVolumePlane(volumeNode);
	
	 sliceLogic->SnapSliceOffsetToIJK();
	 sliceNode->UpdateMatrices();

}

// Create a screen space (2D) mask image for the given
//    polydata.

//		Need to know the mapping from RAS into polygon space
//		so the painter can use this as a mask
//		- need the bounds in RAS space
//		- need to get an IJKToRAS for just the mask area
//		- directions are the XYToRAS for this slice
//		- origin is the lower left of the polygon bounds
//
////vtkMatrix4x4 * maskIJKToRAS = vtkMatrix4x4::New(); initialized value
void qMRMLLabelEffect::MakeMaskImage(vtkPolyData * polyData, vtkMatrix4x4*maskIJKToRAS, vtkImageData*mask)
{
	vtkMRMLSliceLayerLogic *  labelLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

	//vtkMatrix4x4 * maskIJKToRAS = vtkMatrix4x4::New();

	maskIJKToRAS->DeepCopy(sliceNode->GetXYToRAS());
	polyData->GetPoints()->Modified();

	double *bounds = polyData->GetBounds();

	double xlo, xhi, ylo, yhi;
	xlo = bounds[0] - 1;
	xhi = bounds[1];
	ylo = bounds[2] - 1;
	yhi = bounds[3];



	float xy[2] = { float(xlo) ,float(ylo) };
	float originRAS[3];

	this->XYToRAS(xy, originRAS);

	maskIJKToRAS->SetElement(0, 3, originRAS[0]);
	maskIJKToRAS->SetElement(1, 3, originRAS[1]);
	maskIJKToRAS->SetElement(2, 3, originRAS[2]);

	// get a good size for the draw buffer
    //needs to include the full region of the polygon plus a little extra
    //round to int and add extra pixel for both sides
    //: figure out why we need to add buffer pixels on each
	//	#    side for the width in order to end up with a single extra
	//	#    pixel in the rasterized image map.Probably has to
	//	#    do with how boundary conditions are handled in the filler

	int w = int(xhi - xlo) + 32;
	int h = int(yhi - ylo) + 32;

	vtkImageData * imageData = vtkImageData::New();
	imageData->SetDimensions(w, h, 1);

	vtkMRMLVolumeNode * labelNode = labelLogic->GetVolumeNode();
	
	if (!labelNode) return;

	vtkImageData * labelImage = labelNode->GetImageData();

	if (!labelImage) return;

	imageData->AllocateScalars(labelImage->GetScalarType(), 1);

	vtkTransform * translate = vtkTransform::New();

	translate->Translate(-1.0*xlo,-1.0*ylo,0);

	vtkPoints * drawPoints = vtkPoints::New();

	drawPoints->Reset();

	translate->TransformPoints(polyData->GetPoints(), drawPoints);
	drawPoints->Modified();

	vtkImageFillROI* fill = vtkImageFillROI::New();
	fill->SetInputData(imageData);
	fill->SetValue(1);
	fill->SetPoints(drawPoints);
	fill->Update();

	//vtkImageData* mask = vtkImageData::New();
	mask->DeepCopy(fill->GetOutput());
	
}

void qMRMLLabelEffect::GetIJKToRASMatrix(vtkMRMLVolumeNode* volumeNode, vtkMatrix4x4*ijkToRAS)
{
	volumeNode->GetIJKToRASMatrix(ijkToRAS);
	vtkMRMLTransformNode* transformNode = volumeNode->GetParentTransformNode();
	if (transformNode)
	{
		if (transformNode->IsTransformToWorldLinear())
		{
			vtkMatrix4x4* rasToRAS = vtkMatrix4x4::New();
			transformNode->GetMatrixTransformToWorld(rasToRAS);
			rasToRAS->Multiply4x4(rasToRAS, ijkToRAS, ijkToRAS);
		}
		else
		{
			cout << "Cannot handle non - linear transforms - skipping";
		}
	}
	
}


void qMRMLLabelEffect::ApplyPolyMask(vtkPolyData * polyData)
{
	vtkMRMLSliceLayerLogic *  labelLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

	vtkMRMLVolumeNode * labelNode = labelLogic->GetVolumeNode();
	if (!sliceNode || !labelNode) return;

	vtkMatrix4x4*maskIJKToRAS = vtkMatrix4x4::New();
	vtkImageData*mask = vtkImageData::New();

	this->MakeMaskImage(polyData, maskIJKToRAS, mask);

	polyData->GetPoints()->Modified();
	double * bounds = polyData->GetBounds();

	this->ApplyImageMask(maskIJKToRAS, mask, bounds);

}

//apply a pre - rasterized image to the current label layer
//- maskIJKToRAS tells the mapping from image pixels to RAS
//- mask is a vtkImageData
//- bounds are the xy extents of the mask(zlo and zhi ignored)
void qMRMLLabelEffect::ApplyImageMask(vtkMatrix4x4*maskIJKToRAS, vtkImageData*mask, double*bounds)
{
	vtkMRMLSliceLayerLogic *  backgroundLogic = this->sliceLogic->GetBackgroundLayer();
	vtkMRMLVolumeNode *backgroundNode = backgroundLogic->GetVolumeNode();

	if (!backgroundNode) return;
	vtkImageData *  backgroundImage = backgroundNode->GetImageData();
	if (!backgroundImage) return;
	vtkMRMLSliceLayerLogic* labelLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLVolumeNode *	labelNode = labelLogic->GetVolumeNode();
	if (!labelNode) return;

	vtkImageData * labelImage = labelNode->GetImageData();
	if (!labelImage) return;


	//# get the mask bounding box in ijk coordinates
    //# - get the xy bounds
    //# - transform to ijk
    //# - clamp the bounds to the dimensions of the label image		
	double xlo = bounds[0];
	double xhi = bounds[1];
	double ylo = bounds[2];
	double yhi = bounds[3];
	double zlo = bounds[4];
	double zhi = bounds[5];

		
	vtkGeneralTransform * xyToIJK = labelLogic->GetXYToIJKTransform();
	double * tlIJK = xyToIJK->TransformDoublePoint((xlo, yhi, 0));
	double * trIJK = xyToIJK->TransformDoublePoint((xhi, yhi, 0));
	double * blIJK = xyToIJK->TransformDoublePoint((xlo, ylo, 0));
	double * brIJK = xyToIJK->TransformDoublePoint((xhi, ylo, 0));

	QList<double*> cornersIJK;

	cornersIJK.append(tlIJK);
	cornersIJK.append(trIJK);
	cornersIJK.append(blIJK);
	cornersIJK.append(brIJK);

	//# do the clamping of the four corners
	int * dims = labelImage->GetDimensions();
	int	tl[3] = { 0,0,0 };
	int	tr[3] = { 0,0,0 };
	int	bl[3] = { 0,0,0 };
	int	br[3] = { 0,0,0 };
    
	QList<int*> cornersClamped;

	cornersClamped.append(tl);
	cornersClamped.append(tr);
	cornersClamped.append(bl);
	cornersClamped.append(br);

	int clamped;

	for (int i = 0; i < 4; i++)
	{
		double * corner = cornersIJK.value(i);
		int * clampedcorner = cornersClamped.value(i);

		for (int j = 0; j < 3; j++)
		{
			clamped = int(round(corner[j]));
			if (clamped < 0) clamped = 0;
			if (clamped >= dims[j]) clamped = dims[j] - 1;

			clampedcorner[j] = clamped;

		}
	}

	vtkMatrix4x4*  backgroundIJKToRAS = vtkMatrix4x4::New();
	vtkMatrix4x4*  labelIJKToRAS = vtkMatrix4x4::New();

	this->GetIJKToRASMatrix(backgroundNode, backgroundIJKToRAS);
	this->GetIJKToRASMatrix(labelNode, labelIJKToRAS);
    
	this->extractImage = vtkImageData::New();

	//# set up the painter class and let 'r rip!

	this->painter->SetBackgroundImage(backgroundImage);
	this->painter->SetBackgroundIJKToWorld(backgroundIJKToRAS);
	this->painter->SetWorkingImage(labelImage);
	this->painter->SetWorkingIJKToWorld(labelIJKToRAS);
	this->painter->SetMaskImage(mask);
	this->painter->SetExtractImage(this->extractImage);
	this->painter->SetReplaceImage(NULL);
	this->painter->SetMaskIJKToWorld(maskIJKToRAS);
	this->painter->SetTopLeft(tl);
	this->painter->SetTopRight(tr);
	this->painter->SetBottomLeft(bl);
	this->painter->SetBottomRight(br);

	this->painter->SetPaintLabel(this->paintLabel);
	this->painter->SetPaintOver(this->paintOver);

	this->painter->Paint();

	this->editorLogic->markVolumeNodeAsModified(labelNode);

}


//Return a code indicating which plane of IJK space corresponds to the current slice plane orientation.
//Values are 'IJ', 'IK', 'JK', or None.
//This is useful for algorithms like LevelTracing that operate in pixel space.

char* qMRMLLabelEffect::SliceIJKPlane()
{
	int * dims = this->sliceLogic->GetSliceNode()->GetDimensions();
	int mid = std::max(dims[0],dims[1] );
	int offset = std::max( mid, dims[2]);


	float xyPoint0[2] = { 0.0,0.0 };
	float xyPoint1[2] = { float(offset),float(offset) };
	int *IJK0, *IJK1;

	this->BackgroundXYToIJK(xyPoint0, IJK0);
	this->BackgroundXYToIJK(xyPoint1, IJK1);

	if (IJK0[0] == IJK1[0] && IJK0[1] == IJK1[1] && IJK0[2] == IJK1[2])
	{
		return "";
	}
	else if (IJK0[0] == IJK1[0])
	{
		return "JK";
	}
	else if (IJK0[1] == IJK1[1])
	{
		return "IK";
	}
	else if (IJK0[2] == IJK1[2])
	{
		return "IJ";
	}

	return "";


}