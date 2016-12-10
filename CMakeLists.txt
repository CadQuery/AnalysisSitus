cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (AnalysisSitus CXX)

# Install dir of the built project
set (INSTALL_DIR "" CACHE PATH "Where to install" )
set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )

# Build configurations
set (CMAKE_CONFIGURATION_TYPES Release Debug RelWithDebInfo CACHE INTERNAL "" FORCE)

#-------------------------------------------------------------------------------
# Macro and functions
#-------------------------------------------------------------------------------

include (${CMAKE_SOURCE_DIR}/cmake/asitus_3rdparty_macros.cmake)

#-------------------------------------------------------------------------------
# Find 3rd-parties
#-------------------------------------------------------------------------------

# Set a directory containing all 3-rd parties
set (3RDPARTY_DIR "" CACHE PATH "The root directory for all 3-rd parties")

# Qt
include (${CMAKE_SOURCE_DIR}/cmake/asitus_Qt.cmake)

# Active Data
include (${CMAKE_SOURCE_DIR}/cmake/asitus_ActiveData.cmake)

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

# Qt is searched manually first
ASITUS_THIRDPARTY_PRODUCT("QT" "" "" "QtCore4.lib")
#
list ( APPEND 3RDPARTY_INCLUDE_DIRS ${3RDPARTY_QT_INCLUDE_DIR}/QtCore
                                    ${3RDPARTY_QT_INCLUDE_DIR}/QtGui )

# Now set CMAKE_PREFIX_PATH to point to local Qt installation.
# Without this setting find_package() will not work
set(CMAKE_PREFIX_PATH ${3RDPARTY_QT_DIR}/bin)

# Now we can apply standard CMake finder for Qt4. We do this mostly
# to have qt4_wrap_cpp() function available
find_package(Qt4 REQUIRED)

#-------------------------------------------------------------------------------
# INCLUDE directories for compiler
#-------------------------------------------------------------------------------

include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/src/common")
include_directories ("${PROJECT_SOURCE_DIR}/src/geom")
include_directories ("${PROJECT_SOURCE_DIR}/src/geom/skinning")
include_directories ("${PROJECT_SOURCE_DIR}/src/gui")
include_directories ("${PROJECT_SOURCE_DIR}/src/journaling")
include_directories ("${PROJECT_SOURCE_DIR}/src/mesh")
include_directories ("${PROJECT_SOURCE_DIR}/src/visu")
include_directories ("${PROJECT_SOURCE_DIR}/src/visu/prs")

#-------------------------------------------------------------------------------
# Add sources
#-------------------------------------------------------------------------------

ASITUS_ADD_SUBDIR0 (                 src)
ASITUS_ADD_SUBDIR1 (common           src/common)
ASITUS_ADD_SUBDIR1 (geom             src/geom)
ASITUS_ADD_SUBDIR2 (geom   skinning  src/geom/skinning)
ASITUS_ADD_SUBDIR1 (gui              src/gui)
ASITUS_ADD_SUBDIR1 (mesh             src/mesh)
ASITUS_ADD_SUBDIR1 (visu             src/visu)
ASITUS_ADD_SUBDIR2 (visu   prs       src/visu/prs)

message ("UI MOC files: ${ui_moc_files}")

#-------------------------------------------------------------------------------
# Output
#-------------------------------------------------------------------------------

# Setup output directories
set (EXECUTABLE_OUTPUT_PATH
  ${PROJECT_BINARY_DIR}/bin
  CACHE PATH "Output build directory"
)

# Do not bother the user with this output directory
mark_as_advanced (EXECUTABLE_OUTPUT_PATH)

#-------------------------------------------------------------------------------
# 3-rd parties
#-------------------------------------------------------------------------------

# OCCT
ASITUS_THIRDPARTY_PRODUCT("OCCT" "" "Standard.hxx" "TKernel.lib")

# VTK
ASITUS_THIRDPARTY_PRODUCT("VTK" "vtk-6.1" "vtkObject.h" "vtkCommonCore-6.1.lib")

# TBB
ASITUS_THIRDPARTY_PRODUCT("TBB" "" "tbb/tbb.h" "tbb.lib")

# Active Data
ASITUS_THIRDPARTY_PRODUCT("ACTIVE_DATA" "" "ActAPI.h" "ActiveData.lib")

# Quaoar
ASITUS_THIRDPARTY_PRODUCT("QUAOAR" "" "QrCore.h" "QrCore.lib")

message (STATUS "Include dirs: ${3RDPARTY_INCLUDE_DIRS}")
message (STATUS "Library dirs: ${3RDPARTY_LIBRARY_DIRS}")

#-------------------------------------------------------------------------------

string (REGEX REPLACE ";" " " 3RDPARTY_NOT_INCLUDED "${3RDPARTY_NOT_INCLUDED}")

# check all 3rdparty paths
if (3RDPARTY_NOT_INCLUDED)
  message (FATAL_ERROR "NOT FOUND: ${3RDPARTY_NOT_INCLUDED}")
endif()

#-------------------------------------------------------------------------------
# Executable
#-------------------------------------------------------------------------------

# Add executable with linker options after it
link_directories(${3RDPARTY_LIBRARY_DIRS})
#
add_executable (AnalysisSitus ${ui_moc_files} ${src_files})
#
target_link_libraries(AnalysisSitus

  QrCore.lib
  QrGeom3D.lib
  QrTools.lib

  ActiveDataAux.lib
  ActiveDataAPI.lib
  ActiveData.lib

  TKernel.lib
  TKMath.lib
  TKGeomBase.lib
  TKG2d.lib
  TKG3d.lib
  TKTopAlgo.lib
  TKBRep.lib
  TKV3d.lib
  TKPrim.lib
  TKLCAF.lib
  TKCAF.lib
  TKCDF.lib
  TKIVtk.lib
  TKService.lib
  TKXSBase.lib
  TKSTEP.lib
  TKShHealing.lib
  TKOffset.lib

  vtkCommonCore-6.1.lib
  vtkCommonDataModel-6.1.lib
  vtkCommonExecutionModel-6.1.lib
  vtkCommonMath-6.1.lib
  vtkCommonTransforms-6.1.lib
  vtkCommonMisc-6.1.lib
  vtkRenderingCore-6.1.lib
  vtkRenderingOpenGL-6.1.lib
  vtkRenderingAnnotation-6.1.lib
  vtkRenderingLabel-6.1.lib
  vtkRenderingFreeType-6.1.lib
  vtkRenderingFreeTypeOpenGL-6.1.lib
  vtkFiltersCore-6.1.lib
  vtkFiltersGeneral-6.1.lib
  vtkFiltersSources-6.1.lib
  vtkFiltersGeometry-6.1.lib
  vtkFiltersParallel-6.1.lib
  vtkFiltersExtraction-6.1.lib
  vtkFiltersModeling-6.1.lib
  vtkIOCore-6.1.lib
  vtkIOImage-6.1.lib
  vtkImagingCore-6.1.lib
  vtkInteractionStyle-6.1.lib
  vtkInteractionWidgets-6.1.lib
  vtkInfovisLayout-6.1.lib
  vtkGUISupportQt-6.1.lib
  vtkViewsCore-6.1.lib
  vtkViewsInfovis-6.1.lib

  QtCored4.lib
  QtGuid4.lib
)

# Set additional includes for 3-rd parties
include_directories ("${3RDPARTY_INCLUDE_DIRS}")

# Configure warnings level
if (MSVC)
  add_definitions (/W4)
elseif (CMAKE_COMPILER_IS_GNUCXX)
  add_definitions (-Wall -pedantic)
else()
  message ("Unknown compiler")
endif()
