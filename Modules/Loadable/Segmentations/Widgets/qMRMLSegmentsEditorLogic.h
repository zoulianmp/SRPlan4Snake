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


//QT include 
#include "QString.h"



//Segmentation Include
#include "vtkSegmentation.h"
#include "vtkSegment.h"

#include "vtkMRMLSliceCompositeNode.h"

// qMRML includes
#include "qMRMLSliceWidget.h"

#include "qSRPlanSegmentationsModuleWidgetsExport.h"

// MRMLLogic includes
#include "vtkMRMLAbstractLogic.h"
#include "QMap.h"

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
class qMRMLEffect;

/// \brief Slicer logic class for slice manipulation.
///
//
/// Editor Logic for Image Volume Sgementation 
class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLSegmentsEditorLogic : public vtkMRMLAbstractLogic
{
public:

	/// The Usual VTK class functions
	static qMRMLSegmentsEditorLogic *New();
	vtkTypeMacro(qMRMLSegmentsEditorLogic, vtkMRMLAbstractLogic);
	
	//The enum type for EffectMode
	enum EffectMode { PaintBrush, FreeDraw, Threshold };

	/// Convenient methods allowing to initialize SliceLogic given \a newSliceNode
	/// \note This method should be used when the Logic is "shared" between two widgets
	void Initialize(vtkMRMLSliceNode* newSliceNode);
	bool IsInitialized();

	

	void SetCurrentSegment(vtkSegment * segment);
	void SetCurrentSegmentation(vtkSegmentation* segmentation);

	EffectMode GetCurrentEffectMode();
	void SetCurrentEffectMode(EffectMode effect);

	qMRMLEffect * GetCurrentEffect();




	qMRMLEffect * GetEditorEffect(EffectMode effect= qMRMLSegmentsEditorLogic::PaintBrush);


	vtkMRMLSliceCompositeNode * GetCompositeNode(char * layoutName = "Red");
	
	qMRMLSliceWidget * GetSliceWidget(char * layoutName = "Red");
	vtkMRMLSliceLogic * GetSliceLogic(char * layoutName = "Red");

	//Background Volume Related
	vtkMRMLVolumeNode * GetBackgroundVolume();
	vtkImageData * GetBackgroundImage();
	char * GetBackgroundID();

	//Label Volume Related
	vtkMRMLVolumeNode * GetLabelVolume();
	vtkImageData * GetLabelImage();
	char * GetLabelID();

	void SetPropagateMode( int Mode);
	int GetPropagateMode();

	void SetActiveVolumes(const char * masterVolume, const char* mergeVolume = "");
	void PropagateVolumeSelection();


	//Editor related segment's Label
	int  GetLabel( );
	void SetLabel(int label);
	void BackupLabel();
	void RestoreLabel();
	void ToggleLabel();


	bool IsEraseEffectEnabled();
	void SetEraseEffectEnabled(bool enabled);

	void ToggleCrosshair();
	void ToggleForegroundBackground();
	void markVolumeNodeAsModified(vtkMRMLVolumeNode* volumeNode);




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


	//Editor Parameters for Edit LabelmapVolume
	int CurrentLable; //Current Label value for structure
	int StoredLabel; //Used for Restore Label Value
	



	EffectMode CurrentEffectMode;

	qMRMLEffect * CurrentEffect;

	QMap<EffectMode, qMRMLEffect *>  editorEffectMap;

	//  enum Layers
    //  {
	//    LabelLayer = 0x1,
	//	  ForegroundLayer = 0x2,
	//	  BackgroundLayer = 0x4,
	//	  AllLayers = LabelLayer | ForegroundLayer | BackgroundLayer
	//   };
	// Used to index the PropagationMode

	int PropagationMode;

	vtkSegment * CurrentSegment;
	vtkSegmentation * CurrentSegmentation;

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