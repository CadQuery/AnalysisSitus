project(asiVisu)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiVisu.h
  asiVisu_Common.h
  asiVisu_MeshDisplayMode.h
  asiVisu_MeshPrimitive.h
  asiVisu_ShapeDisplayMode.h
  asiVisu_ShapePrimitive.h
)

#------------------------------------------------------------------------------
# Filters
#------------------------------------------------------------------------------

set (filters_H_FILES
  filters/asiVisu_MeshENScalarFilter.h
  filters/asiVisu_MeshEScalarFilter.h
  filters/asiVisu_MeshEVectorFilter.h
  filters/asiVisu_MeshNScalarFilter.h
  filters/asiVisu_PointsVectorFilter.h
  filters/asiVisu_ShapeDisplayModeFilter.h
  filters/asiVisu_ShrinkPolyFilter.h
  filters/asiVisu_TriangulationDisplayModeFilter.h
)
set (filters_CPP_FILES 
  filters/asiVisu_MeshENScalarFilter.cpp
  filters/asiVisu_MeshEScalarFilter.cpp
  filters/asiVisu_MeshEVectorFilter.cpp
  filters/asiVisu_MeshNScalarFilter.cpp
  filters/asiVisu_PointsVectorFilter.cpp
  filters/asiVisu_ShapeDisplayModeFilter.cpp
  filters/asiVisu_ShrinkPolyFilter.cpp
  filters/asiVisu_TriangulationDisplayModeFilter.cpp
)

#------------------------------------------------------------------------------
# Pipelines
#------------------------------------------------------------------------------

set (pipelines_H_FILES
  pipelines/asiVisu_BCurveHandlesPipeline.h
  pipelines/asiVisu_BCurveKnotsPipeline.h
  pipelines/asiVisu_BCurvePolesPipeline.h
  pipelines/asiVisu_BCurveRepersPipeline.h
  pipelines/asiVisu_BSurfAxesPipeline.h
  pipelines/asiVisu_BSurfKnotsPipeline.h
  pipelines/asiVisu_BSurfPolesPipeline.h
  pipelines/asiVisu_CurvatureCombsPipeline.h
  pipelines/asiVisu_CurvePipeline.h
  pipelines/asiVisu_EdgeCurvePipeline.h
  pipelines/asiVisu_EdgeDomainPipeline.h
  pipelines/asiVisu_FaceContourPipeline.h
  pipelines/asiVisu_FaceDomainPipeline.h
  pipelines/asiVisu_FaceSurfaceIsosPipeline.h
  pipelines/asiVisu_FaceSurfaceShadedPipeline.h
  pipelines/asiVisu_FuncLevelsPipeline2d.h
  pipelines/asiVisu_FuncLevelsPipeline3d.h
  pipelines/asiVisu_FuncUnivariatePipeline.h
  pipelines/asiVisu_IVSurfacePipeline.h
  pipelines/asiVisu_MeshContourPipeline.h
  pipelines/asiVisu_MeshENScalarPipeline.h
  pipelines/asiVisu_MeshEScalarPipeline.h
  pipelines/asiVisu_MeshEVectorPipeline.h
  pipelines/asiVisu_MeshNScalarPipeline.h
  pipelines/asiVisu_MeshPipeline.h
  pipelines/asiVisu_OctreePipeline.h
  pipelines/asiVisu_OctreeVectorsPipeline.h
  pipelines/asiVisu_PartEdgesPipeline.h
  pipelines/asiVisu_PartPipeline.h
  pipelines/asiVisu_PartPipelineBase.h
  pipelines/asiVisu_Pipeline.h
  pipelines/asiVisu_PointsPipeline.h
  pipelines/asiVisu_ReCoedgePipeline.h
  pipelines/asiVisu_RePatchPipeline.h
  pipelines/asiVisu_ShapePipeline.h
  pipelines/asiVisu_SurfaceDeviationPipeline.h
  pipelines/asiVisu_TriangulationLinksPipeline.h
  pipelines/asiVisu_TriangulationPipeline.h
  pipelines/asiVisu_TriangulationPipelineBase.h
  pipelines/asiVisu_VectorsPipeline.h
)
set (pipelines_CPP_FILES 
  pipelines/asiVisu_BCurveHandlesPipeline.cpp
  pipelines/asiVisu_BCurveKnotsPipeline.cpp
  pipelines/asiVisu_BCurvePolesPipeline.cpp
  pipelines/asiVisu_BCurveRepersPipeline.cpp
  pipelines/asiVisu_BSurfAxesPipeline.cpp
  pipelines/asiVisu_BSurfKnotsPipeline.cpp
  pipelines/asiVisu_BSurfPolesPipeline.cpp
  pipelines/asiVisu_CurvatureCombsPipeline.cpp
  pipelines/asiVisu_CurvePipeline.cpp
  pipelines/asiVisu_EdgeCurvePipeline.cpp
  pipelines/asiVisu_EdgeDomainPipeline.cpp
  pipelines/asiVisu_FaceContourPipeline.cpp
  pipelines/asiVisu_FaceDomainPipeline.cpp
  pipelines/asiVisu_FaceSurfaceIsosPipeline.cpp
  pipelines/asiVisu_FaceSurfaceShadedPipeline.cpp
  pipelines/asiVisu_FuncUnivariatePipeline.cpp
  pipelines/asiVisu_IVSurfacePipeline.cpp
  pipelines/asiVisu_MeshContourPipeline.cpp
  pipelines/asiVisu_MeshENScalarPipeline.cpp
  pipelines/asiVisu_MeshEScalarPipeline.cpp
  pipelines/asiVisu_MeshEVectorPipeline.cpp
  pipelines/asiVisu_MeshNScalarPipeline.cpp
  pipelines/asiVisu_MeshPipeline.cpp
  pipelines/asiVisu_OctreePipeline.cpp
  pipelines/asiVisu_OctreeVectorsPipeline.cpp
  pipelines/asiVisu_PartEdgesPipeline.cpp
  pipelines/asiVisu_PartPipeline.cpp
  pipelines/asiVisu_PartPipelineBase.cpp
  pipelines/asiVisu_Pipeline.cpp
  pipelines/asiVisu_PointsPipeline.cpp
  pipelines/asiVisu_ReCoedgePipeline.cpp
  pipelines/asiVisu_RePatchPipeline.cpp
  pipelines/asiVisu_ShapePipeline.cpp
  pipelines/asiVisu_SurfaceDeviationPipeline.cpp
  pipelines/asiVisu_TriangulationLinksPipeline.cpp
  pipelines/asiVisu_TriangulationPipeline.cpp
  pipelines/asiVisu_TriangulationPipelineBase.cpp
  pipelines/asiVisu_VectorsPipeline.cpp
)

