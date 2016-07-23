/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// MRML includes
#include "vtkMRMLGeneralParametersNode.h"

// VTK includes
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLGeneralParametersNode);

//----------------------------------------------------------------------------
vtkMRMLGeneralParametersNode::vtkMRMLGeneralParametersNode()
{
  this->HideFromEditors = 1;

  this->ModuleName = 0;
}

//----------------------------------------------------------------------------
vtkMRMLGeneralParametersNode::~vtkMRMLGeneralParametersNode()
{
  if (this->ModuleName)
    {
    delete [] this->ModuleName;
    this->ModuleName = 0;
    }
}

//----------------------------------------------------------------------------
void vtkMRMLGeneralParametersNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream

  vtkIndent indent(nIndent);

  if (this->ModuleName != 0)
    {
    of << " ModuleName =\"" << this->ModuleName << "\"";
    }

  ParameterMap::iterator iter;
  int i = 0;
  for (iter=this->Parameters.begin(); iter != this->Parameters.end(); iter++)
    {
    of << " parameter" << i << "= \"" << iter->first << " " << iter->second << "\"";
    i++;
    }
}

//----------------------------------------------------------------------------
void vtkMRMLGeneralParametersNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != 0)
    {
    attName = *(atts++);
    attValue = *(atts++);

    if ( !strcmp(attName, "ModuleName") )
      {
      this->SetModuleName( attValue );
      }
    else if ( !strncmp(attName, "parameter", strlen("parameter") ) )
      {
      std::string satt(attValue);
      int space = (int)satt.find(" ", 0);
      std::string sname = satt.substr(0,space);
      std::string svalue = satt.substr(space+1,satt.length()-space-1);
      this->SetParameter(sname, svalue);
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLGeneralParametersNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLGeneralParametersNode *node = (vtkMRMLGeneralParametersNode *) anode;

  this->SetModuleName(node->GetModuleName());
  this->Parameters = node->Parameters;
}

//----------------------------------------------------------------------------
const char* vtkMRMLGeneralParametersNode::GetNodeTagName()
{
  return "GeneralParameters";
}

//----------------------------------------------------------------------------
void vtkMRMLGeneralParametersNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  std::map<std::string, std::string>::iterator iter;

  os << indent << "ModuleName: " << (this->GetModuleName() ? this->GetModuleName() : "(none)") << "\n";

  for (iter=this->Parameters.begin(); iter != this->Parameters.end(); iter++)
    {
    os << indent << iter->first << ": " << iter->second << "\n";
    }
}

//----------------------------------------------------------------------------
void vtkMRMLGeneralParametersNode
::SetParameter(const std::string& name, const std::string& value)
{
  const std::string currentValue = this->GetParameter(name);
  if (value != currentValue)
    {
    this->Parameters[name] = value;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLGeneralParametersNode::UnsetParameter(const std::string& name)
{
  int count = this->Parameters.erase(name);
  if (count > 0)
    {
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLGeneralParametersNode::UnsetAllParameters()
{
  std::string::size_type count = this->Parameters.size();
  this->Parameters.clear();
  if (count != this->Parameters.size())
    {
    this->Modified();
    }
}

//----------------------------------------------------------------------------
std::string vtkMRMLGeneralParametersNode
::GetParameter(const std::string& name) const
{
  if ( this->Parameters.find(name) == this->Parameters.end() )
    {
    return std::string();
    }
  return this->Parameters.find(name)->second;
}

//----------------------------------------------------------------------------
int vtkMRMLGeneralParametersNode::GetParameterCount()
{
  return this->Parameters.size();
}

//----------------------------------------------------------------------------
std::string vtkMRMLGeneralParametersNode::GetParameterNamesAsCommaSeparatedList()
{
  std::vector<std::string> names = this->GetParameterNames();
  std::string namesAsStr;
  std::vector<std::string>::iterator it = names.begin();
  if (it != names.end())
    {
    namesAsStr = *it;
    ++it;
    }
  for(; it != names.end(); ++it)
    {
    namesAsStr.append(",").append(*it);
    }
  return namesAsStr;
}

//----------------------------------------------------------------------------
std::vector<std::string> vtkMRMLGeneralParametersNode::GetParameterNames()
{
  // If the number of parameter associated with a node become huge, it
  // could be interesting to cache the list of names instead of recomputing
  // each time the function is called.
  std::vector<std::string> names;

  ParameterMap::iterator it;
  for(it = this->Parameters.begin(); it != this->Parameters.end(); ++it)
    {
    names.push_back(it->first);
    }
  return names;
}
