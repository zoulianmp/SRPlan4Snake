#-----------------------------------------------------------------------------
set(MODULE_NAME "Segmentations")
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(MRML)
add_subdirectory(Logic)
add_subdirectory(MRMLDM)
add_subdirectory(SubjectHierarchyPlugins)
add_subdirectory(Widgets)

#-----------------------------------------------------------------------------


set(MODULE_EXPORT_DIRECTIVE "Q_SRPlan_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")



set(MODULE_INCLUDE_DIRECTORIES
  ${vtkSRPlanSegmentationsModuleMRML_INCLUDE_DIRS}
  ${vtkSRPlanSegmentationsModuleLogic_INCLUDE_DIRS}
  ${vtkSRPlanSegmentationsModuleMRMLDisplayableManager_INCLUDE_DIRS}
  ${qSRPlanSegmentationsModuleWidgets_INCLUDE_DIRS}
  ${qSRPlanSegmentationsSubjectHierarchyPlugins_INCLUDE_DIRS}
  )

  set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  qSlicer${MODULE_NAME}Reader.cxx
  qSlicer${MODULE_NAME}Reader.h
  
  qSRPlanSegmentationDisplaySettingsDialog.h
  qSRPlanSegmentationDisplaySettingsDialog.cxx
  
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  qSlicer${MODULE_NAME}Reader.h
  
  qSRPlanSegmentationDisplaySettingsDialog.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}Module.ui
  Resources/UI/qSRPlanSegmentationDisplaySettingsDialog.ui
  
  )

set(MODULE_TARGET_LIBRARIES
  vtkSRPlan${MODULE_NAME}ModuleMRML
  vtkSRPlan${MODULE_NAME}ModuleLogic
  vtkSRPlan${MODULE_NAME}ModuleMRMLDisplayableManager
  qSRPlan${MODULE_NAME}ModuleWidgets
  qSRPlan${MODULE_NAME}SubjectHierarchyPlugins
  )

set(MODULE_RESOURCES
  Resources/qSRPlan${MODULE_NAME}Module.qrc
  )

#-----------------------------------------------------------------------------
SRPlanMacroBuildQtModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
