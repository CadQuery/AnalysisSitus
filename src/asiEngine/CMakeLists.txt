project(asiEngine)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiEngine.h
  asiEngine_Base.h
  asiEngine_Curve.h
  asiEngine_Domain.h
  asiEngine_Editing.h
  asiEngine_IV.h
  asiEngine_Model.h
  asiEngine_Octree.h
  asiEngine_Part.h
  asiEngine_PatchJointAdaptor.h
  asiEngine_RE.h
  asiEngine_STEPReaderOutput.h
  asiEngine_STEPWriterInput.h
  asiEngine_Tessellation.h
  asiEngine_TolerantShapes.h
  asiEngine_Triangulation.h
)
set (CPP_FILES
  asiEngine_Base.cpp
  asiEngine_Curve.cpp
  asiEngine_Domain.cpp
  asiEngine_Editing.cpp
  asiEngine_IV.cpp
  asiEngine_Model.cpp
  asiEngine_Octree.cpp
  asiEngine_Part.cpp
  asiEngine_PatchJointAdaptor.cpp
  asiEngine_RE.cpp
  asiEngine_STEPReaderOutput.cpp
  asiEngine_STEPWriterInput.cpp
  asiEngine_Tessellation.cpp
  asiEngine_TolerantShapes.cpp
  asiEngine_Triangulation.cpp
)

#------------------------------------------------------------------------------
# Tree Functions
#------------------------------------------------------------------------------

set (func_H_FILES
  func/asiEngine_BuildEdgeFunc.h
  func/asiEngine_BuildOctreeFunc.h
  func/asiEngine_BuildPatchFunc.h
  func/asiEngine_SmoothenCornersFunc.h
  func/asiEngine_SmoothenPatchesFunc.h
)
set (func_CPP_FILES
  func/asiEngine_BuildEdgeFunc.cpp
  func/asiEngine_BuildOctreeFunc.cpp
  func/asiEngine_BuildPatchFunc.cpp
  func/asiEngine_SmoothenCornersFunc.cpp
  func/asiEngine_SmoothenPatchesFunc.cpp
)

#------------------------------------------------------------------------------
set (AD_LIB_FILES
  ActiveDataAPI
  ActiveData
  ActiveDataAux
)
#------------------------------------------------------------------------------
set (OCCT_LIB_FILES
  TKernel
  TKMath
  TKBRep
  TKOffset
  TKTopAlgo
  TKG2d
  TKG3d
  TKGeomBase
  TKGeomAlgo
  TKMesh
  TKShHealing
  TKFeat
  TKBool
  TKBO
  TKPrim
  TKBin
  TKBinL
  TKBinXCAF
  TKLCAF
  TKCDF
  TKCAF
  TKXCAF
  TKService
  TKXSBase
  TKSTEP
  TKIGES
  TKXDESTEP
  TKXDEIGES
)
#------------------------------------------------------------------------------
set (VTK_LIB_FILES
  vtkCommonCore-8.2
  vtkCommonDataModel-8.2
  vtkCommonExecutionModel-8.2
  vtkCommonMath-8.2
  vtkCommonTransforms-8.2
  vtkCommonMisc-8.2
  vtkFiltersCore-8.2
  vtkFiltersGeneral-8.2
  vtkFiltersSources-8.2
  vtkFiltersGeometry-8.2
  vtkFiltersParallel-8.2
  vtkFiltersExtraction-8.2
  vtkFiltersModeling-8.2
  vtkIOCore-8.2
  vtkIOImage-8.2
  vtkImagingCore-8.2
  vtkInteractionStyle-8.2
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

foreach (FILE ${func_H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files\\Tree Functions" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${func_CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files\\Tree Functions" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiEngine_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/func;\
")
#
set (asiEngine_include_dir ${asiEngine_include_dir_loc} PARENT_SCOPE)

include_directories ( SYSTEM
                      ${asiEngine_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
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

add_library (asiEngine SHARED ${H_FILES} ${CPP_FILES} ${func_H_FILES} ${func_CPP_FILES})

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(asiEngine asiAlgo asiData asiVisu)

if (3RDPARTY_tbb_LIBRARY_DIR_DEBUG)
  link_directories(${3RDPARTY_tbb_LIBRARY_DIR_DEBUG})
else()
  link_directories(${3RDPARTY_tbb_LIBRARY_DIR})
endif()

foreach (LIB_FILE ${OCCT_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_OCCT_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiEngine debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiEngine optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiEngine ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiEngine debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiEngine optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiEngine ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiEngine debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiEngine optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiEngine ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiEngine RUNTIME DESTINATION bin LIBRARY DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiEngine
         CONFIGURATIONS Release
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         LIBRARY DESTINATION framework/lib COMPONENT Library)

install (TARGETS asiEngine
         CONFIGURATIONS RelWithDebInfo
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         LIBRARY DESTINATION framework/libi COMPONENT Library)

install (TARGETS asiEngine
         CONFIGURATIONS Debug
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         LIBRARY DESTINATION framework/libd COMPONENT Library)

install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiEngine.pdb DESTINATION framework/bind CONFIGURATIONS Debug)

install (FILES ${H_FILES}      DESTINATION framework/include)
install (FILES ${func_H_FILES} DESTINATION framework/include)