#------------------------------------------------------------------------------
# Presentations
#------------------------------------------------------------------------------

set (presentations_H_FILES
  presentations/asiVisu_CalculusLawPrs.h
  presentations/asiVisu_ContourPrs.h
  presentations/asiVisu_CurvatureCombsPrs.h
  presentations/asiVisu_DefaultPrs.h
  presentations/asiVisu_DeviationPrs.h
  presentations/asiVisu_FaceDomainPrs.h
  presentations/asiVisu_GeomBoundaryEdgesPrs.h
  presentations/asiVisu_GeomCurvePrs.h
  presentations/asiVisu_GeomEdgePrs.h
  presentations/asiVisu_GeomFaceContourPrs.h
  presentations/asiVisu_GeomFaceNormsPrs.h
  presentations/asiVisu_GeomSurfPrs.h
  presentations/asiVisu_IVCurve2dPrs.h
  presentations/asiVisu_IVCurvePrs.h
  presentations/asiVisu_IVPointSet2dPrs.h
  presentations/asiVisu_IVPointSetPrs.h
  presentations/asiVisu_IVSurfacePrs.h
  presentations/asiVisu_IVTessItemPrs.h
  presentations/asiVisu_IVTextItemPrs.h
  presentations/asiVisu_IVTopoItemPrs.h
  presentations/asiVisu_OctreePrs.h
  presentations/asiVisu_PartPrs.h
  presentations/asiVisu_Prs.h
  presentations/asiVisu_PrsManager.h
  presentations/asiVisu_ReCoedgePrs.h
  presentations/asiVisu_ReEdgePrs.h
  presentations/asiVisu_RePatchPrs.h
  presentations/asiVisu_ReVertexPrs.h
  presentations/asiVisu_SurfDeviationPrs.h
  presentations/asiVisu_TessellationNormsPrs.h
  presentations/asiVisu_TessellationPrs.h
  presentations/asiVisu_ThicknessPrs.h
  presentations/asiVisu_TolerantRangePrs.h
  presentations/asiVisu_TriangulationPrs.h
)
set (presentations_CPP_FILES 
  presentations/asiVisu_CalculusLawPrs.cpp
  presentations/asiVisu_ContourPrs.cpp
  presentations/asiVisu_CurvatureCombsPrs.cpp
  presentations/asiVisu_DefaultPrs.cpp
  presentations/asiVisu_DeviationPrs.cpp
  presentations/asiVisu_FaceDomainPrs.cpp
  presentations/asiVisu_GeomBoundaryEdgesPrs.cpp
  presentations/asiVisu_GeomCurvePrs.cpp
  presentations/asiVisu_GeomEdgePrs.cpp
  presentations/asiVisu_GeomFaceContourPrs.cpp
  presentations/asiVisu_GeomFaceNormsPrs.cpp
  presentations/asiVisu_GeomSurfPrs.cpp
  presentations/asiVisu_IVCurve2dPrs.cpp
  presentations/asiVisu_IVCurvePrs.cpp
  presentations/asiVisu_IVPointSet2dPrs.cpp
  presentations/asiVisu_IVPointSetPrs.cpp
  presentations/asiVisu_IVSurfacePrs.cpp
  presentations/asiVisu_IVTessItemPrs.cpp
  presentations/asiVisu_IVTextItemPrs.cpp
  presentations/asiVisu_IVTopoItemPrs.cpp
  presentations/asiVisu_OctreePrs.cpp
  presentations/asiVisu_PartPrs.cpp
  presentations/asiVisu_Prs.cpp
  presentations/asiVisu_PrsManager.cpp
  presentations/asiVisu_ReCoedgePrs.cpp
  presentations/asiVisu_ReEdgePrs.cpp
  presentations/asiVisu_RePatchPrs.cpp
  presentations/asiVisu_ReVertexPrs.cpp
  presentations/asiVisu_SurfDeviationPrs.cpp
  presentations/asiVisu_TessellationNormsPrs.cpp
  presentations/asiVisu_TessellationPrs.cpp
  presentations/asiVisu_ThicknessPrs.cpp
  presentations/asiVisu_TolerantRangePrs.cpp
  presentations/asiVisu_TriangulationPrs.cpp
)

