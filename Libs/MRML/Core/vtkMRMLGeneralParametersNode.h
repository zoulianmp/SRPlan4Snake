/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLGeneralParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/

#ifndef __vtkMRMLGeneralParametersNode_h
#define __vtkMRMLGeneralParametersNode_h

// MRML includes
#include "vtkMRMLNode.h"

// STD includes
#include <string>
#include <vector>

/// The GeneralParametersNode  is simply a MRMLNode container for
/// an arbitrary keyword value pair map ,for any customized module
///
/// ModuleName is the tag parameter to distinguish other generalParametersNode.
/// For example:  ModuleName == "Segmentation", the generalParametersNode is for Segmentation module.
///
class VTK_MRML_EXPORT vtkMRMLGeneralParametersNode : public vtkMRMLNode
{
public:
  static vtkMRMLGeneralParametersNode *New();
  vtkTypeMacro(vtkMRMLGeneralParametersNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName();

  /// The name of the Module - this is used to
  /// customize the node selectors and other things
  vtkGetStringMacro (ModuleName);
  vtkSetStringMacro (ModuleName);

  /// Set module parameter
  void SetParameter(const std::string& name, const std::string& value);

  /// Unset the parameter identified by \a name
  void UnsetParameter(const std::string& name);

  /// Unset all parameters
  /// \sa UnsetParameter
  void UnsetAllParameters();

  /// Get module parameter identified by \a name
  std::string GetParameter(const std::string& name) const;

  /// Get number of parameters
  int GetParameterCount();

  /// Get list of parameter names separated by a comma
  /// \sa GetParameterNames
  std::string GetParameterNamesAsCommaSeparatedList();

  /// Get list of parameter names
  std::vector<std::string> GetParameterNames();

protected:
  vtkMRMLGeneralParametersNode();
  ~vtkMRMLGeneralParametersNode();

  vtkMRMLGeneralParametersNode(const vtkMRMLGeneralParametersNode&);
  void operator=(const vtkMRMLGeneralParametersNode&);

  typedef std::map<std::string, std::string> ParameterMap;
  ParameterMap Parameters;
  char *ModuleName;
};

#endif
