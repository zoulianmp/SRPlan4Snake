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

#include "vtkSRPlanConfigure.h"

// Isodose includes
#include "vtkSRPlanBDoseCalculateLogic.h"
#include "vtkMRMLIsodoseNode.h"

// Subject Hierarchy includes
#include "vtkMRMLSubjectHierarchyConstants.h"
#include "vtkMRMLSubjectHierarchyNode.h"
#include "vtkSlicerSubjectHierarchyModuleLogic.h"

// SlicerRT includes
#include "SlicerRtCommon.h"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumeDisplayNode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLTransformNode.h>

// MRMLLogic includes
#include <vtkMRMLColorLogic.h>
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLScene.h>
#include "vtkSlicerVolumesLogic.h"




// VTK includes
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkImageMarchingCubes.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageReslice.h>
#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkGeneralTransform.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkObjectFactory.h>
#include "vtksys/SystemTools.hxx"



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSRPlanBDoseCalculateLogic);

//----------------------------------------------------------------------------
vtkSRPlanBDoseCalculateLogic::vtkSRPlanBDoseCalculateLogic()
{
	this->m_gridSize = 2.5 ; // dose voxel length unit : mm 
	this->m_cutoff = 50; //Cutoff Distance from center unit:mm
 
	this->planPrimaryVolume = NULL; //The Primary Image Volume for RT Plan 

	this->snakePath = NULL; //the Snake Path
    this-> Ir192Seed =NULL;
    this->doseVolume = NULL; //The
}

//----------------------------------------------------------------------------
vtkSRPlanBDoseCalculateLogic::~vtkSRPlanBDoseCalculateLogic()
{
  

}

//----------------------------------------------------------------------------
void vtkSRPlanBDoseCalculateLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkSRPlanBDoseCalculateLogic::SetPlanPrimaryVolumeNode(vtkMRMLScalarVolumeNode * primary)
{
	vtkSetAndObserveMRMLNodeMacro(this->planPrimaryVolume, primary);
}

vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::GetPlanPrimaryVolumeNode()
{
	return this->planPrimaryVolume;

}


void vtkSRPlanBDoseCalculateLogic::SetSnakePlanPath(vtkMRMLMarkupsNode * snakePath)
{
	vtkSetAndObserveMRMLNodeMacro(this->snakePath, snakePath);

}

vtkMRMLMarkupsNode * vtkSRPlanBDoseCalculateLogic::GetSnakePlanPath()
{
	return this->snakePath;
}


void vtkSRPlanBDoseCalculateLogic::SetDoseCalculateGridSize(double gridSize)
{
	this->m_gridSize = gridSize;

}

