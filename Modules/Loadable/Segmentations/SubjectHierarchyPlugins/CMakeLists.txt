project(qSRPlan${MODULE_NAME}SubjectHierarchyPlugins)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SRPlan_${MODULE_NAME_UPPER}_PLUGINS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${SlicerRtCommon_INCLUDE_DIRS}
  ${vtkSRPlanSegmentationsModuleMRML_INCLUDE_DIRS}
  ${vtkSRPlanSegmentationsModuleLogic_INCLUDE_DIRS}
  ${qSRPlanSegmentationsModule_INCLUDE_DIRS}
  ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS}
  ${MRMLCore_INCLUDE_DIRS}
  ${SRPlan_Base_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  qSlicerSubjectHierarchySegmentationsPlugin.cxx
  qSlicerSubjectHierarchySegmentationsPlugin.h
  qSlicerSubjectHierarchySegmentsPlugin.cxx
  qSlicerSubjectHierarchySegmentsPlugin.h
  )

set(${KIT}_MOC_SRCS
  qSlicerSubjectHierarchySegmentationsPlugin.h
  qSlicerSubjectHierarchySegmentsPlugin.h
  )

set(${KIT}_UI_SRCS
  )

set(${KIT}_RESOURCES
  Resources/${KIT}.qrc
  )

#   ${vtkSRPlanSubjectHierarchyModuleLogic_INCLUDE_DIRS}
#  ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS}

SET (${KIT}_INCLUDE_DIRS 
     ${CMAKE_CURRENT_SOURCE_DIR} 
     ${CMAKE_CURRENT_BINARY_DIR} 
     ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS} 
	 ${vtkSRPlanSubjectHierarchyModuleLogic_INCLUDE_DIRS}
     ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS}
     ${MRMLCore_INCLUDE_DIRS}
     ${MRMLLogic_INCLUDE_DIRS}
     ${qMRMLWidgets_INCLUDE_DIRS}
	 ${vtkSRPlanSegmentationsModuleMRML_INCLUDE_DIRS}
     CACHE INTERNAL "" FORCE)




	 


#-----------------------------------------------------------------------------
set(${KIT}_TARGET_LIBRARIES
  vtkSlicerRtCommon
  vtkSRPlanSegmentationsModuleMRML
  vtkSRPlanSegmentationsModuleLogic
  qSRPlanSubjectHierarchyModuleWidgets
  MRMLCore
  )

#-----------------------------------------------------------------------------
SRPlanMacroBuildModuleQtLibrary(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )