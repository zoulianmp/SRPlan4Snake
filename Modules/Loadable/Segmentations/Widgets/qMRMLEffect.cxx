/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.11 $

=========================================================================auto=*/

// MRML includes
#include "qMRMLEffect.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


qMRMLEffect::qMRMLEffect()
{
	this->EffectOption = qMRMLEffect::All;

}

qMRMLEffect::~qMRMLEffect()
{

}