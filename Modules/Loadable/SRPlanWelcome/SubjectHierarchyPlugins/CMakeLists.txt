project(qSRPlan${MODULE_NAME}SubjectHierarchyPlugins)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SRPlan_${MODULE_NAME_UPPER}_SUBJECT_HIERARCHY_PLUGINS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${SlicerRtCommon_INCLUDE_DIRS}
  ${vtkSRPlanSubjectHierarchyModuleLogic_INCLUDE_DIRS}
  ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS}
  ${MRMLCore_INCLUDE_DIRS}
  ${MRMLLogic_INCLUDE_DIRS}
  ${qMRMLWidgets_INCLUDE_DIRS}
 # ${MRMLCLI_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  qSlicerSubjectHierarchyDICOMPlugin.cxx
  qSlicerSubjectHierarchyDICOMPlugin.h
  
  qSlicerSubjectHierarchySRPlanPlugin.cxx
  qSlicerSubjectHierarchySRPlanPlugin.h
  
  qSlicerSubjectHierarchyRtImagePlugin.cxx
  qSlicerSubjectHierarchyRtImagePlugin.h
  qSlicerSubjectHierarchyRtDoseVolumePlugin.cxx
  qSlicerSubjectHierarchyRtDoseVolumePlugin.h
  qSlicerSubjectHierarchyRtPlanPlugin.cxx
  qSlicerSubjectHierarchyRtPlanPlugin.h
  qSlicerSubjectHierarchyRtBeamPlugin.cxx
  qSlicerSubjectHierarchyRtBeamPlugin.h
  )

set(${KIT}_MOC_SRCS
  qSlicerSubjectHierarchyDICOMPlugin.h
  qSlicerSubjectHierarchySRPlanPlugin.h
	
  qSlicerSubjectHierarchyRtImagePlugin.h
  qSlicerSubjectHierarchyRtDoseVolumePlugin.h
  qSlicerSubjectHierarchyRtPlanPlugin.h
  qSlicerSubjectHierarchyRtBeamPlugin.h
  )


set(${KIT}_UI_SRCS
  )

set(${KIT}_RESOURCES
  Resources/${KIT}.qrc
  )

SET (${KIT}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)

#-----------------------------------------------------------------------------
set(${KIT}_TARGET_LIBRARIES
  vtkSlicerRtCommon
  vtkSRPlanSubjectHierarchyModuleLogic
  qSRPlanSubjectHierarchyModuleWidgets
  MRMLCore
  MRMLLogic
 # MRMLCLI
  qMRMLWidgets
  ${QT_LIBRARIES}
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