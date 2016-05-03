


project(SelfDebug)


#-----------------------------------------------------------------------------
set(${KIT}Testing_ITK_COMPONENTS
  ITKCommon
  )
find_package(ITK 4.6 COMPONENTS ${${KIT}Testing_ITK_COMPONENTS} REQUIRED)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER 1) # See Libs/ITKFactoryRegistration/CMakeLists.txt
list(APPEND ITK_LIBRARIES ITKFactoryRegistration)
list(APPEND ITK_INCLUDE_DIRS
  ${ITKFactoryRegistration_INCLUDE_DIRS}
  )
include(${ITK_USE_FILE})


set(TEST_SOURCES
  qMRMLNodeComboBoxTest1.cxx
  qMRMLNodeComboBoxTest2.cxx
  qMRMLNodeComboBoxTest3.cxx
  qMRMLNodeComboBoxTest4.cxx
  qMRMLNodeComboBoxTest5.cxx
  qMRMLNodeComboBoxTest6.cxx
  qMRMLNodeComboBoxTest7.cxx
  qMRMLNodeComboBoxTest8.cxx
  qMRMLNodeComboBoxLazyUpdateTest1.cxx
  )



SET (Includedir 
     ${CMAKE_CURRENT_SOURCE_DIR}
	 ${CMAKE_CURRENT_BINARY_DIR}
     ${qMRMLWidgets_INCLUDE_DIRS}
	 ${MRMLCore_INCLUDE_DIRS}
)

include_directories( ${Includedir})

SET(Libs
    qMRMLWidgets
	MRMLCore
   )

add_executable(qMRMLNodeComboBoxTest qMRMLNodeComboBoxTest.cxx)
target_link_libraries(qMRMLNodeComboBoxTest ${Libs} )