#------------------------------------------------------------------------------
# Data providers
#------------------------------------------------------------------------------

set (providers_H_FILES
  providers/asiVisu_ContourDataProvider.h
  providers/asiVisu_ContourPointsDataProvider.h
  providers/asiVisu_ContourPolesDataProvider.h
  providers/asiVisu_CurvatureCombsDataProvider.h
  providers/asiVisu_CurveDataProvider.h
  providers/asiVisu_DataProvider.h
  providers/asiVisu_DeviationDataProvider.h
  providers/asiVisu_DomainPointsDataProvider.h
  providers/asiVisu_EdgeDataProvider.h
  providers/asiVisu_FaceContourDataProvider.h
  providers/asiVisu_FaceDataProvider.h
  providers/asiVisu_FaceNormalsDataProvider.h
  providers/asiVisu_FuncDataProvider.h
  providers/asiVisu_FuncUnivariateDataProvider.h
  providers/asiVisu_IVCurve2dDataProvider.h
  providers/asiVisu_IVCurveDataProvider.h
  providers/asiVisu_IVPointSet2dDataProvider.h
  providers/asiVisu_IVPointSetDataProvider.h
  providers/asiVisu_IVSurfaceDataProvider.h
  providers/asiVisu_LawDataProvider.h
  providers/asiVisu_MeshDataProvider.h
  providers/asiVisu_MeshEScalarDataProvider.h
  providers/asiVisu_MeshNScalarDataProvider.h
  providers/asiVisu_OctreeDataProvider.h
  providers/asiVisu_PartDataProvider.h
  providers/asiVisu_PointsDataProvider.h
  providers/asiVisu_PointsFromListDataProvider.h
  providers/asiVisu_ReCoedgeDataProvider.h
  providers/asiVisu_ReEdgeCurveDataProvider.h
  providers/asiVisu_ReEdgePolylineDataProvider.h
  providers/asiVisu_ReEdgeShapeDataProvider.h
  providers/asiVisu_RePatchDataProvider.h
  providers/asiVisu_ReVertexDataProvider.h
  providers/asiVisu_ShapeDataProvider.h
  providers/asiVisu_SurfaceDataProvider.h
  providers/asiVisu_SurfaceDeviationDataProvider.h
  providers/asiVisu_TessNormalsDataProvider.h
  providers/asiVisu_ThicknessDataProvider.h
  providers/asiVisu_TriangulationDataProvider.h
  providers/asiVisu_VectorsDataProvider.h
)
set (providers_CPP_FILES 
  providers/asiVisu_ContourDataProvider.cpp
  providers/asiVisu_ContourPointsDataProvider.cpp
  providers/asiVisu_ContourPolesDataProvider.cpp
  providers/asiVisu_DataProvider.cpp
  providers/asiVisu_DeviationDataProvider.cpp
  providers/asiVisu_DomainPointsDataProvider.cpp
  providers/asiVisu_EdgeDataProvider.cpp
  providers/asiVisu_FaceContourDataProvider.cpp
  providers/asiVisu_FaceDataProvider.cpp
  providers/asiVisu_FaceNormalsDataProvider.cpp
  providers/asiVisu_IVCurve2dDataProvider.cpp
  providers/asiVisu_IVCurveDataProvider.cpp
  providers/asiVisu_IVPointSet2dDataProvider.cpp
  providers/asiVisu_IVPointSetDataProvider.cpp
  providers/asiVisu_IVSurfaceDataProvider.cpp
  providers/asiVisu_LawDataProvider.cpp
  providers/asiVisu_MeshDataProvider.cpp
  providers/asiVisu_OctreeDataProvider.cpp
  providers/asiVisu_PartDataProvider.cpp
  providers/asiVisu_PointsDataProvider.cpp
  providers/asiVisu_PointsFromListDataProvider.cpp
  providers/asiVisu_ReCoedgeDataProvider.cpp
  providers/asiVisu_ReEdgeCurveDataProvider.cpp
  providers/asiVisu_ReEdgePolylineDataProvider.cpp
  providers/asiVisu_ReEdgeShapeDataProvider.cpp
  providers/asiVisu_RePatchDataProvider.cpp
  providers/asiVisu_ReVertexDataProvider.cpp
  providers/asiVisu_ShapeDataProvider.cpp
  providers/asiVisu_SurfaceDeviationDataProvider.cpp
  providers/asiVisu_TessNormalsDataProvider.cpp
  providers/asiVisu_ThicknessDataProvider.cpp
  providers/asiVisu_TriangulationDataProvider.cpp
  providers/asiVisu_VectorsDataProvider.cpp
)

