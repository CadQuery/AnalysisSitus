project (asiTestEngine)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiTestEngine.h
  asiTestEngine_Launcher.h
  asiTestEngine_FileComparator.h
  asiTestEngine_Macro.h
  asiTestEngine_TestCase.h
  outcome.h
)

set (CPP_FILES
  asiTestEngine_FileComparator.cpp
  asiTestEngine_Launcher.cpp
  asiTestEngine_TestCase.cpp
)

#------------------------------------------------------------------------------
# Report generator
#------------------------------------------------------------------------------

set (reportgen_H_FILES
  reportgen/asiTestEngine_DescriptionProc.h
  reportgen/asiTestEngine_ReportRenderer.h
  reportgen/asiTestEngine_ReportStyle.h
  reportgen/asiTestEngine_ReportStyleFactory.h
  reportgen/asiTestEngine_ReportTag.h
  reportgen/asiTestEngine_ReportTagFactory.h
)

set (reportgen_CPP_FILES
  reportgen/asiTestEngine_DescriptionProc.cpp
  reportgen/asiTestEngine_ReportRenderer.cpp
  reportgen/asiTestEngine_ReportStyle.cpp
  reportgen/asiTestEngine_ReportStyleFactory.cpp
  reportgen/asiTestEngine_ReportTag.cpp
  reportgen/asiTestEngine_ReportTagFactory.cpp
)

#------------------------------------------------------------------------------

# Create include variable
set (asiTestEngine_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/reportgen")
#
set (asiTestEngine_include_dir ${asiTestEngine_include_dir_loc} PARENT_SCOPE)

foreach (FILE ${H_FILES})
  source_group ("Header Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${CPP_FILES})
  source_group ("Source Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${reportgen_H_FILES})
  source_group ("Header Files\\Reporting" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${reportgen_CPP_FILES})
  source_group ("Source Files\\Reporting" FILES "${FILE}")
endforeach (FILE)

include_directories ( SYSTEM
                      ${asiTestEngine_include_dir_loc}
                      ${asiData_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR} )

add_library (asiTestEngine SHARED
  ${H_FILES}           ${CPP_FILES}
  ${reportgen_H_FILES} ${reportgen_CPP_FILES})

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(asiTestEngine asiAlgo asiData)

#------------------------------------------------------------------------------
# Installation
#------------------------------------------------------------------------------

install (TARGETS asiTestEngine
         CONFIGURATIONS Release
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         LIBRARY DESTINATION framework/lib COMPONENT Library)

install (TARGETS asiTestEngine
         CONFIGURATIONS RelWithDebInfo
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         LIBRARY DESTINATION framework/libi COMPONENT Library)

install (TARGETS asiTestEngine
         CONFIGURATIONS Debug
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         LIBRARY DESTINATION framework/libd COMPONENT Library)

if (MSVC)
  install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiTestEngine.pdb DESTINATION bind CONFIGURATIONS Debug)
  install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiTestEngine.pdb DESTINATION bini CONFIGURATIONS RelWithDebInfo)
endif()

install (FILES ${H_FILES}           DESTINATION framework/include)
install (FILES ${reportgen_H_FILES} DESTINATION framework/include)
