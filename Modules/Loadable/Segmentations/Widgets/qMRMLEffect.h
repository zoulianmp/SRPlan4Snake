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

class vtkMRMLScene;


// VTK includes
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

class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLEffect : public vtkObject
{
  

public:
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


public:


	ScopeOption EffectOption;






protected:


  
  qMRMLEffect();
  /// critical to have a virtual destructor!
  ~qMRMLEffect();
  qMRMLEffect(const qMRMLEffect&);
  void operator=(const qMRMLEffect&);


 };

#endif
