cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (AnalysisSitus CXX)

# Install dir of the built project
set (INSTALL_DIR "" CACHE PATH "Where to install" )
set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )

# Path to data files for testing
set (TEST_DATA_DIR "" CACHE PATH "Directory with data for unit-testing" )

# Build configurations
set (CMAKE_CONFIGURATION_TYPES Release Debug RelWithDebInfo CACHE INTERNAL "" FORCE)

#-------------------------------------------------------------------------------
# Find 3rd-parties
#-------------------------------------------------------------------------------

include (${CMAKE_SOURCE_DIR}/cmake/asitus_3rdparty_macros.cmake)

ASITUS_RETRIEVE_VERSION(${CMAKE_SOURCE_DIR}/src/exe/exe_Version.h VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

message (STATUS "Analysis Situs VERSION_MAJOR: ${VERSION_MAJOR}")
message (STATUS "Analysis Situs VERSION_MINOR: ${VERSION_MINOR}")
message (STATUS "Analysis Situs VERSION_PATCH: ${VERSION_PATCH}")

# Set a directory containing all 3-rd parties
set (3RDPARTY_DIR "" CACHE PATH "The root directory for all 3-rd parties")

if (NOT DEFINED ${3RDPARTY_DIR} AND ${3RDPARTY_DIR} STREQUAL "")
    message (FATAL_ERROR "... root 3rdparty directory was not found")
endif()

# Qt
include (${CMAKE_SOURCE_DIR}/cmake/asitus_Qt.cmake)

# Active Data
include (${CMAKE_SOURCE_DIR}/cmake/asitus_ActiveData.cmake)

# Mobius
include (${CMAKE_SOURCE_DIR}/cmake/asitus_Mobius.cmake)

# Eigen
include (${CMAKE_SOURCE_DIR}/cmake/asitus_Eigen.cmake)

# VTK
include (${CMAKE_SOURCE_DIR}/cmake/asitus_VTK.cmake)

# OCCT
include (${CMAKE_SOURCE_DIR}/cmake/asitus_OCCT.cmake)

# OCCT 3rdparty
include (${CMAKE_SOURCE_DIR}/cmake/asitus_OCCT_3rdparty.cmake)

# Inno Setup for installation
include (${CMAKE_SOURCE_DIR}/cmake/asitus_InnoSetup.cmake)

set (3RDPARTY_DLL_PATH "${3RDPARTY_tcl_DLL_DIR};\
${3RDPARTY_tk_DLL_DIR};\
${3RDPARTY_freetype_DLL_DIR};\
${3RDPARTY_freeimage_DLL_DIR};\
${3RDPARTY_gl2ps_DLL_DIR};\
${3RDPARTY_tbb_DLL_DIR};\
${3RDPARTY_QT_DIR}/bin;")

set (3RDPARTY_DLL_DEB_PATH "${3RDPARTY_DLL_PATH}")

if (3RDPARTY_OCCT_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_OCCT_DLL_DIR_DEBUG}")
  set (3RDPARTY_DLL_DEB_PATH "${3RDPARTY_DLL_DEB_PATH};${3RDPARTY_OCCT_DLL_DIR_DEBUG}")
endif()

if (3RDPARTY_active_data_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_DLL_DIR_DEBUG}")
  set (3RDPARTY_DLL_DEB_PATH "${3RDPARTY_DLL_DEB_PATH};${3RDPARTY_active_data_DLL_DIR_DEBUG}")
endif()

if (3RDPARTY_vtk_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_DLL_DIR_DEBUG}")
  set (3RDPARTY_DLL_DEB_PATH "${3RDPARTY_DLL_DEB_PATH};${3RDPARTY_vtk_DLL_DIR_DEBUG}")
endif()

set (3RDPARTY_DLL_PATH "${3RDPARTY_DLL_PATH};\
${3RDPARTY_OCCT_DLL_DIR};\
${3RDPARTY_active_data_DLL_DIR};\
${3RDPARTY_vtk_DLL_DIR};")

#-------------------------------------------------------------------------------
string (REGEX REPLACE ";" " " 3RDPARTY_NOT_INCLUDED "${3RDPARTY_NOT_INCLUDED}")

# check all 3rdparty paths
if (3RDPARTY_NOT_INCLUDED)
  message (FATAL_ERROR "NOT FOUND: ${3RDPARTY_NOT_INCLUDED}")
endif()

#-------------------------------------------------------------------------------
# Executable
#-------------------------------------------------------------------------------

# Configure warnings level
if (MSVC)
  add_definitions (/W4)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHa")
elseif (CMAKE_COMPILER_IS_GNUCXX)
  add_definitions (-Wall -pedantic)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")  
else()
  message ("Unknown compiler")
endif()

#-------------------------------------------------------------------------------
# Output
#-------------------------------------------------------------------------------

# build directories
set (OS_WITH_BIT "${PLATFORM}${COMPILER_BITNESS}")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libi")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libi")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libd")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libd")

if (WIN32)
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
endif()

#-------------------------------------------------------------------------------
# Sub-projects
#-------------------------------------------------------------------------------

add_subdirectory(${CMAKE_SOURCE_DIR}/src/asiTcl)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/asiAlgo)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/asiData)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/asiVisu)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/asiEngine)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/asiUI)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/cmdMisc)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/exe)

#-------------------------------------------------------------------------------
# Setup compiler
#-------------------------------------------------------------------------------

if (NOT "${INNOSETUP_EXE}" STREQUAL "" AND EXISTS "${INNOSETUP_EXE}")
  add_subdirectory(${CMAKE_SOURCE_DIR}/setup)
endif()

#-------------------------------------------------------------------------------
# Folders
#-------------------------------------------------------------------------------

################################################################################
set_property(TARGET asiAlgo   PROPERTY FOLDER Framework)
set_property(TARGET asiData   PROPERTY FOLDER Framework)
set_property(TARGET asiVisu   PROPERTY FOLDER Framework)
set_property(TARGET asiEngine PROPERTY FOLDER Framework)
set_property(TARGET asiUI     PROPERTY FOLDER Framework)
################################################################################
set_property(TARGET exe       PROPERTY FOLDER Executables)
################################################################################
set_property(TARGET setup     PROPERTY FOLDER Utilities)
################################################################################
set_property(TARGET asiTcl    PROPERTY FOLDER Batch)
set_property(TARGET cmdMisc   PROPERTY FOLDER Commands)
################################################################################

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------

if (EXISTS "${3RDPARTY_DIR}/Redistributable/msvcp120.dll")
  install (FILES "${3RDPARTY_DIR}/Redistributable/msvcp120.dll" DESTINATION bin)
endif()

if (EXISTS "${3RDPARTY_DIR}/Redistributable/msvcr120.dll")
  install (FILES "${3RDPARTY_DIR}/Redistributable/msvcr120.dll" DESTINATION bin)
endif()
