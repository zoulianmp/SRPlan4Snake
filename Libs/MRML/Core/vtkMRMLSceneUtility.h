/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLScene.h,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.18 $

=========================================================================auto=*/

#ifndef __vtkMRMLSceneUtility_h
#define __vtkMRMLSceneUtility_h

/// Current MRML version
/// needs to be changed when incompatible MRML changes introduced
#define CURRENT_MRML_VERSION "Slicer4.4.0"

// MRML includes
#include "vtkMRML.h"

#include "vtkMRMLScene.h"

#include "vtkMRMLSliceCompositeNode.h"
#include "vtkMRMLGeneralParametersNode.h"
#include "vtkMRMLSelectionNode.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

/// \brief A Scene Utility  class, for SRPlan4Snake
//

class VTK_MRML_EXPORT vtkMRMLSceneUtility : public vtkObject
{
 

public:
  static vtkMRMLSceneUtility *New();
  vtkTypeMacro(vtkMRMLSceneUtility, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

 //Creat a parameters node for segmentation module
  static void CreateParametersNode(vtkMRMLScene* scene);

  //Get The ParametersNode of the Scene
  static vtkMRMLGeneralParametersNode*  GetParametersNode(vtkMRMLScene* scene);
  
  //Get the Composite Node of layoutname in the scene
  static  vtkMRMLSliceCompositeNode *  GetCompositeNode(char * layoutName, vtkMRMLScene* scene );
   
  //Get the Gloable vtkMRMLSelectionNode from a  scene
  static vtkMRMLSelectionNode * GetSelectionNode(vtkMRMLScene* scene);

	
  
protected:


  vtkMRMLSceneUtility();
  virtual ~vtkMRMLSceneUtility();


private:

  vtkMRMLSceneUtility(const vtkMRMLSceneUtility&);   // Not implemented
  void operator=(const vtkMRMLSceneUtility&); // Not implemented

 
};

#endif
