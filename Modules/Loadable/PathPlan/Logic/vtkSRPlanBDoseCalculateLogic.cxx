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
	double m_gridSize = 2.5 ; // dose voxel length unit : mm 
	double m_cutoff = 10;
 
	planPrimaryVolume = NULL; //The Primary Image Volume for RT Plan 

	snakePath = NULL; //the Snake Path
    Ir192Seed =NULL;
    doseVolume = NULL; //The
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


//Initialize the empty Dose Grid,prepare for Dose Calculation
void vtkSRPlanBDoseCalculateLogic::InitializeEmptyDosGrid()
{
	if (!planPrimaryVolume)
	{
		return;
	}

	char * emptyDoseGrid = "DoseGrid";
	//Clone vtkMRMLScalarVolume without imagedata
	this->doseVolume = vtkSlicerVolumesLogic::CloneVolumeWithoutImageData(this->GetMRMLScene(), planPrimaryVolume, emptyDoseGrid);

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

	//planPrimaryVolume->GetRASToIJKMatrix()


	//vtkMRMLVolumeNode::GetRASToIJKMatrix(vtkMatrix4x4* mat)


//	m_gridSize




}



/*


Superclass::GetRASBounds( bounds);

vtkImageData *volumeImage;
if (! (volumeImage = this->GetImageData()) )
{
return;
}

//
// Get the size of the volume in RAS space
// - map the size of the volume in IJK into RAS
// - map the middle of the volume to RAS for the center
//   (IJK space always has origin at first pixel)
//

vtkNew<vtkGeneralTransform> transform;
transform->PostMultiply();
transform->Identity();

vtkNew<vtkMatrix4x4> ijkToRAS;
this->GetIJKToRASMatrix(ijkToRAS.GetPointer());
transform->Concatenate(ijkToRAS.GetPointer());

vtkMRMLTransformNode *transformNode = this->GetParentTransformNode();

if ( transformNode )
{
vtkNew<vtkGeneralTransform> worldTransform;
worldTransform->Identity();
//transformNode->GetTransformFromWorld(worldTransform);
transformNode->GetTransformToWorld(worldTransform.GetPointer());
transform->Concatenate(worldTransform.GetPointer());
}

int dimensions[3];
int i,j,k;
volumeImage->GetDimensions(dimensions);
double doubleDimensions[4], *rasHDimensions;
double minBounds[3], maxBounds[3];

for ( i=0; i<3; i++)
{
minBounds[i] = 1.0e10;
maxBounds[i] = -1.0e10;
}
for ( i=0; i<2; i++)
{
for ( j=0; j<2; j++)
{
for ( k=0; k<2; k++)
{
doubleDimensions[0] = i*(dimensions[0]) - 0.5;
doubleDimensions[1] = j*(dimensions[1]) - 0.5 ;
doubleDimensions[2] = k*(dimensions[2]) - 0.5;
doubleDimensions[3] = 1;
rasHDimensions = transform->TransformDoublePoint( doubleDimensions);
for (int n=0; n<3; n++) {
if (rasHDimensions[n] < minBounds[n])
{
minBounds[n] = rasHDimensions[n];
}
if (rasHDimensions[n] > maxBounds[n])
{
maxBounds[n] = rasHDimensions[n];
}
}
}
}
}

for ( i=0; i<3; i++)
{
bounds[2*i]   = minBounds[i];
bounds[2*i+1] = maxBounds[i];
}


*/














//Given a Dose Grid dimensions as i,j,k ,and create empty Image Data.
vtkImageData * vtkSRPlanBDoseCalculateLogic::CreateEmptyDoseGrid(int * dims)
{

	// Create an image data
	vtkSmartPointer<vtkImageData> imageData =
		vtkSmartPointer<vtkImageData>::New();

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
	this->PrepareDoseGridVolumeNode();


	// 2 . Prepare the Ir192 3D Dose Kernal

	this->PrepareIr192SeedKernal();

	// 3 . Calculate the Dose Distribution

	this->DoseSuperposition(snakePath, Ir192Seed->GetDoseKernalVolume());

}

vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::GetCalculatedDoseVolume()
{
	return this->doseVolume;
}


void vtkSRPlanBDoseCalculateLogic::PrepareDoseGridVolumeNode()
{

	/*	vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::
	int dimensions[3] = { 0, 0, 0 };
	doseVolumeNode->GetImageData()->GetDimensions(dimensions);
	vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
	reslice->SetInputData(doseVolumeNode->GetImageData());
	reslice->SetOutputOrigin(0, 0, 0);
	reslice->SetOutputSpacing(1, 1, 1);
	reslice->SetOutputExtent(0, dimensions[0] - 1, 0, dimensions[1] - 1, 0, dimensions[2] - 1);
	reslice->SetResliceTransform(outputIJK2IJKResliceTransform);
	reslice->Update();
	vtkSmartPointer<vtkImageData> reslicedDoseVolumeImage = reslice->GetOutput();

	*/
}

void vtkSRPlanBDoseCalculateLogic::PrepareIr192SeedKernal()
{


}

void vtkSRPlanBDoseCalculateLogic::DoseSuperposition(vtkMRMLMarkupsNode * snakePath, vtkImageData * doseKernal)
{


}
