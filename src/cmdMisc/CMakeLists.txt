project(cmdMisc)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  cmdMisc.h
)
set (CPP_FILES
  cmdMisc.cpp
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

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (cmdMisc_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};")
#
set (cmdMisc_include_dir ${cmdMisc_include_dir_loc} PARENT_SCOPE)

include_directories ( ${cmdMisc_include_dir_loc}
                      ${asiTcl_include_dir}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
                      ${asiEngine_include_dir}
                      ${asiUI_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_mobius_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR}
                      ${3RDPARTY_tbb_INCLUDE_DIR} )

#------------------------------------------------------------------------------
# Create library
#------------------------------------------------------------------------------

add_library (cmdMisc SHARED
  ${H_FILES} ${CPP_FILES}
)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(cmdMisc asiTcl asiAlgo asiData asiVisu asiEngine asiUI)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS cmdMisc RUNTIME DESTINATION bin COMPONENT Runtime)
