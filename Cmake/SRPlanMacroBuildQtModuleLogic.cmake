################################################################################
#
#  Program: 3D SRPlan
#
#  Copyright (c) Kitware Inc.
#
#  See COPYRIGHT.txt
#  or http://www.SRPlan.org/copyright/copyright.txt for details.
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
#  and was partially funded by NIH grant 3P41RR013218-12S1
#
################################################################################

#This Module is used for Build Module Logic, and need to have QT support
#
#


macro(SRPlanMacroBuildQtModuleLogic)
  set(options
    DISABLE_WRAP_PYTHON
    NO_INSTALL
    )
  set(oneValueArgs
    NAME
    EXPORT_DIRECTIVE
    FOLDER
    )
  set(multiValueArgs
    SRCS
    INCLUDE_DIRECTORIES
    TARGET_LIBRARIES
    )
  CMAKE_PARSE_ARGUMENTS(MODULELOGIC
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )

  if(MODULELOGIC_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to SRPlanMacroBuildModuleLogic(): \"${MODULELOGIC_UNPARSED_ARGUMENTS}\"")
  endif()

  list(APPEND MODULELOGIC_INCLUDE_DIRECTORIES
    ${SRPlan_Libs_INCLUDE_DIRS}
    ${SRPlan_Base_INCLUDE_DIRS}
    ${SRPlan_ModuleLogic_INCLUDE_DIRS}
    ${SRPlan_ModuleMRML_INCLUDE_DIRS}
    )

  if(SRPlan_BUILD_CLI_SUPPORT)
    # Third-party library
    find_package(SRPlanExecutionModel REQUIRED ModuleDescriptionParser)
    list(APPEND MODULELOGIC_INCLUDE_DIRECTORIES
      ${ModuleDescriptionParser_INCLUDE_DIRS}
      )

    # Note: Linking against qSRPlanBaseQTCLI provides logic with
    #       access to the core application modulemanager. Using the module manager
    #       a module logic can then use the services provided by registrered
    #       command line module (CLI).

    list(APPEND MODULELOGIC_TARGET_LIBRARIES
      qSRPlanBaseQTCLI
      )
  else()
    list(APPEND MODULELOGIC_TARGET_LIBRARIES
      SRPlanBaseLogic
      MRMLDisplayableManager
      )
  endif()

  if(NOT DEFINED MODULELOGIC_FOLDER AND DEFINED MODULE_NAME)
    set(MODULELOGIC_FOLDER "Module-${MODULE_NAME}")
  endif()

  set(MODULELOGIC_NO_INSTALL_OPTION)
  if(MODULELOGIC_NO_INSTALL)
    set(MODULELOGIC_NO_INSTALL_OPTION "NO_INSTALL")
  endif()

  SRPlanMacroBuildModuleVTKLibrary(
    NAME ${MODULELOGIC_NAME}
    EXPORT_DIRECTIVE ${MODULELOGIC_EXPORT_DIRECTIVE}
    FOLDER ${MODULELOGIC_FOLDER}
    SRCS ${MODULELOGIC_SRCS}
    INCLUDE_DIRECTORIES ${MODULELOGIC_INCLUDE_DIRECTORIES}
    TARGET_LIBRARIES ${MODULELOGIC_TARGET_LIBRARIES}
    ${MODULELOGIC_NO_INSTALL_OPTION}
    )

  set_property(GLOBAL APPEND PROPERTY SRPlan_MODULE_LOGIC_TARGETS ${MODULELOGIC_NAME})

  #-----------------------------------------------------------------------------
  # Update SRPlan_ModuleLogic_INCLUDE_DIRS
  #-----------------------------------------------------------------------------
  set(SRPlan_ModuleLogic_INCLUDE_DIRS
    ${SRPlan_ModuleLogic_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "SRPlan Module logic includes" FORCE)

  # --------------------------------------------------------------------------
  # Python wrapping
  # --------------------------------------------------------------------------
  if(NOT ${MODULELOGIC_DISABLE_WRAP_PYTHON} AND VTK_WRAP_PYTHON AND BUILD_SHARED_LIBS)

    set(SRPlan_Wrapped_LIBRARIES
      SRPlanBaseLogicPythonD
      )

    foreach(library ${MODULELOGIC_TARGET_LIBRARIES})
      if(TARGET ${library}PythonD)
        list(APPEND SRPlan_Wrapped_LIBRARIES ${library}PythonD)
      endif()
    endforeach()

    SRPlanMacroPythonWrapModuleVTKLibrary(
      NAME ${MODULELOGIC_NAME}
      SRCS ${MODULELOGIC_SRCS}
      WRAPPED_TARGET_LIBRARIES ${SRPlan_Wrapped_LIBRARIES}
      RELATIVE_PYTHON_DIR "."
      )

    # Set python module logic output
    set_target_properties(${MODULELOGIC_NAME}Python ${MODULELOGIC_NAME}PythonD PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SRPlan_QTLOADABLEMODULES_BIN_DIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SRPlan_QTLOADABLEMODULES_LIB_DIR}"
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SRPlan_QTLOADABLEMODULES_LIB_DIR}"
      )

    if(NOT "${MODULELOGIC_FOLDER}" STREQUAL "")
      set_target_properties(${MODULELOGIC_NAME}Python PROPERTIES FOLDER ${MODULELOGIC_FOLDER})
      set_target_properties(${MODULELOGIC_NAME}PythonD PROPERTIES FOLDER ${MODULELOGIC_FOLDER})
    endif()

    # Export target
    set_property(GLOBAL APPEND PROPERTY SRPlan_TARGETS ${MODULELOGIC_NAME}Python ${MODULELOGIC_NAME}PythonD)
  endif()

