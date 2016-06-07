/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLSegmentsEditorLogic.h,v $
Date:      $Date$
Version:   $Revision: 18866

=========================================================================auto=*/

#ifndef __qMRMLSegmentsEditorLogic_h
#define __qMRMLSegmentsEditorLogic_h

// MRMLLogic includes
#include "vtkMRMLAbstractLogic.h"
#include "vtkSRPlanSegmentationsModuleLogicExport.h"
// STD includes
#include <vector>

class vtkMRMLDisplayNode;
class vtkMRMLLinearTransformNode;
class vtkMRMLModelDisplayNode;
class vtkMRMLModelNode;
class vtkMRMLSliceCompositeNode;
class vtkMRMLSliceLayerLogic;
class vtkMRMLSliceNode;
class vtkMRMLVolumeNode;

class vtkAlgorithmOutput;
class vtkCollection;
class vtkImageBlend;
class vtkTransform;
class vtkImageData;
class vtkImageReslice;
class vtkPolyDataCollection;
class vtkTransform;

/// \brief Slicer logic class for slice manipulation.
///
/// This class manages the logic associated with display of slice windows
/// (but not the GUI).  Features of the class include:
///  -- a back-to-front list of MrmlVolumes to be displayed
///  -- a compositing mode for each volume layer (opacity, outline, glyph, checkerboard, etc)
///  -- each layer is required to provide an RGBA image in the space defined by the vtkMRMLSliceNode
///
/// This class manages internal vtk pipelines that create an output vtkImageData
/// which can be used by the vtkSlicerSliceGUI class to display the resulting
/// composite image or it can be used as a texture map in a vtkSlicerView.
/// This class can also be used for resampling volumes for further computation.
class VTK_SRPlan_SEGMENTATIONS_LOGIC_EXPORT qMRMLSegmentsEditorLogic : public vtkMRMLAbstractLogic
{
public:
	/// The Usual VTK class functions
	static qMRMLSegmentsEditorLogic *New();
	vtkTypeMacro(qMRMLSegmentsEditorLogic, vtkMRMLAbstractLogic);
	

	/// Convenient methods allowing to initialize SliceLogic given \a newSliceNode
	/// \note This method should be used when the Logic is "shared" between two widgets
	void Initialize(vtkMRMLSliceNode* newSliceNode);
	bool IsInitialized();

	/// Set / Get SliceLogic name
	vtkSetStringMacro(Name);
	vtkGetStringMacro(Name);

	
protected:

	qMRMLSegmentsEditorLogic();
	virtual ~qMRMLSegmentsEditorLogic();

	virtual void SetMRMLSceneInternal(vtkMRMLScene * newScene);

	///
	/// process logic events
	virtual void ProcessMRMLLogicsEvents(vtkObject * caller,
		unsigned long event,
		void * callData);
	void ProcessMRMLLogicsEvents();

	
	bool                        AddingSliceModelNodes;
	bool                        Initialized;

	char *                      Name;
	vtkMRMLSliceNode *          SliceNode;
	vtkMRMLSliceCompositeNode * SliceCompositeNode;
	vtkMRMLSliceLayerLogic *    BackgroundLayer;
	vtkMRMLSliceLayerLogic *    ForegroundLayer;
	vtkMRMLSliceLayerLogic *    LabelLayer;


	vtkImageBlend *   Blend;
	vtkImageBlend *   BlendUVW;
	vtkImageReslice * ExtractModelTexture;
	vtkAlgorithmOutput *    ImageDataConnection;
	vtkTransform *    ActiveSliceTransform;

	vtkPolyDataCollection * PolyDataCollection;
	vtkCollection *         LookupTableCollection;

	vtkMRMLModelNode *            SliceModelNode;
	vtkMRMLModelDisplayNode *     SliceModelDisplayNode;
	vtkMRMLLinearTransformNode *  SliceModelTransformNode;
	double                        SliceSpacing[3];

private:

	qMRMLSegmentsEditorLogic(const qMRMLSegmentsEditorLogic&);
	void operator=(const qMRMLSegmentsEditorLogic&);

};

#endif

/*  
//The Edit Tool Effects
enum EffectMode { PaintBrush, FreeDraw };

//The current EditMode
EffectMode currentMode;

*/