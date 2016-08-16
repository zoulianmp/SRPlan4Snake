/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// SegmentationCore includes
#include "vtkSegment.h"

#include "vtkSegmentationConverterFactory.h"
#include "vtkOrientedImageData.h"
#include "vtkPolyData.h"
#include "vtkBinaryLabelmapToClosedSurfaceConversionRule.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkMath.h>
#include <vtkDataSet.h>

#include "vtkCallbackCommand.h"
#include "vtkSegmentationConverter.h"
#include "vtkEventBroker.h"

// STD includes
#include <sstream>
#include <algorithm>

//----------------------------------------------------------------------------
const double vtkSegment::SEGMENT_COLOR_VALUE_INVALID[4] = {0.5, 0.5, 0.5, 1.0};

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSegment);

//----------------------------------------------------------------------------
vtkSegment::vtkSegment()
{
  this->Name = NULL;
  this->DefaultColor[0] = 0.5;
  this->DefaultColor[1] = 0.5;
  this->DefaultColor[2] = 0.5;

  
  this->LabelMapImageCallbackCommand = vtkCallbackCommand::New();
  this->LabelMapImageCallbackCommand->SetClientData(reinterpret_cast<void *>(this));
  this->LabelMapImageCallbackCommand->SetCallback(vtkSegment::OnLabelMapImageModified);

}

//----------------------------------------------------------------------------
vtkSegment::~vtkSegment()
{
  this->RemoveAllRepresentations();
  this->Representations.clear();

  if (this->LabelMapImageCallbackCommand)
  {
	  this->LabelMapImageCallbackCommand->SetClientData(NULL);
	  this->LabelMapImageCallbackCommand->Delete();
	  this->LabelMapImageCallbackCommand = NULL;
  }


}

//----------------------------------------------------------------------------
void vtkSegment::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Name:   " << (this->Name ? this->Name : "NULL") << "\n";
  os << indent << "DefaultColor:   (" << this->DefaultColor[0] << ", " << this->DefaultColor[1] << ", " << this->DefaultColor[2] << ")\n";

  RepresentationMap::iterator reprIt;
  os << indent << "Representations:\n";
  for (reprIt=this->Representations.begin(); reprIt!=this->Representations.end(); ++reprIt)
  {
    os << indent << "  " << reprIt->first << "\n";
  }

  std::vector<std::string>::iterator tagIt;
  os << indent << "Tags:\n";
  for (tagIt=this->Tags.begin(); tagIt!=this->Tags.end(); ++tagIt)
  {
    os << indent << "  " << (*tagIt) << "\n";
  }
}

