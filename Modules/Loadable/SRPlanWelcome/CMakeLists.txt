
#-----------------------------------------------------------------------------
set(MODULE_NAME Welcome)
set(MODULE_TITLE "Welcome to SRPlan")

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)



add_subdirectory(SubjectHierarchyPlugins)






#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SRPlan_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Additional includes - Current_{source,binary} and SRPlan_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${qSRPlanCameras_INCLUDE_DIRS}
  ${vtkSRPlanCamerasModuleLogic_INCLUDE_DIRS}
  ${vtkSRPlanColorsModuleLogic_INCLUDE_DIRS}
  ${vtkSRPlanSubjectHierarchyModuleLogic_INCLUDE_DIRS}
  ${qMRMLWidgets_INCLUDE_DIRS}
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  
  qSRPlanNewPatientDialog.h
  qSRPlanNewPatientDialog.cxx
  
  qSlicerSceneIOOptionsWidget.cxx
  qSlicerSceneIOOptionsWidget.h
  qSlicerSceneReader.cxx
  qSlicerSceneReader.h
  qSlicerSceneWriter.cxx
  qSlicerSceneWriter.h
  
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  
  qSRPlanNewPatientDialog.h

  qSlicerSceneIOOptionsWidget.h
  qSlicerSceneReader.h
  qSlicerSceneWriter.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}ModuleWidget.ui
  Resources/UI/qSlicerSceneIOOptionsWidget.ui
  Resources/UI/qSRPlanNewPatientDialog.ui
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

set(MODULE_TARGET_LIBRARIES
   vtkSRPlanCamerasModuleLogic
   vtkSRPlanColorsModuleLogic
   vtkSRPlanSubjectHierarchyModuleLogic
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

