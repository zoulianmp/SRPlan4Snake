
include(${SRPlan_CMAKE_DIR}/SRPlanMacroBuildQtModule.cmake)

set(qtmodules
  Annotations
  Cameras
  Colors  
 # Data 
  DoubleArrays
  Units

  SubjectHierarchy
  Transforms


  #Markups
  PathPlan
  #Measurements
  Models
  ModelMirror
  ModelTransform
  #FiberBundles
  #SceneViews
  #SlicerWelcome
  SRPlanWelcome
  Tables
  Reformat
  #ViewControllers
  Volumes
  #VolumeRendering # Volume Rendering depends of Volumes
  Segmentations
 # SegmentComparison
  SegmentMorphology
  
  )
if(SRPlan_BUILD_CLI_SUPPORT)
  list(APPEND qtmodules
    CropVolume
    )
endif()
if(SRPlan_BUILD_DIFFUSION_SUPPORT)
  list(APPEND qtmodules
    TractographyDisplay
    TractographyInteractiveSeeding # tractography depends of Volumes
    )
endif()

foreach(module ${qtmodules})
  list(FIND SRPlan_QTLOADABLEMODULES_DISABLED ${module} _module_disabled)
  if(${_module_disabled} STREQUAL "-1")
    add_subdirectory(${module})
  endif()
endforeach(module)



# External modules have been moved to Modules/Remote/CMakeLists.txt
