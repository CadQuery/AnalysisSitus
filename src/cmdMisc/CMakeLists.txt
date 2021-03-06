project(cmdMisc)

#------------------------------------------------------------------------------
# Mobius libraries
#------------------------------------------------------------------------------

if (USE_MOBIUS)
  set (MOBIUS_LIB_FILES
    mobiusBSpl
    mobiusCascade
    mobiusCore
    mobiusGeom
  )
endif()

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  cmdMisc.h
)
set (CPP_FILES
  cmdMisc.cpp
  cmdMisc_Coons.cpp
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

include_directories ( SYSTEM
                      ${cmdMisc_include_dir_loc}
                      ${asiTcl_include_dir}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
                      ${asiEngine_include_dir}
                      ${asiUI_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR} )

if (USE_MOBIUS)
  include_directories(SYSTEM ${3RDPARTY_mobius_INCLUDE_DIR})
endif()

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

if (USE_MOBIUS)
  foreach (LIB_FILE ${MOBIUS_LIB_FILES})
    if (WIN32)
      set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    else()
      set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endif()

    if (3RDPARTY_mobius_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_mobius_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
      target_link_libraries (cmdMisc debug ${3RDPARTY_mobius_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
      target_link_libraries (cmdMisc optimized ${3RDPARTY_mobius_LIBRARY_DIR}/${LIB_FILENAME})
    else()
      target_link_libraries (cmdMisc ${3RDPARTY_mobius_LIBRARY_DIR}/${LIB_FILENAME})
    endif()
  endforeach()
endif()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS cmdMisc RUNTIME DESTINATION bin LIBRARY DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS cmdMisc
         CONFIGURATIONS Release
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         LIBRARY DESTINATION framework/lib COMPONENT Library)

install (TARGETS cmdMisc
         CONFIGURATIONS RelWithDebInfo
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         LIBRARY DESTINATION framework/libi COMPONENT Library)

install (TARGETS cmdMisc
         CONFIGURATIONS Debug
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         LIBRARY DESTINATION framework/libd COMPONENT Library)

if (MSVC)
  install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/cmdMisc.pdb DESTINATION framework/bind CONFIGURATIONS Debug)
endif()
