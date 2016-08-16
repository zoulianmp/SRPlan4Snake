
#-----------------------------------------------------------------------------
set(MODULE_NAME "PathPlan")
set(MODULE_TITLE "${MODULE_NAME}")

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(MRML)
add_subdirectory(Logic)
add_subdirectory(MRMLDM)
#add_subdirectory(SubjectHierarchyPlugins)
add_subdirectory(Widgets)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SRPlan_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Additional includes - Current_{source,binary} and SRPlan_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/MRML
  ${CMAKE_CURRENT_BINARY_DIR}/MRML
#  ${CMAKE_CURRENT_SOURCE_DIR}/SubjectHierarchyPlugins
#  ${CMAKE_CURRENT_BINARY_DIR}/SubjectHierarchyPlugins
  ${qSRPlanSubjectHierarchyModuleWidgets_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  )

set(MODULE_SRCS
  qSRPlan${MODULE_NAME}Module.cxx
  qSRPlan${MODULE_NAME}Module.h
  qSRPlan${MODULE_NAME}ModuleWidget.cxx
  qSRPlan${MODULE_NAME}ModuleWidget.h
  qSlicerMarkupsReader.h
  qSlicerMarkupsReader.cxx
  )

set(MODULE_MOC_SRCS
  qSRPlan${MODULE_NAME}Module.h
  qSRPlan${MODULE_NAME}ModuleWidget.h
  qSlicerMarkupsReader.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSRPlan${MODULE_NAME}Module.ui
  #Resources/UI/qSlicer${MODULE_NAME}SettingsPanel.ui
  )

set(MODULE_TARGET_LIBRARIES
  vtkSRPlan${MODULE_NAME}ModuleMRML
  vtkSRPlan${MODULE_NAME}ModuleMRMLDisplayableManager
  vtkSRPlan${MODULE_NAME}ModuleLogic
  qSRPlan${MODULE_NAME}ModuleWidgets
#  qSRPlan${MODULE_NAME}SubjectHierarchyPlugins
  )

set(MODULE_RESOURCES
  Resources/qSRPlan${MODULE_NAME}Module.qrc
  )

#-----------------------------------------------------------------------------
SRPlanMacroBuildLoadableModule(
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
