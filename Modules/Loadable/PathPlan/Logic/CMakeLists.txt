project(vtkSRPlan${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SRPlan_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
    ${SRPlanBaseLogic_INCLUDE_DIRECTORIES}
	${MRMLLogic_INCLUDE_DIRECTORIES}
	${vtkSlicerRtCommon_INCLUDE_DIRECTORIES}
	${vtkSRPlanSegmentationsModuleLogic_INCLUDE_DIRECTORIES}

	${vtkSRPlanVolumesModuleLogic_INCLUDE_DIRECTORIES}
  )

  
set( SeedLibSrc 
     libbrachy.h
	 read_seeds.cxx 
	 seed_pdose.cxx 
	 v_interp.cxx
	 bin_search.cxx
	 get_phys_dat_dir.cxx
	 vtkIr192SeedSource.cxx
)

  
  
set(${KIT}_SRCS
  vtkSRPlan${MODULE_NAME}ModuleLogic.cxx
  vtkSRPlan${MODULE_NAME}ModuleLogic.h
 # vtkMarkupsGlyphSource2D.h
 # vtkMarkupsGlyphSource2D.cxx
  vtkSlicerRTScalarBarActor.h
  vtkSlicerRTScalarBarActor.cxx
  
  vtkSRPlanBDoseCalculateLogic.h
  vtkSRPlanBDoseCalculateLogic.cxx
  
  vtkSlicerIsodoseLogic.h
  vtkSlicerIsodoseLogic.cxx
  vtkSlicerDoseVolumeHistogramLogic.h
  vtkSlicerDoseVolumeHistogramLogic.cxx
  vtkSlicerMarkupsLogic.h
  vtkSlicerMarkupsLogic.cxx
  
  ${SeedLibSrc}
  )

set(${KIT}_TARGET_LIBRARIES
  MRMLLogic
  SRPlanBaseLogic
  #vtkSRPlan${MODULE_NAME}ModuleMRML
  vtkSRPlanAnnotationsModuleMRML
  vtkSlicerRtCommon
  vtkSRPlanSegmentationsModuleLogic
  vtkSRPlanVolumesModuleLogic
  )

#-----------------------------------------------------------------------------
SRPlanMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )

  
if(SRPlan_OUT_UNIFY)
   add_custom_command(
        TARGET ${KIT}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${KIT}> ${SRPlan_OUT_QtLoadable})

endif()


