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


#include "vtkImageIterator.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSRPlanBDoseCalculateLogic);

//----------------------------------------------------------------------------
vtkSRPlanBDoseCalculateLogic::vtkSRPlanBDoseCalculateLogic()
{
	this->m_gridSize = 2.0 ; // dose voxel length unit : mm 
	this->m_cutoff = 70; //Cutoff Distance from center unit:mm
 
	this->planPrimaryVolume = NULL; //The Primary Image Volume for RT Plan 

	this->snakePath = NULL; //the Snake Path
    this-> Ir192Seed =NULL;
    this->doseVolume = NULL; //The

	this->selectionNode = NULL;

	this->TDoseValuemaximum = 0;

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
	//vtkSetAndObserveMRMLNodeMacro(this->planPrimaryVolume, primary);

	this->planPrimaryVolume = primary;
}

vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::GetPlanPrimaryVolumeNode()
{
	return this->planPrimaryVolume;

}


void vtkSRPlanBDoseCalculateLogic::SetSnakePlanPath(vtkMRMLMarkupsNode * snakePath)
{
	//vtkSetAndObserveMRMLNodeMacro(this->snakePath, snakePath);

	this->snakePath = snakePath;

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

	std::string emptyDoseGrid = std::string("DoseGrid") + std::string(snakePath->GetName());
	
	//Clone vtkMRMLScalarVolume without imagedata
	this->doseVolume = vtkSlicerVolumesLogic::CloneVolumeWithoutImageData(this->GetMRMLScene(), planPrimaryVolume, emptyDoseGrid.c_str());

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


void vtkSRPlanBDoseCalculateLogic::InitializeEmptyDosGridNodeAsPlanImage()
{
	if (!planPrimaryVolume)
	{
		return;
	}

	std::string emptyDoseGrid = std::string("DoseGrid") + std::string(snakePath->GetName());

	//Clone vtkMRMLScalarVolume without imagedata
	this->doseVolume = vtkSlicerVolumesLogic::CloneVolumeWithoutImageData(this->GetMRMLScene(), planPrimaryVolume, emptyDoseGrid.c_str());
	 

	int* dimsReal = this->planPrimaryVolume->GetImageData()->GetDimensions();


	vtkImageData * doseGrid = CreateEmptyDoseGrid(dimsReal);

	doseVolume->SetAndObserveImageData(doseGrid);

	/*
	
	vtkImageData * imageData = this->doseVolume->GetImageData();


	// Fill every entry of the image data with x,y,z
	int* dimsReal = imageData->GetDimensions();


	unsigned short *ptr = static_cast<unsigned short *>(imageData->GetScalarPointer(0, 0, 0));
	for (int z = 0; z < dimsReal[2]; z++)
	{
		for (int y = 0; y < dimsReal[1]; y++)
		{
			for (int x = 0; x < dimsReal[0]; x++)
			{
				*ptr++ = 0.0;

			}
		}
	}
	*/
}

void vtkSRPlanBDoseCalculateLogic::InvalidDoseAndRemoveDoseVolumeNodeFromScene()
{
	if (this->doseVolume)
	{
		this->GetMRMLScene()->RemoveNode(doseVolume);
	}

	this->doseVolume = NULL;

	//Show Dose in Slice Views "Red","Yellow", ("Green"
	this->SetDoseNodetoLayoutCompositeNode("Red", this->doseVolume);
	this->SetDoseNodetoLayoutCompositeNode("Yellow", this->doseVolume);
	this->SetDoseNodetoLayoutCompositeNode("Green", this->doseVolume);
	//this->selectionNode->SetActiveDoseGridID("sasd");
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

	int Extent[6];
	Extent[0] = 0;
	Extent[1] = dims[0]-1;
	Extent[2] = 0;
	Extent[3] = dims[1] - 1;
	Extent[4] = 0;
	Extent[5] = dims[2] - 1;

	imageData->SetExtent(Extent);


	imageData->AllocateScalars(VTK_DOUBLE, 1);

	// Fill every entry of the image data with x,y,z
	int* dimsReal = imageData->GetDimensions();

	double *ptr = static_cast<double *>(imageData->GetScalarPointer(0, 0, 0));
	for (int z = 0; z < dimsReal[2]; z++)
	{
		for (int y = 0; y < dimsReal[1]; y++)
		{
			for (int x = 0; x < dimsReal[0]; x++)
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


	//Remove Preexist DoseDistributionNode before clone a new one
	if (this->doseVolume)
	{
		this->InvalidDoseAndRemoveDoseVolumeNodeFromScene();
	}


	this->InitializeEmptyDosGridNode();

	//this->InitializeEmptyDosGridNodeAsPlanImage();

	double * spacing = this->doseVolume->GetSpacing();

	this->m_gridSize = spacing[0];

	// 2 . Prepare the Ir192 3D Dose Kernal

	this->PrepareIr192SeedKernal();

	// 3 . Calculate the Dose Distribution

	vtkImageData* kernal = this->Ir192Seed->GetDoseKernalVolume();

	// initial the Dose Maximun to 0
	this->TDoseValuemaximum = 0;

	this->DoseSuperposition(snakePath, kernal);

	this->NormalizedToMaximum(this->doseVolume,this->TDoseValuemaximum);

	//Update the SelectionNode Active Dose Grid ID
	
	this->selectionNode->SetActiveDoseGridID(this->doseVolume->GetID());


	//this->resampledTodoseVolume = 	vtkSlicerVolumesLogic::ResampleVolumeToReferenceVolume(this->doseVolume, this->planPrimaryVolume);

	

	//this->SetDoseNodetoLayoutCompositeNode("Red", this->resampledTodoseVolume);
	//this->SetDoseNodetoLayoutCompositeNode("Yellow", this->resampledTodoseVolume);
	//this->SetDoseNodetoLayoutCompositeNode("Green", this->resampledTodoseVolume);


	//Show Dose in Slice Views "Red","Yellow", ("Green"
	this->SetDoseNodetoLayoutCompositeNode("Red", this->doseVolume);
	this->SetDoseNodetoLayoutCompositeNode("Yellow", this->doseVolume);
	this->SetDoseNodetoLayoutCompositeNode("Green", this->doseVolume); 

	//just for Debug 
	//this->SetPlangImageNodetoBackgroundofLayoutCompositeNode("Red", this->planPrimaryVolume);
	

}

vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::GetCalculatedDoseVolume()
{
	return this->doseVolume;
}


vtkMRMLScalarVolumeNode * vtkSRPlanBDoseCalculateLogic::GetResampledDoseVolume()
{
	return this->resampledTodoseVolume;
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
	int numMarkups = snakePath->GetNumberOfMarkups();

	float doseWeight = 0.0;

	vtkVector3d seedPosition;
	double  rasPosition[3] = {0,0,0};

	int IJK[3] = { 0,0,0 }; //The RAS corresponding IJK

	for (int m = 0; m<numMarkups; m++)
	{
		Markup * markup = snakePath->GetNthMarkup(m);

		//skip the Realtime Tracing Mark
		if (!strcmp((markup->Label).c_str(), "TMark"))
			continue;


		doseWeight = markup->Weight;
		
		//Skip the 0 weight markup 
		if (!doseWeight)
			continue;

		seedPosition = markup->points[0];

		rasPosition[0] = seedPosition[0];
		rasPosition[1] = seedPosition[1];
		rasPosition[2] = seedPosition[2];

		this->GetIJKFromRASPostion(this->doseVolume, rasPosition, IJK);


		// Define the extent to be extracted
		int baseExtent_for_extract [6]; //The Dose Grid  need to extract
		int kenalExtent_for_extract [6]; // The kernal need to extract 


		int* dosegridExtent = this->doseVolume->GetImageData()->GetExtent();

		int* kernalExtent = doseKernal->GetExtent();


		int Imin, Imax, Jmin, Jmax, Kmin, Kmax;

		Imin = IJK[0] + kernalExtent[0];
		Imax = IJK[0] + kernalExtent[1];

		Jmin = IJK[1] + kernalExtent[2];
		Jmax = IJK[1] + kernalExtent[3];

		Kmin = IJK[2] + kernalExtent[4];
		Kmax = IJK[2] + kernalExtent[5];

		//**************************************************************************************
		//Imin Imax Index
		if (Imin < dosegridExtent[0])
		{
			baseExtent_for_extract[0] = dosegridExtent[0];
			kenalExtent_for_extract[0] = kernalExtent[0] + (dosegridExtent[0] - Imin);
		}
		else
		{
			baseExtent_for_extract[0] = Imin;
			kenalExtent_for_extract[0] = kernalExtent[0];
		}

		if (Imax  > dosegridExtent[1])
		{
			baseExtent_for_extract[1] = dosegridExtent[1];
			kenalExtent_for_extract[1] = kernalExtent[1] - (Imax - dosegridExtent[1]);
		}
		else
		{
			baseExtent_for_extract[1] = Imax;
			kenalExtent_for_extract[1] = kernalExtent[1];
		}
		//**************************************************************************************
		//Jmin Jmax Index

		if (Jmin < dosegridExtent[2])
		{
			baseExtent_for_extract[2] = dosegridExtent[2];
			kenalExtent_for_extract[2] = kernalExtent[2] + (dosegridExtent[2] - Jmin);
		}
		else
		{
			baseExtent_for_extract[2] = Jmin;
			kenalExtent_for_extract[2] = kernalExtent[2];
		}

		if (Jmax  > dosegridExtent[3])
		{
			baseExtent_for_extract[3] = dosegridExtent[3];
			kenalExtent_for_extract[3] = kernalExtent[3] - (Jmax - dosegridExtent[3]);
		}
		else
		{
			baseExtent_for_extract[3] = Jmax;
			kenalExtent_for_extract[3] = kernalExtent[3];
		}

		//**************************************************************************************
		//Kmin Kmax Index

		if (Kmin < dosegridExtent[4])
		{
			baseExtent_for_extract[4] = dosegridExtent[4];
			kenalExtent_for_extract[4] = kernalExtent[4] + (dosegridExtent[4] - Kmin);
		}
		else
		{
			baseExtent_for_extract[4] = Kmin;
			kenalExtent_for_extract[4] = kernalExtent[4];
		}

		if (Kmax  > dosegridExtent[5])
		{
			baseExtent_for_extract[5] = dosegridExtent[5];
			kenalExtent_for_extract[5] = kernalExtent[5] - (Kmax - dosegridExtent[5]);
		}
		else
		{
			baseExtent_for_extract[5] = Kmax;
			kenalExtent_for_extract[5] = kernalExtent[5];
		}

		//**************************************************************************************
		//Image Iterator to accumulate the Dose Calculaion

		vtkImageData * emptyDoseGrid = this->doseVolume->GetImageData();
		//emptyDoseGrid->Initialize();


		// Retrieve the entries from the image data and print them to the screen
		vtkImageIterator<double> doseBaseIt(emptyDoseGrid, baseExtent_for_extract);




		bool endb = doseBaseIt.IsAtEnd();

	    doseBaseIt.Initialize(this->doseVolume->GetImageData(), baseExtent_for_extract);

		bool beginb = doseBaseIt.IsAtEnd();





		//Make sure the Kernal image pixle type to double
		vtkImageIterator<double> kernalIt(doseKernal, kenalExtent_for_extract);

		double detaInc,base;

		while (!doseBaseIt.IsAtEnd())
		{
			// For Dose Grid 
			double * doseBaseValIt = doseBaseIt.BeginSpan();
			double * doseBaseValEnd = doseBaseIt.EndSpan();

			//For Kernal 
			double* kernalValIt = kernalIt.BeginSpan();
			double *kernalValEnd = kernalIt.EndSpan();

			while (doseBaseValIt != doseBaseValEnd)
			{
				base = *doseBaseValIt;
				// Increment for each component
				detaInc = (*kernalValIt) * doseWeight;


				*doseBaseValIt = base + detaInc;

				if (this->TDoseValuemaximum < *doseBaseValIt)
				{
					this->TDoseValuemaximum = *doseBaseValIt;
				}

				doseBaseValIt++;
				kernalValIt++;
			}
			

			doseBaseIt.NextSpan();
			kernalIt.NextSpan();

		}


		
		

	}

	/*
	//**************************************************
	//Just for debug show 
	int roiExtent[6];
	roiExtent[0] = IJK[0] - 3;
	roiExtent[1] = IJK[0] + 3;
	roiExtent[2] = IJK[1] - 3;
	roiExtent[3] = IJK[1] + 3;
	roiExtent[4] = IJK[2] - 3;
	roiExtent[5] = IJK[2] + 3;


	this->PrintROIDose(this->doseVolume->GetImageData(), roiExtent);




	//End just for debug show
	//*****************************************************

	*/
}




//Normalize the Dose Grid to Maximum,Get the Relative distribution
void vtkSRPlanBDoseCalculateLogic::NormalizedToMaximum(vtkMRMLScalarVolumeNode * absDoseVolume, double dosMax)
{
	vtkImageData * absImageData = absDoseVolume->GetImageData();

	//double* range = absImageData->GetScalarRange();

	double maximum = dosMax;

	if (maximum <= 0) 
		return;


	// Normalized the Pixel
	int* dims = absImageData->GetDimensions();
	double *ptr = static_cast<double *>(absImageData->GetScalarPointer(0, 0, 0));

	double  absDose;

	for (int z = 0; z<dims[2]; z++)
	{
		for (int y = 0; y<dims[1]; y++)
		{
			for (int x = 0; x<dims[0]; x++)
			{
				absDose = *ptr;
				*ptr = (absDose / maximum) * 100;

				ptr++;

			}
		}
	}

	
	//**************************************************
	//Just for debug show
	int roiExtent[6];

	

	roiExtent[0] = (dims[0] / 2) - 3;
	roiExtent[1] = (dims[0] / 2) + 3;

	roiExtent[2] = (dims[1] / 2 ) - 3;
	roiExtent[3] = (dims[1] / 2 ) + 3;

	roiExtent[4] = (dims[2] / 2) - 3;
	roiExtent[5] = (dims[1] / 2) + 3;


	this->PrintROIDose(absDoseVolume->GetImageData(), roiExtent);

	std::cout <<"Normalized DoseVolume output "<< std::endl;


	//End just for debug show
	//*****************************************************

	
}



//Givent a RAS Point, return the IJK Index
void vtkSRPlanBDoseCalculateLogic::GetIJKFromRASPostion(vtkMRMLScalarVolumeNode * VolumeNode, double * rasPosition, int * IJK)
{


	vtkNew<vtkGeneralTransform> transform;
	transform->PostMultiply();
	transform->Identity();


	vtkNew<vtkMatrix4x4> rasToIJKMatrix;
	VolumeNode->GetRASToIJKMatrix(rasToIJKMatrix.GetPointer());

	transform->Concatenate(rasToIJKMatrix.GetPointer());


//	int IJK[3] = { 0,0,0 };

	double* vFromRAS;
	vFromRAS = transform->TransformDoublePoint(rasPosition); //rasPosition double [3]


	IJK[0] = int (vFromRAS[0]);

	IJK[1] = int(vFromRAS[1]);

	IJK[2] = int(vFromRAS[2]);

	
}


// Retrieve the entries from the image data and print them to the screen

void vtkSRPlanBDoseCalculateLogic::PrintROIDose(vtkImageData * data, int * extent)
{

	std::cout << "******Begin In the vtkSRPlanBDoseCalculateLogic->PrintROIDose******* ";
	std::cout << std::endl;

	int numComp = data->GetNumberOfScalarComponents();

	vtkImageIterator<double> it(data, extent);

	while (!it.IsAtEnd())
	{
		double* valIt = it.BeginSpan();
		double *valEnd = it.EndSpan();
		while (valIt != valEnd)
		{

			double Dose = *valIt++;

			std::cout << "(Dose:" << Dose << ") ";
		}
		std::cout << std::endl;
		it.NextSpan();
	}

	std::cout << "****** End in the vtkSRPlanBDoseCalculateLogic->PrintROIDose******* ";
	std::cout << std::endl;
}




vtkMRMLSelectionNode * vtkSRPlanBDoseCalculateLogic::GetSelectionNode()
{
	return this->selectionNode;
}

void vtkSRPlanBDoseCalculateLogic::SetSelectionNode(vtkMRMLSelectionNode * selectionNode)
{
	this->selectionNode = selectionNode;
}


// Just for Debug the show
void  vtkSRPlanBDoseCalculateLogic::SetPlangImageNodetoBackgroundofLayoutCompositeNode(char * layoutName, vtkMRMLScalarVolumeNode * absDoseVolume)
{

	int count = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");

	for (int i = 0; i < count; i++)
	{
		vtkMRMLNode * compNode = this->GetMRMLScene()->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode");

		if (!strcmp(vtkMRMLSliceCompositeNode::SafeDownCast(compNode)->GetLayoutName(), layoutName))

		{
			vtkMRMLSliceCompositeNode* comp = vtkMRMLSliceCompositeNode::SafeDownCast(compNode);

			if (absDoseVolume)
			{
				comp->SetBackgroundVolumeID(absDoseVolume->GetID());
				 
			}
			else
			{
				comp->SetBackgroundVolumeID(NULL);
			}
			break;

		}
	}

}

 

// "Red",  ("Yellow", ("Green" for Layout name 
void vtkSRPlanBDoseCalculateLogic::SetDoseNodetoLayoutCompositeNode(char * layoutName, vtkMRMLScalarVolumeNode * absDoseVolume)
{
 
	int count = this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");

	for (int i = 0; i < count; i++)
	{
		vtkMRMLNode * compNode = this->GetMRMLScene()->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode");

		if (!strcmp(vtkMRMLSliceCompositeNode::SafeDownCast(compNode)->GetLayoutName(), layoutName))

		{
			vtkMRMLSliceCompositeNode* comp = vtkMRMLSliceCompositeNode::SafeDownCast(compNode);

			if (absDoseVolume)
			{
				comp->SetForegroundVolumeID(absDoseVolume->GetID());
				comp->SetForegroundOpacity(0.7);
			}
			else
			{
				comp->SetForegroundVolumeID(NULL);
			}
			break;

		}
	}
}

 
