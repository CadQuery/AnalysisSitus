project(asiData)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiData.h
  asiData_NodeFlags.h
  asiData_ParameterFlags.h
  asiData_Partition.h
  asiData_RootNode.h
)
set (CPP_FILES
  asiData_Partition.cpp
  asiData_RootNode.cpp
)

#------------------------------------------------------------------------------
# Part
#------------------------------------------------------------------------------

set (part_H_FILES
  part/asiData_AAGAttr.h
  part/asiData_AAGParameter.h
  part/asiData_BoundaryEdgesNode.h
  part/asiData_BVHAttr.h
  part/asiData_BVHParameter.h
  part/asiData_CurvatureCombsNode.h
  part/asiData_CurveNode.h
  part/asiData_DeviationNode.h
  part/asiData_EdgeNode.h
  part/asiData_ElemMetadataNode.h
  part/asiData_FaceContourNode.h
  part/asiData_FaceNode.h
  part/asiData_FaceNormsNode.h
  part/asiData_MetadataNode.h
  part/asiData_NamingAttr.h
  part/asiData_NamingParameter.h
  part/asiData_PartNode.h
  part/asiData_SurfDeviationNode.h
  part/asiData_SurfNode.h
  part/asiData_TolerantRangeNode.h
  part/asiData_TolerantShapesNode.h
  part/asiData_VertexNode.h
)
set (part_CPP_FILES
  part/asiData_AAGAttr.cpp
  part/asiData_AAGParameter.cpp
  part/asiData_BoundaryEdgesNode.cpp
  part/asiData_BVHAttr.cpp
  part/asiData_BVHParameter.cpp
  part/asiData_CurvatureCombsNode.cpp
  part/asiData_CurveNode.cpp
  part/asiData_DeviationNode.cpp
  part/asiData_EdgeNode.cpp
  part/asiData_ElemMetadataNode.cpp
  part/asiData_FaceContourNode.cpp
  part/asiData_FaceNode.cpp
  part/asiData_FaceNormsNode.cpp
  part/asiData_MetadataNode.cpp
  part/asiData_NamingAttr.cpp
  part/asiData_NamingParameter.cpp
  part/asiData_PartNode.cpp
  part/asiData_SurfDeviationNode.cpp
  part/asiData_SurfNode.cpp
  part/asiData_TolerantRangeNode.cpp
  part/asiData_TolerantShapesNode.cpp
  part/asiData_VertexNode.cpp
)

#------------------------------------------------------------------------------
# Mesh
#------------------------------------------------------------------------------

set (mesh_H_FILES
  mesh/asiData_TessNode.h
  mesh/asiData_TessNormsNode.h
  mesh/asiData_TriangulationNode.h
)
set (mesh_CPP_FILES
  mesh/asiData_TessNode.cpp
  mesh/asiData_TessNormsNode.cpp
  mesh/asiData_TriangulationNode.cpp
)

#------------------------------------------------------------------------------
# IV
#------------------------------------------------------------------------------

set (iv_H_FILES
  iv/asiData_IVCurve2dNode.h
  iv/asiData_IVCurveNode.h
  iv/asiData_IVCurves2dNode.h
  iv/asiData_IVCurvesNode.h
  iv/asiData_IVNode.h
  iv/asiData_IVPoints2dNode.h
  iv/asiData_IVPointSet2dNode.h
  iv/asiData_IVPointSetNode.h
  iv/asiData_IVPointsNode.h
  iv/asiData_IVSurfaceNode.h
  iv/asiData_IVSurfacesNode.h
  iv/asiData_IVTessItemNode.h
  iv/asiData_IVTessNode.h
  iv/asiData_IVTextItemNode.h
  iv/asiData_IVTextNode.h
  iv/asiData_IVTopoItemNode.h
  iv/asiData_IVTopoNode.h
)
set (iv_CPP_FILES
  iv/asiData_IVCurve2dNode.cpp
  iv/asiData_IVCurveNode.cpp
  iv/asiData_IVCurves2dNode.cpp
  iv/asiData_IVCurvesNode.cpp
  iv/asiData_IVNode.cpp
  iv/asiData_IVPoints2dNode.cpp
  iv/asiData_IVPointSet2dNode.cpp
  iv/asiData_IVPointSetNode.cpp
  iv/asiData_IVPointsNode.cpp
  iv/asiData_IVSurfaceNode.cpp
  iv/asiData_IVSurfacesNode.cpp
  iv/asiData_IVTessItemNode.cpp
  iv/asiData_IVTessNode.cpp
  iv/asiData_IVTextItemNode.cpp
  iv/asiData_IVTextNode.cpp
  iv/asiData_IVTopoItemNode.cpp
  iv/asiData_IVTopoNode.cpp
)

#------------------------------------------------------------------------------
# Misc
#------------------------------------------------------------------------------