#------------------------------------------------------------------------------
# Selection
#------------------------------------------------------------------------------

set (selection_H_FILES
  selection/asiVisu_InteractorStylePick.h
  selection/asiVisu_InteractorStylePick2d.h
  selection/asiVisu_NodeInfo.h
  selection/asiVisu_PartNodeInfo.h
  selection/asiVisu_Selection.h
  selection/asiVisu_TriangulationNodeInfo.h
)
set (selection_CPP_FILES 
  selection/asiVisu_InteractorStylePick.cpp
  selection/asiVisu_InteractorStylePick2d.cpp
  selection/asiVisu_NodeInfo.cpp
  selection/asiVisu_PartNodeInfo.cpp
  selection/asiVisu_Selection.cpp
  selection/asiVisu_TriangulationNodeInfo.cpp
)

#------------------------------------------------------------------------------
# Sources
#------------------------------------------------------------------------------

set (sources_H_FILES
  sources/asiVisu_BCurveHandlesSource.h
  sources/asiVisu_BCurveKnotsSource.h
  sources/asiVisu_BCurvePolesSource.h
  sources/asiVisu_BCurveRepersSource.h
  sources/asiVisu_BSurfAxesSource.h
  sources/asiVisu_BSurfKnotsSource.h
  sources/asiVisu_BSurfPolesSource.h
  sources/asiVisu_CurvatureCombsSource.h
  sources/asiVisu_CurveSource.h
  sources/asiVisu_CurveSourceBase.h
  sources/asiVisu_FuncLevelsSource2d.h
  sources/asiVisu_FuncLevelsSource3d.h
  sources/asiVisu_FuncLevelsSourceBase.h
  sources/asiVisu_FuncUnivariateSource.h
  sources/asiVisu_MeshSource.h
  sources/asiVisu_OctreeSource.h
  sources/asiVisu_PCurveSource.h
  sources/asiVisu_PDomainSource.h
  sources/asiVisu_PointsSource.h
  sources/asiVisu_ReCoedgeSource.h
  sources/asiVisu_ShapeColorSource.h
  sources/asiVisu_ShapeColorSourceAAG.h
  sources/asiVisu_ShapeColorSourceMeta.h
  sources/asiVisu_ShapeData.h
  sources/asiVisu_ShapeRobustSource.h
  sources/asiVisu_ShapeRobustTessellator.h
  sources/asiVisu_SurfaceSource.h
  sources/asiVisu_TriangulationSource.h
)
set (sources_CPP_FILES 
  sources/asiVisu_BCurveHandlesSource.cpp
  sources/asiVisu_BCurveKnotsSource.cpp
  sources/asiVisu_BCurvePolesSource.cpp
  sources/asiVisu_BCurveRepersSource.cpp
  sources/asiVisu_BSurfAxesSource.cpp
  sources/asiVisu_BSurfKnotsSource.cpp
  sources/asiVisu_BSurfPolesSource.cpp
  sources/asiVisu_CurvatureCombsSource.cpp
  sources/asiVisu_CurveSource.cpp
  sources/asiVisu_CurveSourceBase.cpp
  sources/asiVisu_FuncUnivariateSource.cpp
  sources/asiVisu_MeshSource.cpp
  sources/asiVisu_OctreeSource.cpp
  sources/asiVisu_PCurveSource.cpp
  sources/asiVisu_PDomainSource.cpp
  sources/asiVisu_PointsSource.cpp
  sources/asiVisu_ReCoedgeSource.cpp
  sources/asiVisu_ShapeData.cpp
  sources/asiVisu_ShapeRobustSource.cpp
  sources/asiVisu_ShapeRobustTessellator.cpp
  sources/asiVisu_SurfaceSource.cpp
  sources/asiVisu_TriangulationSource.cpp
)

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

