/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.18 $

=========================================================================auto=*/

#ifndef __qMRMLEffect_h
#define __qMRMLEffect_h

// MRML includes
#include "vtkMRML.h"
#include "vtkObserverManager.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkImageSlicePaint.h"

//Editor 
#include "qMRMLSegmentsEditorLogic.h"
#include "vtkMRMLSliceLayerLogic.h"


#include "qMRMLSliceWidget.h"
#include "vtkMRMLSliceLogic.h"
#include "qMRMLSliceView.h"

class vtkMRMLScene;


// VTK includes

#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <vtkActor2DCollection.h>
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

#include "vtkSRPlanSegmentationsModuleLogicExport.h"


//Usage of current Class
//
//The Effect to be success, you need have Presetup order
//Setup 01  void SetSliceWidget(qMRMLSliceWidget* sliceWidget);  
//Setup 02  void SetupEventsObservation(); 
//Setup 03  void SetEditorLogic(qMRMLSegmentsEditorLogic* editorLogic);  


class VTK_SRPlan_SEGMENTATIONS_LOGIC_EXPORT qMRMLEffect : public vtkObject
{
  

public:
	static qMRMLEffect *New();
    vtkTypeMacro(qMRMLEffect,vtkObject);
   //void PrintSelf(ostream& os, vtkIndent indent);



/* ScopeOption Type
options for operating only on a portion of the input volume
('All' meaning the full volume or 'Visible' meaning defined by 
the current slice node are supported)
if a subclass provides a list of scope options then a selection menu will be provided. */
  
  enum ScopeOption
    {
		All,  
        Visible
    };

  //The Effect to be success, you need have Presetup order

  void SetSliceWidget(qMRMLSliceWidget* sliceWidget);  //#Setup 01
  virtual void SetupEventsObservation();  //#Setup 02 
  virtual void RemoveEventsObservation();

  void SetEditorLogic(qMRMLSegmentsEditorLogic* editorLogic);  //#Setup 03

  static void EffectEventCallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  virtual void ProcessEvent(vtkObject *caller, unsigned long event,void *callData);


  void AbortEvent(unsigned long event);

  void CursorOff();
  void CursorOn();

  //clean up actors and observers
  virtual void CleanUp();

  void RASToXY(float* rasPoint, float * xy);
  void RASToXYZ(float* rasPoint, float *xyz);

  void XYToRAS(float* xyPoint, float * ras);
  void LayerXYToIJK(vtkMRMLSliceLayerLogic * layerLogic, float * xyPoint, int * IJK);
  void BackgroundXYToIJK(float* xyPoint, int * IJK);
  void LabelXYToIJK(float * xyPoint, int * IJK);
  int  LabelAtXY(float * xyPoint);
  void XYZToRAS(float *xyzPoint, float * ras);

  float * GetPaintColor(); //get the rgba
  vtkImageData* GetScopedLayer(vtkMRMLSliceLayerLogic * layerLogic);

  vtkImageData* GetScopedBackground();

  vtkImageData*  GetScopedLabelInput();

  vtkImageData*  GetScopedLabelOutput();

  void ApplyScopedLabel();

  //Corners' order: TopLeft(ijkCorners[0]),TopRight(ijkCorners[1]),BottomLeft(ijkCorners[2]),BottomRight(ijkCorners[3])
  void GetVisibleCorners(vtkMRMLSliceLayerLogic * layerLogic, QList<int*> * out_ijkCorners );

  bool IsObserving();


public:


	ScopeOption effectScope;






protected:


  
  qMRMLEffect();

  /// critical to have a virtual destructor!
  ~qMRMLEffect();
  qMRMLEffect(const qMRMLEffect&);
  void operator=(const qMRMLEffect&);



  qMRMLSliceWidget* sliceWidget;
  vtkMRMLSliceLogic* sliceLogic;
  qMRMLSliceView * sliceView;

  vtkRenderer * renderer;
  vtkRenderWindow * renderWindow;
  vtkRenderWindowInteractor * interactor;

  qMRMLSegmentsEditorLogic* editorLogic;

  vtkActor2DCollection * actors;

  char * actionState;

  // indication of events observation satuts
  bool observing;
  
  vtkUnsignedLongArray * interactorObserverTags;

  vtkUnsignedLongArray * sliceNodeTags;

  // the events set , effect needs to processe
  vtkIntArray *events;

  QCursor savedCursor;

  //instance variables used internally buffer for result of scoped editing
  vtkImageData* scopedImageBuffer;   // also as extractImage 
  vtkImageSlicePaint * scopedSlicePaint; //used for painter

 };

#endif
