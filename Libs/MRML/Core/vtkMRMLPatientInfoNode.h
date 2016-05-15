/*=auto=========================================================================

    

  Program:    
  Module:    $RCSfile: vtkMRMLCrosshairNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/

#ifndef __vtkMRMLPatientInfoNode_h
#define __vtkMRMLPatientInfoNode_h

#include "vtkMRMLNode.h"

// VTK includes
#include <vtkStdString.h>

/// \brief MRML node for storing a Patient Information added by
///
/// This node stores the information about a Patient (PatientID,
/// Name, attributes)
class VTK_MRML_EXPORT vtkMRMLPatientInfoNode : public vtkMRMLNode
{
  public:
  static vtkMRMLPatientInfoNode *New();
  vtkTypeMacro(vtkMRMLPatientInfoNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*
  /// CursorPositionModifiedEvent is invoked when the cursor position is modified,
  /// for example a mouse pointer is moved in a slice view.
  enum
    {
      CursorPositionModifiedEvent = 22000
    };

	*/

  enum Gender
  {
	  Male,
      Female,
	  Other
  };

  virtual vtkMRMLNode* CreateNodeInstance(); 

  ///
  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  ///
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "PatientInfo";};



  ///
  /// configures the crosshair appearance and behavior
  vtkGetMacro (PatientID, vtkStdString);
  vtkSetMacro (PatientID, vtkStdString);

  vtkGetMacro (PatientName, vtkStdString);
  vtkSetMacro (PatientName, vtkStdString);
 
  vtkGetMacro(PatientAge, int);
  vtkSetMacro(PatientAge, int);


  vtkGetMacro(PatientGender, Gender);
  vtkSetMacro(PatientGender, Gender);

  





protected:
  vtkMRMLPatientInfoNode();
  ~vtkMRMLPatientInfoNode();
  vtkMRMLPatientInfoNode(const vtkMRMLPatientInfoNode&);
  void operator=(const vtkMRMLPatientInfoNode&);


  /// The Attributes of Patient Information

  vtkStdString PatientID;
  vtkStdString PatientName;

  int PatientAge;

  Gender PatientGender;


};

#endif

