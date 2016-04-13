project(vtkSegmentationCore)
# Called in a different name than the folder so that Slicer does not want to handle it as a module (see hard-coded "Lib" postfixes in qSlicerUtils.cxx)

# --------------------------------------------------------------------------
# Configure headers
# --------------------------------------------------------------------------
set(configure_header_file vtkSegmentationCoreConfigure.h)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${configure_header_file}.in
  ${CMAKE_CURRENT_BINARY_DIR}/${configure_header_file}
  )

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------

SET (vtkSegmentationCore_SRCS 
  vtkOrientedImageData.cxx
  vtkOrientedImageData.h
  vtkOrientedImageDataResample.cxx
  vtkOrientedImageDataResample.h
  vtkSegment.cxx
  vtkSegment.h
  vtkSegmentation.cxx
  vtkSegmentation.h
  vtkSegmentationConverter.cxx
  vtkSegmentationConverter.h
  vtkSegmentationConverterFactory.cxx
  vtkSegmentationConverterFactory.h
  vtkSegmentationConverterRule.cxx
  vtkSegmentationConverterRule.h
  vtkTopologicalHierarchy.cxx
  vtkTopologicalHierarchy.h
  vtkBinaryLabelmapToClosedSurfaceConversionRule.cxx
  vtkBinaryLabelmapToClosedSurfaceConversionRule.h
  vtkClosedSurfaceToBinaryLabelmapConversionRule.cxx
  vtkClosedSurfaceToBinaryLabelmapConversionRule.h
  vtkCalculateOversamplingFactor.cxx
  vtkCalculateOversamplingFactor.h
  vtkPlanarContourToClosedSurfaceConversionRule.cxx
  vtkPlanarContourToClosedSurfaceConversionRule.h
  )

# Abstract/pure virtual classes

#set_source_files_properties(
#  ABSTRACT
#  )
  
SET (vtkSegmentationCore_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Slicer_Libs_INCLUDE_DIRS} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
# --------------------------------------------------------------------------

set(lib_name ${PROJECT_NAME})
SET (vtkSegmentationCore_LIBS ${vtkSegmentationCore_LIBS}
  ${VTK_LIBRARIES}
  MRMLCore
  vtkAddon
  )


set(include_dirs
  ${MRMLCore_INCLUDE_DIRS}
  ${vtkSegmentationCore_INCLUDE_DIRS}
  ${vtkAddon_INCLUDE_DIRS}
  )

include_directories(${include_dirs})

#INCLUDE_DIRECTORIES( ${vtkSegmentationCore_INCLUDE_DIRS} )

ADD_LIBRARY(${lib_name} ${vtkSegmentationCore_SRCS})
TARGET_LINK_LIBRARIES( ${lib_name} ${vtkSegmentationCore_LIBS} )


# Apply user-defined properties to the library target.
if(SRPlan_LIBRARY_PROPERTIES)
  set_target_properties(${lib_name} PROPERTIES ${SRPlan_LIBRARY_PROPERTIES})
endif()

if(SRPlan_OUT_UNIFY)
   add_custom_command(
        TARGET ${lib_name}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${lib_name}> ${SRPlan_OUT_Library})

endif()


IF (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT WIN32)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF()

# --------------------------------------------------------------------------
# Export target
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_EXPORT_FILE)
  set(${PROJECT_NAME}_EXPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)
endif()
export(TARGETS ${lib_name} APPEND FILE ${${PROJECT_NAME}_EXPORT_FILE})

# --------------------------------------------------------------------------
# Install library
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_BIN_DIR)
  set(${PROJECT_NAME}_INSTALL_BIN_DIR bin)
endif()
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_LIB_DIR)
  set(${PROJECT_NAME}_INSTALL_LIB_DIR lib/${PROJECT_NAME})
endif()


install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR} COMPONENT Development
  ) 
  
# --------------------------------------------------------------------------
# Set INCLUDE_DIRS variable
# --------------------------------------------------------------------------
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "${PROJECT_NAME} include dirs" FORCE)

# --------------------------------------------------------------------------
# Configure support file
# --------------------------------------------------------------------------
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkSegmentationCoreConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkSegmentationCoreConfig.cmake
  )
