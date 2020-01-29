project(asiTcl)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiTcl.h
  asiTcl_CommandInfo.h
  asiTcl_Interp.h
  asiTcl_Plugin.h
  asiTcl_PluginMacro.h
)

set (CPP_FILES
  asiTcl_Interp.cpp
  asiTcl_Plugin.cpp
)

#------------------------------------------------------------------------------
# Libraries
#------------------------------------------------------------------------------

set (OCCT_LIB_FILES
  TKernel
  TKMath
)

if (WIN32)
  set (TCL_LIB_FILES
    tcl86
  )
else()
  set (TCL_LIB_FILES
    tcl8.6
  )
endif()

#------------------------------------------------------------------------------
# Add sources
#------------------------------------------------------------------------------

foreach (FILE ${H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${CPP_FILES})
  source_group ("Source Files" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiTcl_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};")
#
set (asiTcl_include_dir ${asiTcl_include_dir_loc} PARENT_SCOPE)

include_directories ( SYSTEM
                      ${asiTcl_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_tcl_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR} )

#------------------------------------------------------------------------------
# Create library
#------------------------------------------------------------------------------

add_library (asiTcl SHARED
  ${H_FILES} ${CPP_FILES}
)

target_link_libraries(asiTcl asiAlgo)

#------------------------------------------------------------------------------
# Set linker inputs
#------------------------------------------------------------------------------

foreach (LIB_FILE ${OCCT_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_OCCT_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiTcl debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiTcl optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiTcl ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${TCL_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  target_link_libraries (asiTcl ${3RDPARTY_tcl_LIBRARY_DIR}/${LIB_FILENAME})
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiTcl RUNTIME DESTINATION bin LIBRARY DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiTcl
         CONFIGURATIONS Release
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         LIBRARY DESTINATION framework/lib COMPONENT Library)

install (TARGETS asiTcl
         CONFIGURATIONS RelWithDebInfo
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         LIBRARY DESTINATION framework/libi COMPONENT Library)

install (TARGETS asiTcl
         CONFIGURATIONS Debug
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         LIBRARY DESTINATION framework/libd COMPONENT Library)

install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiTcl.pdb DESTINATION framework/bind CONFIGURATIONS Debug)

install (FILES ${H_FILES} DESTINATION framework/include)
