project(qSRPlan${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SRPlan_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${vtkSRPlanSegmentationsModuleMRML_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  qMRMLSegmentsTableView.cxx
  qMRMLSegmentsTableView.h
  qMRMLSegmentationRepresentationsListView.cxx
  qMRMLSegmentationRepresentationsListView.h
  qMRMLSegmentationConversionParametersWidget.cxx
  qMRMLSegmentationConversionParametersWidget.h
  qMRMLSegmentSelectorWidget.cxx
  qMRMLSegmentSelectorWidget.h
  qMRMLDoubleSpinBoxDelegate.cxx
  qMRMLDoubleSpinBoxDelegate.h
  
  qMRMLSegmentsEditorWidget.h
  qMRMLSegmentsEditorWidget.cxx
  
  )

set(${KIT}_MOC_SRCS
  qMRMLSegmentsTableView.h
  qMRMLSegmentationRepresentationsListView.h
  qMRMLSegmentationConversionParametersWidget.h
  qMRMLSegmentSelectorWidget.h
  qMRMLDoubleSpinBoxDelegate.h

   qMRMLSegmentsEditorWidget.h
  )

set(${KIT}_UI_SRCS
  Resources/UI/qMRMLSegmentsTableView.ui
  Resources/UI/qMRMLSegmentationRepresentationsListView.ui
  Resources/UI/qMRMLSegmentationConversionParametersWidget.ui
  Resources/UI/qMRMLSegmentSelectorWidget.ui
  Resources/UI/qMRMLSegmentsEditorWidget.ui
  
  )

set(${KIT}_RESOURCES
  Resources/${KIT}.qrc
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSRPlanSegmentationsModuleMRML
  )

SET (${KIT}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)
  
#-----------------------------------------------------------------------------
SRPlanMacroBuildModuleWidgets(
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

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------
add_subdirectory(DesignerPlugins)
