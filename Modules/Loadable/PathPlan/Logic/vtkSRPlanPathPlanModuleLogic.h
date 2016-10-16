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

// .NAME vtkSRPlanPathPlanModuleLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSRPlanPathPlanModuleLogic_h
#define __vtkSRPlanPathPlanModuleLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

#include "vtkMRMLSelectionNode.h"
#include "vtkSlicerMarkupsLogic.h"
#include "vtkSRPlanBDoseCalculateLogic.h"
#include "vtkSlicerIsodoseLogic.h"
#include "vtkSlicerDoseVolumeHistogramLogic.h"


// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSRPlanPathPlanModuleLogicExport.h"

class vtkMRMLMarkupsNode;
class vtkMRMLMarkupsDisplayNode;

/// \ingroup Slicer_QtModules_Markups
class VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT vtkSRPlanPathPlanModuleLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSRPlanPathPlanModuleLogic *New();
  vtkTypeMacro(vtkSRPlanPathPlanModuleLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);


  void SetMarkupsLogic(vtkSlicerMarkupsLogic * markupsLogic);

  //Get the SelectionNode of MRML Scene
  vtkMRMLSelectionNode * GetSelectionNode();
  //Get the SelectionNode of MRML Scene
  void SetSelectionNode(vtkMRMLSelectionNode * selectionNode);



  vtkSlicerMarkupsLogic * GetMarkupsLogic();
  vtkSRPlanBDoseCalculateLogic * GetBDoseCalculateLogic();

  vtkSlicerIsodoseLogic * GetISODoseLogic();

  vtkSlicerDoseVolumeHistogramLogic * GetDVHLogic();


protected:
  vtkSRPlanPathPlanModuleLogic();
  virtual ~vtkSRPlanPathPlanModuleLogic();

  /// Initialize listening to MRML events
  virtual void SetMRMLSceneInternal(vtkMRMLScene * newScene);


private:

  vtkSRPlanPathPlanModuleLogic(const vtkSRPlanPathPlanModuleLogic&); // Not implemented
  void operator=(const vtkSRPlanPathPlanModuleLogic&);               // Not implemented

  vtkMRMLSelectionNode * selectionNode;
  
  vtkSmartPointer<vtkSlicerMarkupsLogic> MarkupsLogic;

  vtkSmartPointer<vtkSRPlanBDoseCalculateLogic> BDoseCalLogic;

  vtkSmartPointer<vtkSlicerIsodoseLogic> IsoDoseLogic;
  
  vtkSmartPointer<vtkSlicerDoseVolumeHistogramLogic> DVHLogic;
};

#endif