//Step 1
//Initialize the empty Dose Grid,prepare for Dose Calculation
void vtkSRPlanBDoseCalculateLogic::InitializeEmptyDosGridNode()
{
	if (!planPrimaryVolume)
	{
		return;
	}

	char * emptyDoseGrid = "DoseGrid";
	//Clone vtkMRMLScalarVolume without imagedata
	this->doseVolume = vtkSlicerVolumesLogic::CloneVolumeWithoutImageData(this->GetMRMLScene(), planPrimaryVolume, emptyDoseGrid);

	// Set DoseGrid spacing
	doseVolume->SetSpacing(m_gridSize, m_gridSize, m_gridSize);

	//************************************************************
	//create empty vtkImageData, Set and Observer

	double rasBounds[6];
	planPrimaryVolume->GetRASBounds(rasBounds);


	double rmin, rmax, amin, amax, smin, smax;
	rmin = rasBounds[0];
	rmax = rasBounds[1];
	amin = rasBounds[2];
	amax = rasBounds[3];
	smin = rasBounds[4];
	smax = rasBounds[5];



	double rLen, aLen, sLen;

	rLen = rasBounds[1] - rasBounds[0];

	aLen = rasBounds[3] - rasBounds[2];

	sLen = rasBounds[5] - rasBounds[4];

	//The Unit vector
	double rUnit[3] = { rLen , 0.0, 0.0};

	double aUnit[3] = { 0.0, aLen, 0.0};

	double sUnit[3] = { 0.0, 0.0, sLen};

	
	//planPrimaryVolume->GetRASToIJKMatrix()



	vtkNew<vtkGeneralTransform> transform;
	transform->PostMultiply();
	transform->Identity();


	//Refered by, PlanImage
	//vtkNew<vtkMatrix4x4> planVolumeRASToIJK;
	//planPrimaryVolume->GetRASToIJKMatrix(planVolumeRASToIJK.GetPointer());


	vtkNew<vtkMatrix4x4> doseGridRASToIJK;
	doseVolume->GetRASToIJKMatrix(doseGridRASToIJK.GetPointer());


	//******************************************
	//Clone the doseGridRASToIJK, Use the matrix get the new spacing IJK Dimensions
	vtkNew<vtkMatrix4x4> midRASToIJK;
	midRASToIJK->DeepCopy(doseGridRASToIJK.GetPointer());


	midRASToIJK->SetElement(0, 3, 0.0);
	midRASToIJK->SetElement(1, 3, 0.0);
	midRASToIJK->SetElement(2, 3, 0.0);

	transform->Concatenate(midRASToIJK.GetPointer());


	//transform->Concatenate(doseGridRASToIJK.GetPointer());

	double* vFromR, * vFromA, *vFromS;
 
	double vR[3] = { 0.0,0.0,0.0 };
	double vA[3] = { 0.0,0.0,0.0 };
	double vS[3] = { 0.0,0.0,0.0 };


	//The Empty Dose Grid IJK Dims
		
	int IJKDims[3] = {0,0,0};

	
	vFromR = transform->TransformDoublePoint(rUnit);


	vR[0] = vFromR[0];
	vR[1] = vFromR[1];
	vR[2] = vFromR[2];
	



	vFromA = transform->TransformDoublePoint(aUnit); 

	vA[0] = vFromA[0];
	vA[1] = vFromA[1];
	vA[2] = vFromA[2];
	 

		
	vFromS = transform->TransformDoublePoint(sUnit);
	vS[0] = vFromS[0];
	vS[1] = vFromS[1];
	vS[2] = vFromS[2];

	double tR, tA, tS;


	for (int i = 0; i<3; i++)
	{
		tR = vR[i];
		tA = vA[i];		
		tS = vS[i];

		if (abs(tR) > 0.0001)
			IJKDims[i] = int(abs(tR));

		if (abs(tA) > 0.0001)
			IJKDims[i] = int(abs(tA));

		if (abs(tS) > 0.0001)
			IJKDims[i] = int(abs(tS));

	}

	
	vtkImageData * doseGrid =  CreateEmptyDoseGrid(IJKDims);

  	doseVolume->SetAndObserveImageData(doseGrid );

}



//Given a Dose Grid dimensions as i,j,k ,and create empty Image Data.
vtkImageData * vtkSRPlanBDoseCalculateLogic::CreateEmptyDoseGrid(int * dims)
{

	// Create an image data
	//vtkSmartPointer<vtkImageData> imageData =
	//	vtkSmartPointer<vtkImageData>::New();

	vtkImageData *  imageData = vtkImageData::New();

	imageData->SetOrigin(0.0,0.0,0.0);
	imageData->SetSpacing(1, 1, 1);
	// Specify the size of the image data
	imageData->SetDimensions(dims[0], dims[1],dims[2]);

	imageData->AllocateScalars(VTK_DOUBLE, 1);

	// Fill every entry of the image data with x,y,z
	//int* dims = imageData->GetDimensions();

	double *ptr = static_cast<double *>(imageData->GetScalarPointer(0, 0, 0));
	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				*ptr++ = 0.0;
				
			}
		}
	}

	return imageData;

}

//The length in mm,to determine the range of seed source dose distribution
void vtkSRPlanBDoseCalculateLogic::SetDoseCalculateCutoff(double cutoff)
{
	this->m_cutoff = cutoff;
}


void vtkSRPlanBDoseCalculateLogic::StartDoseCalcualte()
{
	// 1 . Prepare a Dose Grid Volume for Dose Calculate

	if (!planPrimaryVolume || !snakePath)
		return;
	this->InitializeEmptyDosGridNode();

	// 2 . Prepare the Ir192 3D Dose Kernal

	this->PrepareIr192SeedKernal();

	// 3 . Calculate the Dose Distribution

	vtkImageData* kernal = this->Ir192Seed->GetDoseKernalVolume();

	this->DoseSuperposition(snakePath, kernal);

}

vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::GetCalculatedDoseVolume()
{
	return this->doseVolume;
}


//Step 2 , Prepare The SeedSource 
void vtkSRPlanBDoseCalculateLogic::PrepareIr192SeedKernal()
{
	this->Ir192Seed = vtkIr192SeedSource::New();
	this->Ir192Seed->SetGridSpacing(this->m_gridSize);
	this->Ir192Seed->SetDoseKernalCutoff(this->m_cutoff);

	this->Ir192Seed->SetupIr192Seed();
	this->Ir192Seed->UpdateDoseKernalVolume();

}

void vtkSRPlanBDoseCalculateLogic::DoseSuperposition(vtkMRMLMarkupsNode * snakePath, vtkImageData * doseKernal)
{
	int i = 0;

}