//---------------------------------------------------------------------------
bool vtkSegment::GetModifiedSinceRead(const vtkTimeStamp& storedTime)
{
  RepresentationMap::iterator reprIt;
  for (reprIt=this->Representations.begin(); reprIt!=this->Representations.end(); ++reprIt)
  {
    if (reprIt->second && reprIt->second->GetMTime() > storedTime)
    {
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------
void vtkSegment::WriteXML(ostream& of, int nIndent)
{
  vtkIndent indent(nIndent);

  of << indent << "Name:\"" << (this->Name ? this->Name : "NULL") << "\"";
  of << indent << "DefaultColor:\"(" << this->DefaultColor[0] << ", " << this->DefaultColor[1] << ", " << this->DefaultColor[2] << ")\"";

  RepresentationMap::iterator reprIt;
  of << indent << "Representations:\"";
  for (reprIt=this->Representations.begin(); reprIt!=this->Representations.end(); ++reprIt)
  {
    of << indent << "  " << reprIt->first << "\"";
  }

  std::vector<std::string>::iterator tagIt;
  of << indent << "Tags:\"";
  for (tagIt=this->Tags.begin(); tagIt!=this->Tags.end(); ++tagIt)
  {
    of << indent << "  " << (*tagIt) << "\"";
  }
}

//----------------------------------------------------------------------------
void vtkSegment::DeepCopy(vtkSegment* aSegment)
{
  if (!aSegment)
  {
    return;
  }

  // Copy properties
  this->SetName(aSegment->Name);
  this->SetDefaultColor(aSegment->DefaultColor);
  this->Tags = aSegment->Tags;

  // Deep copy representations
  RepresentationMap::iterator reprIt;
  for (reprIt=aSegment->Representations.begin(); reprIt!=aSegment->Representations.end(); ++reprIt)
  {
    vtkDataObject* representationCopy =
      vtkSegmentationConverterFactory::GetInstance()->ConstructRepresentationObjectByClass( reprIt->second->GetClassName() );
    if (!representationCopy)
    {
      vtkErrorMacro("DeepCopy: Unable to construct representation type class '" << reprIt->second->GetClassName() << "'");
      return;
    }
    representationCopy->DeepCopy(reprIt->second);
    this->AddRepresentation(reprIt->first, representationCopy);
    representationCopy->Delete(); // Release ownership to segment only
  }
}

//---------------------------------------------------------------------------
// (Xmin, Xmax, Ymin, Ymax, Zmin, Zmax)
//---------------------------------------------------------------------------
void vtkSegment::GetBounds(double bounds[6])
{
  vtkOrientedImageData::UninitializeBounds(bounds);

  RepresentationMap::iterator reprIt;
  for (reprIt=this->Representations.begin(); reprIt!=this->Representations.end(); ++reprIt)
  {
    vtkDataSet* representationDataSet = vtkDataSet::SafeDownCast(reprIt->second);
    if (representationDataSet)
    {
      double representationBounds[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
      vtkOrientedImageData::UninitializeBounds(representationBounds);
      representationDataSet->GetBounds(representationBounds);
      vtkSegment::ExtendBounds(representationBounds, bounds);
    }
  }
}

//---------------------------------------------------------------------------
vtkDataObject* vtkSegment::GetRepresentation(std::string name)
{
  // Use find function instead of operator[] not to create empty representation if it is missing
  RepresentationMap::iterator reprIt = this->Representations.find(name);
  if (reprIt != this->Representations.end())
  {
    return reprIt->second.GetPointer();
  }
  else
  {
    return NULL;
  }
}

//---------------------------------------------------------------------------
void vtkSegment::AddRepresentation(std::string name, vtkDataObject* representation)
{
  if (this->GetRepresentation(name) == representation)
  {
    return;
  }

  const char* masterRepresentName =  vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName();
  if (!strcmp(name.c_str(), masterRepresentName))
  {
	  //master represenataion Add observer
	  vtkEventBroker::GetInstance()->AddObservation(
		  representation, vtkCommand::ModifiedEvent, this, this->LabelMapImageCallbackCommand);
	  
  }

  this->Representations[name] = representation;
  representation->Register(this); // Otherwise the representation object may get deleted (and then crashes in vtkSegmentation::SegmentModified)
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSegment::RemoveRepresentation(std::string name)
{

  vtkDataObject* representation = this->GetRepresentation(name);

  const char* masterRepresentName = vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName();
  if (!strcmp(name.c_str(), masterRepresentName))
  {
	  //master represenataion Add observer
	  vtkEventBroker::GetInstance()->RemoveObservations(
		  representation, vtkCommand::ModifiedEvent, this, this->LabelMapImageCallbackCommand);

  }


  if (representation)
  {
    this->Representations.erase(name);
    representation->UnRegister(this);
    this->Modified();
  }
}



/*
//Update The Closed Surface Representation from Labmap Image
void vtkSegment::UpdateClosedSurfaceFromLabelMapImage()
{
	const char* masterRepresentName = vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName();



	vtkImageData * labelImage = vtkImageData::SafeDownCast(this->GetRepresentation(masterRepresentName));

	vtkMRMLScene * scene = qSlicerCoreAP
	//The the vtkMRMLLabelMapVolumeNode by provided vtkImageData.
	static vtkMRMLLabelMapVolumeNode * GetLabelMapVolumeNodebyImageData(vtkMRMLScene* scene, vtkImageData* imageData);




	vtkOrientedImageData* labelMapImage 



	const char* closedSurfaceName = vtkSegmentationConverter::GetSegmentationClosedSurfaceRepresentationName();

	vtkPolyData* closedSurface = vtkPolyData::SafeDownCast(this->GetRepresentation(closedSurfaceName));


	vtkBinaryLabelmapToClosedSurfaceConversionRule * conversionRule = vtkBinaryLabelmapToClosedSurfaceConversionRule::New();

	conversionRule->SetConversionParameter(conversionRule->GetDecimationFactorParameterName(),
		"0.0", "Desired reduction in the total number of polygons (e.g., if set to 0.9, then reduce the data set to 10% of its original size)");

	conversionRule->SetConversionParameter(conversionRule->GetSmoothingFactorParameterName(),
		"0.1", "Relaxation factor for Laplacian smoothing. Value of 0 results in no smoothing, while 1 means significant smoothing.");


	if (!closedSurface)
	{
		closedSurface = vtkPolyData::New();
		conversionRule->Convert(labelMapImage, closedSurface);

		this->AddRepresentation(closedSurfaceName, closedSurface);
	}
	else
	{
		conversionRule->Convert(labelMapImage, closedSurface);
	}

}




*/






//---------------------------------------------------------------------------
void vtkSegment::RemoveAllRepresentations(std::string exceptionRepresentationName/*=""*/)
{
  RepresentationMap::iterator reprIt = this->Representations.begin();
  while (reprIt != this->Representations.end())
  {
    if (reprIt->first.compare(exceptionRepresentationName))
    {
      RepresentationMap::iterator erasedIt = reprIt;
      vtkDataObject* representation = this->Representations[reprIt->first].GetPointer();

	  //If Master representation removed, Remove the Observation
	  const char* name = reprIt->first.c_str();
	  const char* masterRepresentName = vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName();
	  if (!strcmp(name , masterRepresentName))
	  {
		  //master represenataion Add observer
		  vtkEventBroker::GetInstance()->RemoveObservations(
			  representation, vtkCommand::ModifiedEvent, this, this->LabelMapImageCallbackCommand);

	  }

      ++reprIt;
      this->Representations.erase(erasedIt);
      representation->UnRegister(this); // Not just call RemoveRepresentation to avoid multiple Modified calls
    }
    else
    {
      ++reprIt;
    }
  }
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSegment::GetContainedRepresentationNames(std::vector<std::string>& representationNames)
{
  representationNames.clear();

  RepresentationMap::iterator reprIt;
  for (reprIt=this->Representations.begin(); reprIt!=this->Representations.end(); ++reprIt)
  {
    representationNames.push_back(reprIt->first);
  }
}

//---------------------------------------------------------------------------
void vtkSegment::AddTag(std::string tag)
{
  this->Tags.push_back(tag);
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSegment::RemoveTag(std::string tag)
{
  std::vector<std::string>::iterator tagsIt = std::find(this->Tags.begin(), this->Tags.end(), tag);
  if (tagsIt != this->Tags.end())
  {
    this->Tags.erase(tagsIt);
  }
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSegment::GetTags(std::vector<std::string> &tags)
{
  tags = this->Tags;
}

//---------------------------------------------------------------------------
// Global RAS in the form (Xmin, Xmax, Ymin, Ymax, Zmin, Zmax)
void vtkSegment::ExtendBounds(double partialBounds[6], double globalBounds[6])
{
  if (partialBounds[0] < globalBounds[0] )
  {
    globalBounds[0] = partialBounds[0];
  }
  if (partialBounds[2] < globalBounds[2] )
  {
    globalBounds[2] = partialBounds[2];
  }
  if (partialBounds[4] < globalBounds[4] )
  {
    globalBounds[4] = partialBounds[4];
  }

  if (partialBounds[1] > globalBounds[1] )
  {
    globalBounds[1] = partialBounds[1];
  }
  if (partialBounds[3] > globalBounds[3] )
  {
    globalBounds[3] = partialBounds[3];
  }
  if (partialBounds[5] > globalBounds[5] )
  {
    globalBounds[5] = partialBounds[5];
  }
}


//The callback function for Process LabelMapImageModified by GUI
void vtkSegment::OnLabelMapImageModified(vtkObject* vtkNotUsed(caller),
	unsigned long vtkNotUsed(eid),
	void* clientData,
	void* vtkNotUsed(callData))
{
	const char* masterRepresentName = vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName();
	vtkSegment * self = reinterpret_cast<vtkSegment *>(clientData);

	self->RemoveAllRepresentations(masterRepresentName);

}