set (utils_H_FILES
  utils/asiVisu_GeomUtils.h
  utils/asiVisu_MeshResultUtils.h
  utils/asiVisu_MeshUtils.h
  utils/asiVisu_Utils.h
)
set (utils_CPP_FILES
  utils/asiVisu_GeomUtils.cpp
  utils/asiVisu_MeshResultUtils.cpp
  utils/asiVisu_MeshUtils.cpp
  utils/asiVisu_Utils.cpp
)

#------------------------------------------------------------------------------
# Widgets
#------------------------------------------------------------------------------

set (widgets_H_UI_FILES
)
set (widgets_H_FILES
  widgets/asiVisu_PlaneWidget.h
)
set (widgets_CPP_FILES
  widgets/asiVisu_PlaneWidget.cpp
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
  vtkCommonCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkCommonColor-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkCommonDataModel-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkCommonExecutionModel-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkCommonMath-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkCommonTransforms-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkCommonMisc-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersGeneral-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersSources-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersGeometry-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersParallel-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersExtraction-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkFiltersModeling-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkGUISupportQt-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkIOCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkIOImage-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkImagingCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkInteractionStyle-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkInteractionWidgets-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkRenderingAnnotation-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkRenderingCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
  vtkRenderingOpenGL2-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}
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

