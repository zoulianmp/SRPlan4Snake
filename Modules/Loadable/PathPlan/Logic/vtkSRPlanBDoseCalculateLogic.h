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
#include "vtkMRMLSelectionNode.h"

#include "vtkMRMLSliceCompositeNode.h"

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

	//Get the SelectionNode of MRML Scene
	vtkMRMLSelectionNode * GetSelectionNode();
	//Get the SelectionNode of MRML Scene
	void SetSelectionNode(vtkMRMLSelectionNode * selectionNode);


	//Set the Primary Plan Volume
	void SetPlanPrimaryVolumeNode(vtkMRMLScalarVolumeNode * primary);
	vtkMRMLScalarVolumeNode * GetPlanPrimaryVolumeNode();


	void SetSnakePlanPath(vtkMRMLMarkupsNode * snakePath);
	vtkMRMLMarkupsNode * GetSnakePlanPath();

	void SetDoseCalculateGridSize(double gridSize);

	//The length in mm,to determine the range of seed source dose distribution
	void SetDoseCalculateCutoff(double cutoff);

	void StartDoseCalcualte();

	//Normalize the Dose Grid to Maximum,Get the Relative distribution
	void NormalizedToMaximum(vtkMRMLScalarVolumeNode * absDoseVolume);

	void PrepareIr192SeedKernal();

	void DoseSuperposition(vtkMRMLMarkupsNode * snakePath , vtkImageData * doseKernal);

	vtkMRMLScalarVolumeNode * GetCalculatedDoseVolume();

	//Initialize the empty Dose Grid Node,prepare for Dose Calculation
	//as vtkMRMLScalarVolumeNode * doseVolume; //The calculated Dosevolume Node
	void InitializeEmptyDosGridNode();
	
	void PrintROIDose(vtkImageData * data, int * extent);
  
	void InvalidDoseAndRemoveDoseVolumeNodeFromScene();


	//Set the Dose Node to Layout CompositNode, R,Y,G
	void  SetDoseNodetoLayoutCompositeNode(char * layoutName, vtkMRMLScalarVolumeNode * absDoseVolume);

protected:
	//Create a empty IJK ImageData, Origin(0,0,0),spacing(1,1,1)
	vtkImageData * CreateEmptyDoseGrid(int * dims);

	//Givent a RAS Point, return the IJK Index
    void GetIJKFromRASPostion(vtkMRMLScalarVolumeNode * VolumeNode, double * rasPosition, int * IJK);

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

	vtkMRMLSelectionNode * selectionNode;

	double m_gridSize; // dose voxel length unit : mm 

	double m_cutoff; //Using the cutoff value in mm unit, to define the dose kernal 


};

#endif

