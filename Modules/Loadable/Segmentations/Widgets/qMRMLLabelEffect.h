/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.18 $

=========================================================================auto=*/

#ifndef __qMRMLLabelEffect_h
#define __qMRMLLabelEffect_h

// MRML includes
#include "vtkMRML.h"
#include "vtkObserverManager.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkImageSlicePaint.h"

//Editor 
#include "qMRMLSegmentsEditorLogic.h"
#include "vtkMRMLSliceLayerLogic.h"
#include "qMRMLEffect.h"


#include "qMRMLSliceWidget.h"
#include "vtkMRMLSliceLogic.h"
#include "qMRMLSliceView.h"

class vtkMRMLScene;


// VTK includes

#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <vtkActorCollection.h>
#include <vtkActor.h>
#include <vtkUnsignedLongArray.h>
#include "vtkPolyData.h"

#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
class vtkCallbackCommand;

// Slicer VTK add-on includes
#include <vtkLoggingMacros.h>

// STD includes
#include <map>
#include <string>
#include <vector>

#include "qSRPlanSegmentationsModuleWidgetsExport.h"

class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLLabelEffect : public qMRMLEffect
{
  

public:
 vtkTypeMacro(qMRMLLabelEffect,qMRMLEffect);
  //void PrintSelf(ostream& os, vtkIndent indent);



  virtual void ProcessEvent(vtkObject *caller, unsigned long event,void *callData);
 

  //clean up actors and observers
  virtual void CleanUp();

  void RotateSliceToImage();

  //maskIJKToRAS, mask are returned values
  void MakeMaskImage(vtkPolyData * polyData, vtkMatrix4x4*maskIJKToRAS, vtkImageData*mask);

  void GetIJKToRASMatrix(vtkMRMLVolumeNode* volumeNode, vtkMatrix4x4*ijkToRAS);

  void ApplyPolyMask(vtkPolyData * polyData);

  void ApplyImageMask(vtkMatrix4x4*maskIJKToRAS, vtkImageData*mask, double*bounds);

  int GetPaintLabel();
  void SetPaintLabel( int label);

  char* SliceIJKPlane();

protected:
	bool paintOver;
	int paintLabel;
	//bool usesThreshold;
	
	//instance variables used internally buffer for result of scoped editing
	vtkImageData* extractImage;   // also as extractImage 
	vtkImageSlicePaint *  painter; //used for painter

protected:


  
  qMRMLLabelEffect();
  qMRMLLabelEffect(qMRMLSliceWidget* sliceWidget);
  /// critical to have a virtual destructor!
  ~qMRMLLabelEffect();
  qMRMLLabelEffect(const qMRMLLabelEffect&);
  void operator=(const qMRMLLabelEffect&);



 };

#endif
