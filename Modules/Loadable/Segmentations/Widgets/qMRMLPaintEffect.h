/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  PaintEffect is a subclass of LabelEffect
  that implements the interactive paintbrush tool
  in the slicer editor


=========================================================================auto=*/

#ifndef __qMRMLPaintEffect_h
#define __qMRMLPaintEffect_h

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

#include <vtkPoints2D.h>
#include "vtkActor2D.h"


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

class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT  qMRMLPaintEffect: public qMRMLLabelEffect
{
  

public:
  vtkTypeMacro(qMRMLPaintEffect,qMRMLLabelEffect);
  //void PrintSelf(ostream& os, vtkIndent indent);



  virtual void ProcessEvent(vtkObject *caller, unsigned long event,void *callData);
 

  //clean up actors and observers
  virtual void CleanUp();
  enum BrushType { Square, Circle, Sphere, Box };
  

  void CreateGlyph(vtkPolyData * brush);
  void ScaleBrushSize(double scaleFactor);

  void PaintAddPoint(int x, int y);


  void PaintFeedback();
  void PaintApply();
  void PaintBrush(double x, double y);
  void PaintPixel(double x, double y);

  void PositionActors();
public:





protected:
	//the size of brush, for square and circle. diameter.
	int brushSize;
	BrushType shape;

	bool delayedPaint;
	bool pixelMode;

	double * position;
	vtkActor2DCollection * feedbackActors;
	vtkPoints2D * paintCoordinates;

	vtkPolyData* brush;
	vtkActor2D * brushActor;

	//rasToXY;

  
  qMRMLPaintEffect();
  qMRMLPaintEffect(qMRMLSliceWidget* sliceWidget);
  /// critical to have a virtual destructor!
  ~qMRMLPaintEffect();
  qMRMLPaintEffect(const qMRMLPaintEffect&);
  void operator=(const qMRMLPaintEffect&);



 };

#endif