endmacro()






macro(SRPlanMacroBuildLoadableModule)
  set(options
    NO_INSTALL
    NO_TITLE
    WITH_GENERIC_TESTS
    )
  set(oneValueArgs
    NAME
    TITLE
    EXPORT_DIRECTIVE
    )
  set(multiValueArgs
    SRCS
    MOC_SRCS
    UI_SRCS
    INCLUDE_DIRECTORIES
    TARGET_LIBRARIES
    RESOURCES
    )
  CMAKE_PARSE_ARGUMENTS(LOADABLEMODULE
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )

  if(LOADABLEMODULE_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to SRPlanMacroBuildLoadableModule(): \"${LOADABLEMODULE_UNPARSED_ARGUMENTS}\"")
  endif()

  set(MY_EXPORT_HEADER_PREFIX qSRPlan${LOADABLEMODULE_NAME}Module)
  message(STATUS "Configuring Loadable module: ${LOADABLEMODULE_NAME} [${MY_EXPORT_HEADER_PREFIX}Export.h]")

  # --------------------------------------------------------------------------
  # Sanity checks
  # --------------------------------------------------------------------------
  set(expected_defined_vars NAME EXPORT_DIRECTIVE)
  foreach(var ${expected_defined_vars})
    if(NOT DEFINED LOADABLEMODULE_${var})
      message(FATAL_ERROR "${var} is mandatory")
    endif()
  endforeach()

  if(NOT DEFINED LOADABLEMODULE_TITLE)
    set(LOADABLEMODULE_TITLE ${LOADABLEMODULE_NAME})
  endif()

  # --------------------------------------------------------------------------
  # Define library name
  # --------------------------------------------------------------------------
  set(lib_name qSRPlan${LOADABLEMODULE_NAME}Module)

  # --------------------------------------------------------------------------
  # Define Module title
  # --------------------------------------------------------------------------
  if(NOT LOADABLEMODULE_NO_TITLE)
    add_definitions(
      -DMODULE_TITLE="${LOADABLEMODULE_TITLE}"
      -DQTMODULE_TITLE="${LOADABLEMODULE_TITLE}" # For backward compatibility
      )
  endif()

  # --------------------------------------------------------------------------
  # Include dirs
  # --------------------------------------------------------------------------

  if(NOT DEFINED ${lib_name}_SOURCE_DIR)
    set(${lib_name}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)
  endif()

  if(NOT DEFINED ${lib_name}_BINARY_DIR)
    set(${lib_name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)
  endif()

  set(${lib_name}_INCLUDE_DIRS ${${lib_name}_SOURCE_DIR} ${${lib_name}_BINARY_DIR} CACHE INTERNAL "" FORCE)

  include_directories(
    ${${lib_name}_INCLUDE_DIRS}
    ${SRPlan_Libs_INCLUDE_DIRS}
    ${SRPlan_Base_INCLUDE_DIRS}
    ${SRPlan_ModuleLogic_INCLUDE_DIRS}
    ${SRPlan_ModuleMRML_INCLUDE_DIRS}
    ${LOADABLEMODULE_INCLUDE_DIRECTORIES}
    )

  #-----------------------------------------------------------------------------
  # Configure export header
  #-----------------------------------------------------------------------------
  set(MY_LIBRARY_EXPORT_DIRECTIVE ${LOADABLEMODULE_EXPORT_DIRECTIVE})
  set(MY_LIBNAME ${lib_name})

  # Sanity checks
  if(NOT EXISTS ${SRPlan_EXPORT_HEADER_TEMPLATE})
    message(FATAL_ERROR "error: SRPlan_EXPORT_HEADER_TEMPLATE doesn't exist: ${SRPlan_EXPORT_HEADER_TEMPLATE}")
  endif()

  configure_file(
    ${SRPlan_EXPORT_HEADER_TEMPLATE}
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  set(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  #-----------------------------------------------------------------------------
  # Sources
  #-----------------------------------------------------------------------------
  set(LOADABLEMODULE_MOC_OUTPUT)
  QT5_WRAP_CPP(LOADABLEMODULE_MOC_OUTPUT ${LOADABLEMODULE_MOC_SRCS})
  set(LOADABLEMODULE_UI_CXX)
  QT5_WRAP_UI(LOADABLEMODULE_UI_CXX ${LOADABLEMODULE_UI_SRCS})
  set(LOADABLEMODULE_QRC_SRCS)
  if(DEFINED LOADABLEMODULE_RESOURCES)
    QT5_ADD_RESOURCES(LOADABLEMODULE_QRC_SRCS ${LOADABLEMODULE_RESOURCES})
  endif()

  if(NOT EXISTS ${SRPlan_LOGOS_RESOURCE})
    message("Warning, SRPlan_LOGOS_RESOURCE doesn't exist: ${SRPlan_LOGOS_RESOURCE}")
  endif()
  QT5_ADD_RESOURCES(LOADABLEMODULE_QRC_SRCS ${SRPlan_LOGOS_RESOURCE})

  set_source_files_properties(
    ${LOADABLEMODULE_SRCS} # For now, let's prevent the module widget from being wrapped
    ${LOADABLEMODULE_UI_CXX}
    ${LOADABLEMODULE_MOC_OUTPUT}
    ${LOADABLEMODULE_QRC_SRCS}
    WRAP_EXCLUDE
    )

  # --------------------------------------------------------------------------
  # Source groups
  # --------------------------------------------------------------------------
  source_group("Resources" FILES
    ${LOADABLEMODULE_UI_SRCS}
    ${SRPlan_LOGOS_RESOURCE}
    ${LOADABLEMODULE_RESOURCES}
    )

  source_group("Generated" FILES
    ${LOADABLEMODULE_UI_CXX}
    ${LOADABLEMODULE_MOC_OUTPUT}
    ${LOADABLEMODULE_QRC_SRCS}
    ${dynamicHeaders}
    )

  # --------------------------------------------------------------------------
  # Build library
  #-----------------------------------------------------------------------------
  add_library(${lib_name}
    ${LOADABLEMODULE_SRCS}
    ${LOADABLEMODULE_MOC_OUTPUT}
    ${LOADABLEMODULE_UI_CXX}
    ${LOADABLEMODULE_QRC_SRCS}
    )

  # Set loadable modules output path
  set_target_properties(${lib_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SRPlan_QTLOADABLEMODULES_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SRPlan_QTLOADABLEMODULES_LIB_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SRPlan_QTLOADABLEMODULES_LIB_DIR}"
    )
  set_target_properties(${lib_name} PROPERTIES LABELS ${lib_name})

  target_link_libraries(${lib_name}
    ${SRPlan_GUI_LIBRARY}
    ${LOADABLEMODULE_TARGET_LIBRARIES}
    )

  # Apply user-defined properties to the library target.
  if(SRPlan_LIBRARY_PROPERTIES)
    set_target_properties(${lib_name} PROPERTIES ${SRPlan_LIBRARY_PROPERTIES})
  endif()

  # Folder
  if(NOT DEFINED LOADABLEMODULE_FOLDER AND DEFINED MODULE_NAME)
    set(LOADABLEMODULE_FOLDER "Module-${MODULE_NAME}")
  endif()
  if(NOT "${LOADABLEMODULE_FOLDER}" STREQUAL "")
    set_target_properties(${lib_name} PROPERTIES FOLDER ${LOADABLEMODULE_FOLDER})
  endif()

  set_property(GLOBAL APPEND PROPERTY SRPlan_MODULE_TARGETS ${lib_name})

  
  
  if(SRPlan_OUT_UNIFY)
    add_custom_command(
        TARGET  ${lib_name}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${lib_name}> ${SRPlan_OUT_QtLoadable})

  endif()

  
  # --------------------------------------------------------------------------
  # Install library
  # --------------------------------------------------------------------------
  if(NOT LOADABLEMODULE_NO_INSTALL)
    install(TARGETS ${lib_name}
      RUNTIME DESTINATION ${SRPlan_INSTALL_QTLOADABLEMODULES_BIN_DIR} COMPONENT RuntimeLibraries
      LIBRARY DESTINATION ${SRPlan_INSTALL_QTLOADABLEMODULES_LIB_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${SRPlan_INSTALL_QTLOADABLEMODULES_LIB_DIR} COMPONENT Development
      )
  endif()

  # --------------------------------------------------------------------------
  # Install headers
  # --------------------------------------------------------------------------
  if(DEFINED SRPlan_DEVELOPMENT_INSTALL)
    if(NOT DEFINED ${LOADABLEMODULE_NAME}_DEVELOPMENT_INSTALL)
      set(${LOADABLEMODULE_NAME}_DEVELOPMENT_INSTALL ${SRPlan_DEVELOPMENT_INSTALL})
    endif()
  else()
    if(NOT DEFINED ${LOADABLEMODULE_NAME}_DEVELOPMENT_INSTALL)
      set(${LOADABLEMODULE_NAME}_DEVELOPMENT_INSTALL OFF)
    endif()
  endif()

  if(NOT LOADABLEMODULE_NO_INSTALL AND ${LOADABLEMODULE_NAME}_DEVELOPMENT_INSTALL)
    # Install headers
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES
      ${headers}
      ${dynamicHeaders}
      DESTINATION ${SRPlan_INSTALL_QTLOADABLEMODULES_INCLUDE_DIR}/${LOADABLEMODULE_NAME} COMPONENT Development
      )
  endif()

  if(BUILD_TESTING AND LOADABLEMODULE_WITH_GENERIC_TESTS)

    set(KIT ${lib_name})

    set(KIT_GENERIC_TEST_SRCS)
    set(KIT_GENERIC_TEST_NAMES)
    set(KIT_GENERIC_TEST_NAMES_CXX)
    SRPlanMacroConfigureGenericCxxModuleTests(${LOADABLEMODULE_NAME}
      KIT_GENERIC_TEST_SRCS
      KIT_GENERIC_TEST_NAMES
      KIT_GENERIC_TEST_NAMES_CXX
      )

    set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "DEBUG_LEAKS_ENABLE_EXIT_ERROR();" )
    create_test_sourcelist(Tests ${KIT}CxxTests.cxx
      ${KIT_GENERIC_TEST_NAMES_CXX}
      EXTRA_INCLUDE vtkMRMLDebugLeaksMacro.h
      )

    add_executable(${KIT}GenericCxxTests ${Tests})
    set_target_properties(${KIT}GenericCxxTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${SRPlan_BIN_DIR})
    target_link_libraries(${KIT}GenericCxxTests ${KIT})
    if(NOT "${LOADABLEMODULE_FOLDER}" STREQUAL "")
      set_target_properties(${KIT}GenericCxxTests PROPERTIES FOLDER ${LOADABLEMODULE_FOLDER})
    endif()

    foreach(testname ${KIT_GENERIC_TEST_NAMES})
      add_test(NAME ${testname}
               COMMAND ${SRPlan_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}GenericCxxTests> ${testname})
      set_property(TEST ${testname} PROPERTY LABELS ${KIT})
    endforeach()
  endif()

endmacro()


#
# Deprecated macro - Will be removed for 4.4
#
macro(SRPlanMacroBuildQtModule)
  set(options
    NO_INSTALL
    NO_TITLE
    WITH_GENERIC_TESTS
    )
  set(oneValueArgs
    NAME
    TITLE
    EXPORT_DIRECTIVE
    )
  set(multiValueArgs
    SRCS
    MOC_SRCS
    UI_SRCS
    INCLUDE_DIRECTORIES
    TARGET_LIBRARIES
    RESOURCES
    )
  CMAKE_PARSE_ARGUMENTS(QTMODULE
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )

  #message(AUTHOR_WARNING
  #  "Macro 'SRPlanMacroBuildQtModule' is deprecated, "
  #  "consider using macro 'SRPlanMacroBuildLoadableModule'."
  #  "See http://www.na-mic.org/Bug/view.php?id=2648"
  #  )

  set(QTMODULE_ARGS)
  foreach(arg ${oneValueArgs} ${multiValueArgs})
    list(APPEND QTMODULE_ARGS ${arg} ${QTMODULE_${arg}})
  endforeach()

  set(QTMODULE_OPTIONS)
  foreach(option ${options})
    if(${QTMODULE_${option}})
      list(APPEND QTMODULE_OPTIONS ${option})
    endif()
  endforeach()

  SRPlanMacroBuildLoadableModule(${QTMODULE_ARGS} ${QTMODULE_OPTIONS})
endmacro()
