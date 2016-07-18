project(vtkSRPlan${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

 

set(${KIT}_EXPORT_DIRECTIVE "VTK_SRPlan_${MODULE_NAME_UPPER}_LOGIC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_BINARY_DIR} 
	${vtkSegmentationCore_INCLUDE_DIRS}
	${SRPlan_Base_INCLUDE_DIRS} 
    ${Qt5Core_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt5Xml_INCLUDE_DIRS}
	${Qt5Concurrent_INCLUDE_DIRS}

	${CTK_INCLUDE_DIRS}

    ${vtkSlicerSegmentationsModuleMRML_INCLUDE_DIRS}

	${qSRPlanVolumesModule_INCLUDE_DIRS}
    ${vtkSRPlanVolumesModuleLogic_INCLUDE_DIRS}
    ${qSRPlanBaseQTGUI_INCLUDE_DIRS}
	${qMRMLWidgets_INCLUDE_DIRS}
	${qSRPlanBaseQTCore_INCLUDE_DIRS}
	${qSRPlanBaseQTGUI_INCLUDE_DIRS}
	
  )

set(${KIT}_SRCS
  vtkSlicer${MODULE_NAME}ModuleLogic.cxx
  vtkSlicer${MODULE_NAME}ModuleLogic.h
  
  qMRMLSegmentsEditorLogic.h
  qMRMLSegmentsEditorLogic.cxx

  vtkImageConnectivity.cxx
  vtkImageErode.cxx
  vtkImageFillROI.cxx
  vtkImageLabelChange.cxx
  vtkImageSlicePaint.cxx
  vtkImageStash.cxx
  vtkPichonFastMarching.cxx
  vtkPichonFastMarchingPDF.cxx
  
  
  qMRMLEffect.h
  qMRMLEffect.cxx
    
  qMRMLThresholdEffect.h
  qMRMLThresholdEffect.cxx
  
  
  qMRMLLabelEffect.h
  qMRMLLabelEffect.cxx
  
  qMRMLPaintEffect.h
  qMRMLPaintEffect.cxx
  
  qMRMLDrawEffect.h
  qMRMLDrawEffect.cxx
  
  
  
  )

    
set(${KIT}_MOC_SRCS
    qMRMLEffect.h
#	qMRMLThresholdEffect.h
#	qMRMLLabelEffect.h
	qMRMLPaintEffect.h
#    qMRMLDrawEffect.h
   )
  
  
set(${KIT}_TARGET_LIBRARIES
  vtkSRPlan${MODULE_NAME}ModuleMRML
  ${CTK_LIBRARIES}
  vtkSRPlanVolumesModuleLogic
  qSRPlanVolumesModule
  qSRPlanBaseQTGUI
  qSRPlanBaseQTCore
  qMRMLWidgets

  Qt5::Core
  Qt5::Widgets
  Qt5::Xml
  Qt5::Concurrent
  )




set(${KIT}_MOC_OUTPUT)

QT5_WRAP_CPP(${KIT}_MOC_OUTPUT ${${KIT}_MOC_SRCS})

source_group("Generated" FILES
   ${KIT}_MOC_OUTPUT
    )


#Add generated files to Source files 
set(${KIT}_SRCS 
    ${${KIT}_SRCS} 
    ${${KIT}_MOC_OUTPUT}
	)
#message("The kit SRS: in the SegmentationsModule Logic ", ${${KIT}_SRCS} )

#-----------------------------------------------------------------------------
SRPlanMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
  
  
  
  