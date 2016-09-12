/*==============================================================================

  Copyright (c) Radiation Medicine Program, University Health Network,
  Princess Margaret Hospital, Toronto, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kevin Wang, Princess Margaret Cancer Centre 
  and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// .NAME vtkSRPlanBDoseLogic - slicer logic class for isodose creation
// .SECTION Description
// This class manages the logic associated with creating isodose lines and
// surfaces from dose distributions


#ifndef __vtkSRPlanBDoseCalculateLogic_h
#define __vtkSRPlanBDoseCalculateLogic_h

// Slicer includes
#include "vtkMRMLAbstractLogic.h"

#include "vtkSRPlanPathPlanModuleLogicExport.h"


#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLVolumeNode.h"
#include <vtkMRMLScalarVolumeNode.h>

#include "vtkIr192SeedSource.h"
/*
vtkMRMLScene* scene = this->mrmlScene();
vtkMRMLSelectionNode * selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
selectionNode->SetActiveSegmentationID(segmentationNode->GetID());
*/



/// \ingroup SlicerRt_QtModules_Isodose
class VTK_SRPlan_PATHPLAN_MODULE_LOGIC_EXPORT vtkSRPlanBDoseCalculateLogic :
  public vtkMRMLAbstractLogic
{

 
public:
  static vtkSRPlanBDoseCalculateLogic *New();
  vtkTypeMacro(vtkSRPlanBDoseCalculateLogic, vtkMRMLAbstractLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

   

public:
	//Set the Primary Plan Volume
	void SetPlanPrimaryVolumeNode(vtkMRMLScalarVolumeNode * primary);
	vtkMRMLScalarVolumeNode * GetPlanPrimaryVolumeNode();


	void SetSnakePlanPath(vtkMRMLMarkupsNode * snakePath);
	vtkMRMLMarkupsNode * GetSnakePlanPath();

	void SetDoseCalculateGridSize(double gridSize);

	//The length in mm,to determine the range of seed source dose distribution
	void SetDoseCalculateCutoff(double cutoff);

	void StartDoseCalcualte();

	// 
	void PrepareDoseGridVolumeNode();

	void PrepareIr192SeedKernal();

	void DoseSuperposition(vtkMRMLMarkupsNode * snakePath , vtkImageData * doseKernal);

	vtkMRMLScalarVolumeNode * GetCalculatedDoseVolume();
  
protected:


protected:
	vtkSRPlanBDoseCalculateLogic();
  virtual ~vtkSRPlanBDoseCalculateLogic();

private:
	vtkSRPlanBDoseCalculateLogic(const vtkSRPlanBDoseCalculateLogic&); // Not implemented
    void operator=(const vtkSRPlanBDoseCalculateLogic&);               // Not implemented

protected:
	vtkMRMLScalarVolumeNode * planPrimaryVolume; //The Primary Image Volume for RT Plan 

	vtkMRMLMarkupsNode * snakePath; //the Snake Path

	vtkIr192SeedSource * Ir192Seed;

	vtkMRMLScalarVolumeNode * doseVolume; //The calculated Dosevolume Node



	double m_gridSize; // dose voxel length unit : mm 

	double m_cutoff; //Using the cutoff value in mm unit, to define the dose kernal 


};

#endif

