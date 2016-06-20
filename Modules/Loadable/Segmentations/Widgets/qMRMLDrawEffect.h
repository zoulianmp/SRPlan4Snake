/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.18 $


  """
  One instance of this will be created per-view when the effect
  is selected.  It is responsible for implementing feedback and
  label map changes in response to user input.
  This class observes the editor parameter node to configure itself
  and queries the current view for background and label volume
  nodes to operate on.
=========================================================================auto=*/

#ifndef __qMRMLDrawEffect_h
#define __qMRMLDrawEffect_h

// MRML includes
#include "vtkMRML.h"
#include "vtkObserverManager.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkImageSlicePaint.h"

//Editor 
#include "qMRMLSegmentsEditorLogic.h"
#include "vtkMRMLSliceLayerLogic.h"
#include "qMRMLLabelEffect.h"


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

class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT  qMRMLDrawEffect: public qMRMLLabelEffect
{
  

public:
  vtkTypeMacro(qMRMLDrawEffect,qMRMLLabelEffect);
  //void PrintSelf(ostream& os, vtkIndent indent);



  virtual void ProcessEvent(vtkObject *caller, unsigned long event,void *callData);
 

  //clean up actors and observers
  virtual void CleanUp();

  


public:

	vtkPolyData* CreatePolyData();
	void SetLineMode(char* mode);
	void AddPoint(float * ras);
	void DeleteLastPoint();
	void Apply();
	void PositionActors();
	void ResetPolyData();

protected:

	//# keep a flag since events such as sliceNode modified
	//# may come during superclass construction, which will
	//# invoke our processEvents method
	bool initialized; 
  
	double activeSlice ;
	unsigned long lastInsertSliceNodeMTime = NULL;
	char* actionState = "";  //"drawing","",


	vtkPoints * xyPoints;
	vtkPoints * rasPoints;
	vtkPolyData * polyData;

	vtkActor2D * actor;

    qMRMLDrawEffect();
    qMRMLDrawEffect(qMRMLSliceWidget* sliceWidget);
    /// critical to have a virtual destructor!
    ~qMRMLDrawEffect();
    qMRMLDrawEffect(const qMRMLDrawEffect&);
    void operator=(const qMRMLDrawEffect&);



 };

#endif
