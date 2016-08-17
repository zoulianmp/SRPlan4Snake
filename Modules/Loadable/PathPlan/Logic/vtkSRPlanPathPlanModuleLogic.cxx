/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Markups includes
#include "vtkSRPlanPathPlanModuleLogic.h"

// Markups MRML includes
#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsFiducialStorageNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLMarkupsStorageNode.h"

// Annotation MRML includes
#include "vtkMRMLAnnotationFiducialNode.h"
#include "vtkMRMLAnnotationPointDisplayNode.h"
#include "vtkMRMLAnnotationTextDisplayNode.h"

// MRML includes
#include "vtkMRMLCameraNode.h"
#include "vtkMRMLHierarchyNode.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLSliceCompositeNode.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLSceneViewNode.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkStringArray.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSRPlanPathPlanModuleLogic);

//----------------------------------------------------------------------------
vtkSRPlanPathPlanModuleLogic::vtkSRPlanPathPlanModuleLogic()
{
	this->MarkupsLogic = vtkSmartPointer<vtkSlicerMarkupsLogic>::New();
	

}

//----------------------------------------------------------------------------
vtkSRPlanPathPlanModuleLogic::~vtkSRPlanPathPlanModuleLogic()
{
 
}

//----------------------------------------------------------------------------
void vtkSRPlanPathPlanModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
void vtkSRPlanPathPlanModuleLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
 
	this->MarkupsLogic->SetMRMLScene(newScene);
	  
	
}



//---------------------------------------------------------------------------



void vtkSRPlanPathPlanModuleLogic::SetMarkupsLogic(vtkSlicerMarkupsLogic * markupsLogic)
{



}

vtkSlicerMarkupsLogic * vtkSRPlanPathPlanModuleLogic::GetMarkupsLogic()
{
	return this->MarkupsLogic.GetPointer();
 
}