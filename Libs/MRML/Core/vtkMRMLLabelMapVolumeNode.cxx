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

  This file was originally developed by Andras Lasso, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// MRML includes
#include "vtkMRMLLabelMapVolumeNode.h"
#include "vtkMRMLLabelMapVolumeDisplayNode.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>

#include <vtkOrientedImageData.h>
#include <vtkOrientedImageDataResample.h>

#include <vtkMRMLTransformNode.h>
#include <vtkGeneralTransform.h>


vtkOrientedImageData * vtkMRMLLabelMapVolumeNode::GetOrientedImageData()
{
	vtkOrientedImageData* orientedImageData = vtkOrientedImageData::New();
	orientedImageData->vtkImageData::DeepCopy(this->GetImageData());

	vtkSmartPointer<vtkMatrix4x4> ijkToRasMatrix = vtkSmartPointer<vtkMatrix4x4>::New();


	this->GetIJKToRASMatrix(ijkToRasMatrix);
	orientedImageData->SetGeometryFromImageToWorldMatrix(ijkToRasMatrix);

	

	// Get world to reference RAS transform
	vtkSmartPointer<vtkGeneralTransform> nodeToWorldTransform = vtkSmartPointer<vtkGeneralTransform>::New();
	vtkMRMLTransformNode* parentTransformNode = this->GetParentTransformNode();
	if (parentTransformNode)
	{
		parentTransformNode->GetTransformToWorld(nodeToWorldTransform);

		// Transform oriented image data
		vtkOrientedImageDataResample::TransformOrientedImage(orientedImageData, nodeToWorldTransform);

	}


	return orientedImageData;


}







//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLabelMapVolumeNode);

//----------------------------------------------------------------------------
vtkMRMLLabelMapVolumeNode::vtkMRMLLabelMapVolumeNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLLabelMapVolumeNode::~vtkMRMLLabelMapVolumeNode()
{
}

//-----------------------------------------------------------
void vtkMRMLLabelMapVolumeNode::CreateNoneNode(vtkMRMLScene *scene)
{
  vtkNew<vtkImageData> id;
  id->SetDimensions(1, 1, 1);
  id->AllocateScalars(VTK_SHORT, 1);
  id->GetPointData()->GetScalars()->FillComponent(0, 0);

  vtkNew<vtkMRMLLabelMapVolumeNode> n;
  n->SetName("None");
  // the scene will set the id
  n->SetAndObserveImageData(id.GetPointer());
  scene->AddNode(n.GetPointer());
}

//----------------------------------------------------------------------------
void vtkMRMLLabelMapVolumeNode::CreateDefaultDisplayNodes()
{
  if (vtkMRMLLabelMapVolumeDisplayNode::SafeDownCast(this->GetDisplayNode())!=NULL)
    {
    // display node already exists
    return;
    }
  if (this->GetScene()==NULL)
    {
    vtkErrorMacro("vtkMRMLLabelMapVolumeNode::CreateDefaultDisplayNodes failed: scene is invalid");
    return;
    }
  vtkNew<vtkMRMLLabelMapVolumeDisplayNode> dispNode;
  this->GetScene()->AddNode(dispNode.GetPointer());
  dispNode->SetDefaultColorMap();
  this->SetAndObserveDisplayNodeID(dispNode->GetID());
}