foreach (FILE ${filters_H_FILES})
  source_group ("Header Files\\Filters" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${filters_CPP_FILES})
  source_group ("Source Files\\Filters" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${pipelines_H_FILES})
  source_group ("Header Files\\Pipelines" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${pipelines_CPP_FILES})
  source_group ("Source Files\\Pipelines" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${presentations_H_FILES})
  source_group ("Header Files\\Presentations" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${presentations_CPP_FILES})
  source_group ("Source Files\\Presentations" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${providers_H_FILES})
  source_group ("Header Files\\Providers" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${providers_CPP_FILES})
  source_group ("Source Files\\Providers" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${selection_H_FILES})
  source_group ("Header Files\\Selection" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${selection_CPP_FILES})
  source_group ("Source Files\\Selection" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${sources_H_FILES})
  source_group ("Header Files\\Sources" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${sources_CPP_FILES})
  source_group ("Source Files\\Sources" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_H_FILES})
  source_group ("Header Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_CPP_FILES})
  source_group ("Source Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${widgets_H_FILES})
  source_group ("Header Files\\Widgets" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${widgets_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")

  message (STATUS "... Info: next header file ${FILE}")
  source_group ("Header Files\\Widgets" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${widgets_CPP_FILES})
  source_group ("Source Files\\Widgets" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiVisu_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/filters;\
  ${CMAKE_CURRENT_SOURCE_DIR}/pipelines;\
  ${CMAKE_CURRENT_SOURCE_DIR}/presentations;\
  ${CMAKE_CURRENT_SOURCE_DIR}/providers;\
  ${CMAKE_CURRENT_SOURCE_DIR}/selection;\
  ${CMAKE_CURRENT_SOURCE_DIR}/sources;\
  ${CMAKE_CURRENT_SOURCE_DIR}/utils;\
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets;")
#
set (asiVisu_include_dir ${asiVisu_include_dir_loc} PARENT_SCOPE)

include_directories ( SYSTEM
                      ${asiVisu_include_dir_loc}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
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

add_library (asiVisu SHARED
  ${H_FILES} ${CPP_FILES} ${ui_moc_files}
  ${filters_H_FILES}       ${filters_CPP_FILES}
  ${pipelines_H_FILES}     ${pipelines_CPP_FILES}
  ${presentations_H_FILES} ${presentations_CPP_FILES}
  ${providers_H_FILES}     ${providers_CPP_FILES}
  ${selection_H_FILES}     ${selection_CPP_FILES}
  ${sources_H_FILES}       ${sources_CPP_FILES}
  ${utils_H_FILES}         ${utils_CPP_FILES}
  ${widgets_H_FILES}       ${widgets_CPP_FILES} ${widgets_H_UI_FILES}
)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(asiVisu asiAlgo asiData)

qt5_use_modules(asiVisu Core Widgets)
#
if (NOT WIN32)
  qt5_use_modules(asiVisu X11Extras)
endif()

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
    target_link_libraries (asiVisu debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiVisu optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiVisu ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiVisu debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiVisu optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiVisu ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME_RELEASE "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set (LIB_FILENAME_DEBUG "${LIB_FILE}d${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME_RELEASE "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (LIB_FILENAME_DEBUG "lib${LIB_FILE}d${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME_DEBUG}")
    target_link_libraries (asiVisu debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME_DEBUG})
    target_link_libraries (asiVisu optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME_RELEASE})
  else()
    target_link_libraries (asiVisu ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME_RELEASE})
  endif()
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiVisu RUNTIME DESTINATION bin LIBRARY DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiVisu
         CONFIGURATIONS Release
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         LIBRARY DESTINATION framework/lib COMPONENT Library)

install (TARGETS asiVisu
         CONFIGURATIONS RelWithDebInfo
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         LIBRARY DESTINATION framework/libi COMPONENT Library)

install (TARGETS asiVisu
         CONFIGURATIONS Debug
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         LIBRARY DESTINATION framework/libd COMPONENT Library)

if (MSVC)
  install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiVisu.pdb DESTINATION framework/bind CONFIGURATIONS Debug)
endif()

install (FILES ${H_FILES}               DESTINATION framework/include)
install (FILES ${filters_H_FILES}       DESTINATION framework/include)
install (FILES ${pipelines_H_FILES}     DESTINATION framework/include)
install (FILES ${presentations_H_FILES} DESTINATION framework/include)
install (FILES ${providers_H_FILES}     DESTINATION framework/include)
install (FILES ${selection_H_FILES}     DESTINATION framework/include)
install (FILES ${sources_H_FILES}       DESTINATION framework/include)
install (FILES ${utils_H_FILES}         DESTINATION framework/include)
install (FILES ${widgets_H_FILES}       DESTINATION framework/include)
install (FILES ${widgets_H_UI_FILES}    DESTINATION framework/include)
