project(qSRPlan${MODULE_NAME}SubjectHierarchyPlugins)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SRPlan_${MODULE_NAME_UPPER}_SUBJECT_HIERARCHY_PLUGINS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${vtkSRPlanSubjectHierarchyModuleLogic_INCLUDE_DIRS}
  ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS}
  ${MRMLCore_INCLUDE_DIRS}
  ${MRMLLogic_INCLUDE_DIRS}
  ${qMRMLWidgets_INCLUDE_DIRS}
  ${SlicerRtCommon_INCLUDE_DIRS}
  ${vtkSRPlanPathPlanModuleMRML_INCLUDE_DIRS}
  ${vtkSRPlanPathPlanModuleLogic_INCLUDE_DIRS}

  )

set(${KIT}_SRCS
  qSlicerSubjectHierarchyMarkupsPlugin.cxx
  qSlicerSubjectHierarchyMarkupsPlugin.h
 # qSlicerSubjectHierarchyDoseVolumeHistogramPlugin.h
 # qSlicerSubjectHierarchyDoseVolumeHistogramPlugin.cxx
 # qSlicerSubjectHierarchyIsodosePlugin.h
 # qSlicerSubjectHierarchyIsodosePlugin.cxx
  )

set(${KIT}_MOC_SRCS
  qSlicerSubjectHierarchyMarkupsPlugin.h
 # qSlicerSubjectHierarchyDoseVolumeHistogramPlugin.h
 # qSlicerSubjectHierarchyIsodosePlugin.h
  )

set(${KIT}_UI_SRCS
  )

set(${KIT}_RESOURCES
  Resources/qSRPlanPathPlanSubjectHierarchyPlugins.qrc
  )

  
  
  
#-----------------------------------------------------------------------------
set(${KIT}_TARGET_LIBRARIES
  vtkSRPlanSubjectHierarchyModuleLogic
  qSRPlanSubjectHierarchyModuleWidgets
  vtkSlicerRtCommon
  vtkSRPlanPathPlanModuleMRML
  vtkSRPlanPathPlanModuleLogic
  MRMLCore
  MRMLLogic
  qMRMLWidgets
  ${QT_LIBRARIES}
  )

#-----------------------------------------------------------------------------
SRPlanMacroBuildModuleQtLibrary(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
 # FOLDER "Module-${MODULE_NAME}"
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )

  
  
if(SRPlan_OUT_UNIFY)
   add_custom_command(
        TARGET ${KIT}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${KIT}> ${SRPlan_OUT_DesignerPlugin})

endif()
