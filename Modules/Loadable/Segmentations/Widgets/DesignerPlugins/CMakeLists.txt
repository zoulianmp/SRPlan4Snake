set(KIT ${PROJECT_NAME}Plugins)

set(${KIT}_EXPORT "Q_SRPlan_MODULE_${MODULE_NAME_UPPER}_WIDGETS_PLUGINS_EXPORT")

set(${KIT}_SRCS
  qSlicerSegmentationsModuleWidgetsAbstractPlugin.cxx
  qSlicerSegmentationsModuleWidgetsAbstractPlugin.h
  qSlicerSegmentationsModuleWidgetsPlugin.cxx
  qSlicerSegmentationsModuleWidgetsPlugin.h
  qMRMLSegmentsTableViewPlugin.cxx
  qMRMLSegmentsTableViewPlugin.h
  qMRMLSegmentationRepresentationsListViewPlugin.cxx
  qMRMLSegmentationRepresentationsListViewPlugin.h
  qMRMLSegmentationConversionParametersWidgetPlugin.cxx
  qMRMLSegmentationConversionParametersWidgetPlugin.h
  qMRMLSegmentSelectorWidgetPlugin.cxx
  qMRMLSegmentSelectorWidgetPlugin.h
  
  qMRMLSegmentsEditorWidgetPlugin.h
  qMRMLSegmentsEditorWidgetPlugin.cxx
  
#  qMRMLWindowLevelPanelPlugin.h
#  qMRMLWindowLevelPanelPlugin.cxx   
  
  
  )

set(${KIT}_MOC_SRCS
  qSlicerSegmentationsModuleWidgetsPlugin.h
  qMRMLSegmentsTableViewPlugin.h
  qMRMLSegmentationRepresentationsListViewPlugin.h
  qMRMLSegmentationConversionParametersWidgetPlugin.h
  qMRMLSegmentSelectorWidgetPlugin.h
  qMRMLSegmentsEditorWidgetPlugin.h
  
 # qMRMLWindowLevelPanelPlugin.h 
  
  )

set(${KIT}_TARGET_LIBRARIES
  qSRPlanSegmentationsModuleWidgets
  )

#-----------------------------------------------------------------------------
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Slicer_DIR}/bin)

ctkMacroBuildQtDesignerPlugin(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )


  
  
if(SRPlan_OUT_UNIFY)
   add_custom_command(
        TARGET ${KIT}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${KIT}> ${SRPlan_OUT_DesignerPlugin})

endif()

