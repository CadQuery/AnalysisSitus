project(asiTest)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiTest.h
  asiTest_CaseIDs.h
  asiTest_CommonFacilities.h
  asiTest_ProgressNotifier.h
  asiTest_TclTestCase.h
)
set (CPP_FILES
  asiTest_CommonFacilities.cpp
  asiTest_Main.cpp
  asiTest_ProgressNotifier.cpp
  asiTest_TclTestCase.cpp
)

#------------------------------------------------------------------------------
# Test cases
#------------------------------------------------------------------------------

set (cases_editing_H_FILES
  cases/editing/asiTest_InvertShells.h
  cases/editing/asiTest_KEV.h
  cases/editing/asiTest_RecognizeBlends.h
  cases/editing/asiTest_SuppressBlends.h
)
set (cases_editing_CPP_FILES
  cases/editing/asiTest_InvertShells.cpp
  cases/editing/asiTest_KEV.cpp
  cases/editing/asiTest_RecognizeBlends.cpp
  cases/editing/asiTest_SuppressBlends.cpp
)

set (cases_framework_H_FILES
  cases/framework/asiTest_DataDictionary.h
  cases/framework/asiTest_Utils.h
)
set (cases_framework_CPP_FILES
  cases/framework/asiTest_DataDictionary.cpp
  cases/framework/asiTest_Utils.cpp
)

set (cases_inspection_H_FILES
  cases/inspection/asiTest_AAG.h
  cases/inspection/asiTest_EdgeVexity.h
  cases/inspection/asiTest_IsContourClosed.h
)
set (cases_inspection_CPP_FILES
  cases/inspection/asiTest_AAG.cpp
  cases/inspection/asiTest_EdgeVexity.cpp
  cases/inspection/asiTest_IsContourClosed.cpp
)

#------------------------------------------------------------------------------
# Add sources
#------------------------------------------------------------------------------

foreach (FILE ${H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${cases_editing_H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files\\Cases\\Editing" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${cases_editing_CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files\\Cases\\Editing" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${cases_framework_H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files\\Cases\\Framework" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${cases_framework_CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files\\Cases\\Framework" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${cases_inspection_H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files\\Cases\\Inspection" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${cases_inspection_CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files\\Cases\\Inspection" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Resources
#------------------------------------------------------------------------------

source_group ("Resources" FILES resources/asiTestDictionary.xml)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiTest_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/cases;\
  ${CMAKE_CURRENT_SOURCE_DIR}/cases/editing;\
  ${CMAKE_CURRENT_SOURCE_DIR}/cases/framework;\
  ${CMAKE_CURRENT_SOURCE_DIR}/cases/inspection;")
#
set (asiTest_include_dir ${asiTest_include_dir_loc} PARENT_SCOPE)

include_directories ( ${asiTest_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiEngine_include_dir}
                      ${asiUI_include_dir}
                      ${asiVisu_include_dir}
                      ${asiTcl_include_dir}
                      ${asiTestEngine_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_mobius_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR}
                      ${3RDPARTY_tbb_INCLUDE_DIR} )

#------------------------------------------------------------------------------
# Create executable
#------------------------------------------------------------------------------

add_executable(asiTest ${src_files} ${RCC_FILE} resources/asiTestDictionary.xml)

set (X_COMPILER_BITNESS "x${COMPILER_BITNESS}")

COPY_RESOURCES_TO_BINARY_DIRS(resources/asiTestDictionary.xml)

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/exePROTOTYPE.vcxproj.user.in
               ${asiTest_BINARY_DIR}/asiTest.vcxproj.user @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/setenv_deb.bat
               ${asiTest_BINARY_DIR}/setenv_deb.bat @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/setenv_rel.bat
               ${asiTest_BINARY_DIR}/setenv_rel.bat @ONLY)

# Post build event: run tests.
add_custom_command(
        TARGET asiTest
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests."
        COMMAND IF 1==$<CONFIG:Release> ( call ${asiTest_BINARY_DIR}/setenv_rel.bat && $<TARGET_FILE:asiTest> )
        COMMAND IF 1==$<CONFIG:RelWithDebInfo> ( call ${asiTest_BINARY_DIR}/setenv_rel.bat && $<TARGET_FILE:asiTest> )
        COMMAND IF 1==$<CONFIG:Debug> ( call ${asiTest_BINARY_DIR}/setenv_deb.bat && $<TARGET_FILE:asiTest> )
    )

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(asiTest asiAlgo asiData asiEngine asiTestEngine asiTcl asiUI cmdEngine cmdMisc)

#------------------------------------------------------------------------------
# Installation
#------------------------------------------------------------------------------

install (TARGETS asiTest RUNTIME DESTINATION bin COMPONENT Runtime)
install (FILES resources/asiTestDictionary.xml DESTINATION bin/resources)