set (misc_H_FILES
  misc/asiData_ContourNode.h
  misc/asiData_DesignLawNode.h
  misc/asiData_FuncAttr.h
  misc/asiData_FuncParameter.h
)
set (misc_CPP_FILES
  misc/asiData_ContourNode.cpp
  misc/asiData_DesignLawNode.cpp
  misc/asiData_FuncAttr.cpp
  misc/asiData_FuncParameter.cpp
)

#------------------------------------------------------------------------------
# RE
#------------------------------------------------------------------------------

set (re_H_FILES
  re/asiData_ReCoedgeNode.h
  re/asiData_ReEdgeNode.h
  re/asiData_ReEdgesNode.h
  re/asiData_RePatchesNode.h
  re/asiData_RePatchNode.h
  re/asiData_ReTopoNode.h
  re/asiData_ReVertexNode.h
  re/asiData_ReVerticesNode.h
)
set (re_CPP_FILES
  re/asiData_ReCoedgeNode.cpp
  re/asiData_ReEdgeNode.cpp
  re/asiData_ReEdgesNode.cpp
  re/asiData_RePatchesNode.cpp
  re/asiData_RePatchNode.cpp
  re/asiData_ReTopoNode.cpp
  re/asiData_ReVertexNode.cpp
  re/asiData_ReVerticesNode.cpp
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
  vtkCommonCore-8.0
  vtkCommonDataModel-8.0
  vtkCommonExecutionModel-8.0
  vtkCommonMath-8.0
  vtkCommonTransforms-8.0
  vtkCommonMisc-8.0
  vtkFiltersCore-8.0
  vtkFiltersGeneral-8.0
  vtkFiltersSources-8.0
  vtkFiltersGeometry-8.0
  vtkFiltersParallel-8.0
  vtkFiltersExtraction-8.0
  vtkFiltersModeling-8.0
  vtkIOCore-8.0
  vtkIOImage-8.0
  vtkImagingCore-8.0
  vtkInteractionStyle-8.0
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

foreach (FILE ${part_H_FILES})
  source_group ("Header Files\\Part" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${part_CPP_FILES})
  source_group ("Source Files\\Part" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${mesh_H_FILES})
  source_group ("Header Files\\Mesh" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${mesh_CPP_FILES})
  source_group ("Source Files\\Mesh" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${iv_H_FILES})
  source_group ("Header Files\\IV" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${iv_CPP_FILES})
  source_group ("Source Files\\IV" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${misc_H_FILES})
  source_group ("Header Files\\Misc" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${misc_CPP_FILES})
  source_group ("Source Files\\Misc" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${re_H_FILES})
  source_group ("Header Files\\RE" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${re_CPP_FILES})
  source_group ("Source Files\\RE" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiData_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/part;\
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh;\
  ${CMAKE_CURRENT_SOURCE_DIR}/iv;\
  ${CMAKE_CURRENT_SOURCE_DIR}/re;\
  ${CMAKE_CURRENT_SOURCE_DIR}/misc;")
#
set (asiData_include_dir ${asiData_include_dir_loc} PARENT_SCOPE)

include_directories ( ${asiData_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR}
                      ${3RDPARTY_tbb_INCLUDE_DIR} )

if (USE_MOBIUS)
  include_directories(${3RDPARTY_mobius_INCLUDE_DIR})
endif()

#------------------------------------------------------------------------------
# Configure resources
#------------------------------------------------------------------------------

COPY_RESOURCES_TO_BINARY_DIRS(resources/Plugin)
COPY_RESOURCES_TO_BINARY_DIRS(resources/Resources)

#------------------------------------------------------------------------------
# Create library
#------------------------------------------------------------------------------

add_library (asiData SHARED
  ${H_FILES}      ${CPP_FILES}
  ${part_H_FILES} ${part_CPP_FILES}
  ${mesh_H_FILES} ${mesh_CPP_FILES}
  ${iv_H_FILES}   ${iv_CPP_FILES}
  ${re_H_FILES}   ${re_CPP_FILES}
  ${misc_H_FILES} ${misc_CPP_FILES}
)

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
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         CONFIGURATIONS RelWithDebInfo
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         CONFIGURATIONS RelWithDebInfo)

install (TARGETS asiData
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         CONFIGURATIONS Debug
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         CONFIGURATIONS Debug)

install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiData.pdb DESTINATION framework/bind CONFIGURATIONS Debug)

install (FILES ${H_FILES}      DESTINATION framework/include)
install (FILES ${part_H_FILES} DESTINATION framework/include)
install (FILES ${mesh_H_FILES} DESTINATION framework/include)
install (FILES ${iv_H_FILES}   DESTINATION framework/include)
install (FILES ${re_H_FILES}   DESTINATION framework/include)
install (FILES ${misc_H_FILES} DESTINATION framework/include)

install (FILES resources/Plugin    DESTINATION bin/resources)
install (FILES resources/Resources DESTINATION bin/resources)
