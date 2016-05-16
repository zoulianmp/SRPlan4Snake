/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLCrosshairNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// MRML includes
#include "vtkMRMLPatientInfoNode.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLPatientInfoNode);

//----------------------------------------------------------------------------
vtkMRMLPatientInfoNode::vtkMRMLPatientInfoNode()
{

  this->PatientID = "0000000000";
  this->PatientName = "DefaultName";
  this->PatientAge = 0;
  this->PatientGender = Male;


  this->SetSingletonOn();

}

//----------------------------------------------------------------------------
vtkMRMLPatientInfoNode::~vtkMRMLPatientInfoNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLPatientInfoNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " PatientID=\"" << this->PatientID << "\"";

  of << indent << " PatientName=\"" << this->PatientName << "\"";

  of << indent << " PatientAge=\"" << this->PatientAge << "\"";

  of << indent << " PatientGender=\"" << this->PatientGender << "\"";

}

//----------------------------------------------------------------------------
void vtkMRMLPatientInfoNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
  {
	  attName = *(atts++);
	  attValue = *(atts++);

	  if (!strcmp(attName, "PatientID"))
	  {
		  this->SetPatientID(attValue);
	  }
	  else if (!strcmp(attName, "PatientName"))
	  {
		  this->SetPatientName(attValue);

	  }
	  else if (!strcmp(attName, "PatientAge"))
	  {
		  this->SetPatientAge(atoi(attValue));

	  }
	  else if (!strcmp(attName, "PatientGender"))
	  {		  
		  if (!strcmp(attValue, "Male"))
		  {
			  this->SetPatientGender(Male);
		  }
		  else if (!strcmp(attValue, "Female"))
		  {
			  this->SetPatientGender(Female);
		  }
		  else if (!strcmp(attValue, "Other"))
		  {
			  this->SetPatientGender(Other);
		  }
	  }

  }

  this->EndModify(disabledModify);

}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLPatientInfoNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLPatientInfoNode *node = vtkMRMLPatientInfoNode::SafeDownCast(anode);

  this->SetPatientID(node->GetPatientID());
  this->SetPatientName(node->GetPatientName());
  this->SetPatientAge(node->GetPatientAge());

  this->SetPatientGender(node->GetPatientGender());


  this->EndModify(disabledModify);

}

//----------------------------------------------------------------------------
void vtkMRMLPatientInfoNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "PatientID: " << this->PatientID << "\n";
  os << indent << "PatientName: " << this->PatientName << "\n";
  os << indent << "PatientAge: " << this->PatientAge << "\n";
  os << indent << "PatientGender: " << this->PatientGender << "\n";
 
}

// End
