project(asiData)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiData.h
  asiData_AAGAttr.h
  asiData_AAGParameter.h
  asiData_BoundaryEdgesNode.h
  asiData_ContourNode.h
  asiData_CurveNode.h
  asiData_DesignLawNode.h
  asiData_EdgeNode.h
  asiData_FaceNode.h
  asiData_FaceNormsNode.h
  asiData_FuncAttr.h
  asiData_FuncParameter.h
  asiData_IVCurveNode.h
  asiData_IVCurvesNode.h
  asiData_IVNode.h
  asiData_IVPoints2dNode.h
  asiData_IVPointSet2dNode.h
  asiData_IVPointSetNode.h
  asiData_IVPointsNode.h
  asiData_IVSurfaceNode.h
  asiData_IVSurfacesNode.h
  asiData_IVTessItemNode.h
  asiData_IVTessNode.h
  asiData_IVTextItemNode.h
  asiData_IVTextNode.h
  asiData_IVTopoItemNode.h
  asiData_IVTopoNode.h
  asiData_Partition.h
  asiData_PartNode.h
  asiData_REAnalysisNode.h
  asiData_REContourNode.h
  asiData_REContoursNode.h
  asiData_REFunctionsNode.h
  asiData_REGaussMapNode.h
  asiData_RENode.h
  asiData_RENormalsNode.h
  asiData_REPointsNode.h
  asiData_REPrimitiveNode.h
  asiData_RESegmentNode.h
  asiData_RESegmentsNode.h
  asiData_RESurfaceNode.h
  asiData_RESurfacesNode.h
  asiData_RootNode.h
  asiData_SurfNode.h
  asiData_TessNode.h
  asiData_TriangulationNode.h
)

set (CPP_FILES 
  asiData_AAGAttr.cpp
  asiData_AAGParameter.cpp
  asiData_BoundaryEdgesNode.cpp
  asiData_ContourNode.cpp
  asiData_CurveNode.cpp
  asiData_DesignLawNode.cpp
  asiData_EdgeNode.cpp
  asiData_FaceNode.cpp
  asiData_FaceNormsNode.cpp
  asiData_FuncAttr.cpp
  asiData_FuncParameter.cpp
  asiData_IVCurveNode.cpp
  asiData_IVCurvesNode.cpp
  asiData_IVNode.cpp
  asiData_IVPoints2dNode.cpp
  asiData_IVPointSet2dNode.cpp
  asiData_IVPointSetNode.cpp
  asiData_IVPointsNode.cpp
  asiData_IVSurfaceNode.cpp
  asiData_IVSurfacesNode.cpp
  asiData_IVTessItemNode.cpp
  asiData_IVTessNode.cpp
  asiData_IVTextItemNode.cpp
  asiData_IVTextNode.cpp
  asiData_IVTopoItemNode.cpp
  asiData_IVTopoNode.cpp
  asiData_Partition.cpp
  asiData_PartNode.cpp
  asiData_REAnalysisNode.cpp
  asiData_REContourNode.cpp
  asiData_REContoursNode.cpp
  asiData_REFunctionsNode.cpp
  asiData_REGaussMapNode.cpp
  asiData_RENode.cpp
  asiData_RENormalsNode.cpp
  asiData_REPointsNode.cpp
  asiData_REPrimitiveNode.cpp
  asiData_RESegmentNode.cpp
  asiData_RESegmentsNode.cpp
  asiData_RESurfaceNode.cpp
  asiData_RESurfacesNode.cpp
  asiData_RootNode.cpp
  asiData_SurfNode.cpp
  asiData_TessNode.cpp
  asiData_TriangulationNode.cpp
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
set (TBB_LIB_FILES
  tbb
)
#------------------------------------------------------------------------------
set (VTK_LIB_FILES
  vtkCommonCore-7.1
  vtkCommonDataModel-7.1
  vtkCommonExecutionModel-7.1
  vtkCommonMath-7.1
  vtkCommonTransforms-7.1
  vtkCommonMisc-7.1
  vtkFiltersCore-7.1
  vtkFiltersGeneral-7.1
  vtkFiltersSources-7.1
  vtkFiltersGeometry-7.1
  vtkFiltersParallel-7.1
  vtkFiltersExtraction-7.1
  vtkFiltersModeling-7.1
  vtkIOCore-7.1
  vtkIOImage-7.1
  vtkImagingCore-7.1
  vtkInteractionStyle-7.1
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
set (asiData_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};")
#
set (asiData_include_dir ${asiData_include_dir_loc} PARENT_SCOPE)

include_directories ( ${asiData_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_mobius_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR}
                      ${3RDPARTY_tbb_INCLUDE_DIR} )

#------------------------------------------------------------------------------
# Create library
#------------------------------------------------------------------------------

add_library (asiData SHARED ${H_FILES} ${CPP_FILES})

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(asiData asiAlgo)

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
    target_link_libraries (asiData debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiData optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiData ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiData debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiData optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiData ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiData debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiData optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiData ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${TBB_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set (LIB_FILENAME_DEBUG "${LIB_FILE}_debug${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (LIB_FILENAME_DEBUG "lib${LIB_FILE}_debug${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  target_link_libraries (asiData debug ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME_DEBUG})
  target_link_libraries (asiData optimized ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME})
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiData RUNTIME DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiData
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         CONFIGURATIONS Release
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         CONFIGURATIONS Release)

install (TARGETS asiData
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         CONFIGURATIONS Debug
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         CONFIGURATIONS Debug)

install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiData.pdb DESTINATION framework/bind CONFIGURATIONS Debug)

install (FILES ${H_FILES} DESTINATION framework/include)
