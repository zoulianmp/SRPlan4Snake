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
#include "qMRMLThresholdEffect.h"
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


qMRMLThresholdEffect::qMRMLThresholdEffect()
{
	
}



qMRMLThresholdEffect::~qMRMLThresholdEffect()
{
	

} 

qMRMLThresholdEffect::qMRMLThresholdEffect(qMRMLSliceWidget* sliceWidget)
{


}

void qMRMLThresholdEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
{
	

}



//clean up actors and observers
void qMRMLThresholdEffect::CleanUp()
{

}



