project(asiAlgo)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiAlgo.h
)

#------------------------------------------------------------------------------
# Auxiliary
#------------------------------------------------------------------------------

set (aux_H_FILES
  auxiliary/asiAlgo_BuildContours.h
  auxiliary/asiAlgo_BVHFacets.h
  auxiliary/asiAlgo_BVHIterator.h
  auxiliary/asiAlgo_CheckToler.h
  auxiliary/asiAlgo_Classifier.h
  auxiliary/asiAlgo_ClassifyPointFace.h
  auxiliary/asiAlgo_CompleteEdgeLoop.h
  auxiliary/asiAlgo_DesignLaw.h
  auxiliary/asiAlgo_FairBCurve.h
  auxiliary/asiAlgo_FairingAijFunc.h
  auxiliary/asiAlgo_FairingBjFunc.h
  auxiliary/asiAlgo_FairingCoeffFunc.h
  auxiliary/asiAlgo_FuncUnivariate.h
  auxiliary/asiAlgo_HitFacet.h
  auxiliary/asiAlgo_IntersectCC.h
  auxiliary/asiAlgo_IntersectCS.h
  auxiliary/asiAlgo_IntersectionCurveSS.h
  auxiliary/asiAlgo_IntersectionPointCC.h
  auxiliary/asiAlgo_IntersectionPointCS.h
  auxiliary/asiAlgo_IntersectSS.h
  auxiliary/asiAlgo_Membership.h
  auxiliary/asiAlgo_PlateOnEdges.h
  auxiliary/asiAlgo_QuickHull2d.h
)
set (aux_CPP_FILES 
  auxiliary/asiAlgo_BuildContours.cpp
  auxiliary/asiAlgo_BVHFacets.cpp
  auxiliary/asiAlgo_BVHIterator.cpp
  auxiliary/asiAlgo_CheckToler.cpp
  auxiliary/asiAlgo_Classifier.cpp
  auxiliary/asiAlgo_ClassifyPointFace.cpp
  auxiliary/asiAlgo_CompleteEdgeLoop.cpp
  auxiliary/asiAlgo_DesignLaw.cpp
  auxiliary/asiAlgo_FairBCurve.cpp
  auxiliary/asiAlgo_FairingAijFunc.cpp
  auxiliary/asiAlgo_FairingBjFunc.cpp
  auxiliary/asiAlgo_FuncUnivariate.cpp
  auxiliary/asiAlgo_HitFacet.cpp
  auxiliary/asiAlgo_IntersectCC.cpp
  auxiliary/asiAlgo_IntersectCS.cpp
  auxiliary/asiAlgo_IntersectSS.cpp
  auxiliary/asiAlgo_PlateOnEdges.cpp
  auxiliary/asiAlgo_QuickHull2d.cpp
)

#------------------------------------------------------------------------------
# Editing
#------------------------------------------------------------------------------

set (editing_H_FILES
  editing/asiAlgo_DeleteEdges.h
  editing/asiAlgo_DetachFaces.h
  editing/asiAlgo_Euler.h
  editing/asiAlgo_EulerKEF.h
  editing/asiAlgo_EulerKEV.h
  editing/asiAlgo_EulerPoincare.h
  editing/asiAlgo_FindFree.h
  editing/asiAlgo_FindNonmanifold.h
  editing/asiAlgo_FindSameHosts.h
  editing/asiAlgo_History.h
  editing/asiAlgo_InvertFaces.h
  editing/asiAlgo_JoinEdges.h
  editing/asiAlgo_ModBase.h
  editing/asiAlgo_ModConstructEdge.h
  editing/asiAlgo_ModEdgeInfo.h
  editing/asiAlgo_Naming.h
  editing/asiAlgo_RehostFaces.h
  editing/asiAlgo_SuppressFaces.h
  editing/asiAlgo_TopoKill.h
)
set (editing_CPP_FILES 
  editing/asiAlgo_DeleteEdges.cpp
  editing/asiAlgo_DetachFaces.cpp
  editing/asiAlgo_Euler.cpp
  editing/asiAlgo_EulerKEF.cpp
  editing/asiAlgo_EulerKEV.cpp
  editing/asiAlgo_EulerPoincare.cpp
  editing/asiAlgo_FindFree.cpp
  editing/asiAlgo_FindNonmanifold.cpp
  editing/asiAlgo_FindSameHosts.cpp
  editing/asiAlgo_History.cpp
  editing/asiAlgo_InvertFaces.cpp
  editing/asiAlgo_JoinEdges.cpp
  editing/asiAlgo_ModBase.cpp
  editing/asiAlgo_ModConstructEdge.cpp
  editing/asiAlgo_Naming.cpp
  editing/asiAlgo_RehostFaces.cpp
  editing/asiAlgo_SuppressFaces.cpp
  editing/asiAlgo_TopoKill.cpp
)

#------------------------------------------------------------------------------
# Features
#------------------------------------------------------------------------------

set (features_H_FILES
  features/asiAlgo_AAG.h
  features/asiAlgo_AAGIterator.h
  features/asiAlgo_BorderTrihedron.h
  features/asiAlgo_DihedralAngle.h
  features/asiAlgo_FeatureAngle.h
  features/asiAlgo_FeatureAttr.h
  features/asiAlgo_FeatureAttrAdjacency.h
  features/asiAlgo_FeatureAttrAngle.h
  features/asiAlgo_TopoAttr.h
  features/asiAlgo_TopoAttrLocation.h
  features/asiAlgo_TopoAttrName.h
  features/asiAlgo_TopoAttrOrientation.h
  features/asiAlgo_TopoGraph.h
)
set (features_CPP_FILES 
  features/asiAlgo_AAG.cpp
  features/asiAlgo_AAGIterator.cpp
  features/asiAlgo_DihedralAngle.cpp
  features/asiAlgo_TopoGraph.cpp
)

#------------------------------------------------------------------------------
# Interoperability
#------------------------------------------------------------------------------

set (interop_H_FILES
  interop/asiAlgo_FileFormat.h
  interop/asiAlgo_IGES.h
  interop/asiAlgo_InteropVars.h
  interop/asiAlgo_OBJ.h
  interop/asiAlgo_PLY.h
  interop/asiAlgo_STEP.h
)
set (interop_CPP_FILES 
  interop/asiAlgo_FileFormat.cpp
  interop/asiAlgo_IGES.cpp
  interop/asiAlgo_OBJ.cpp
  interop/asiAlgo_PLY.cpp
  interop/asiAlgo_STEP.cpp
)

#------------------------------------------------------------------------------
# Mesh
#------------------------------------------------------------------------------

set (mesh_H_FILES
  mesh/asiAlgo_MeshConvert.h
  mesh/asiAlgo_MeshGen.h
  mesh/asiAlgo_MeshInfo.h
  mesh/asiAlgo_MeshLink.h
  mesh/asiAlgo_MeshMerge.h
  mesh/asiAlgo_MeshParams.h
)
set (mesh_CPP_FILES 
  mesh/asiAlgo_MeshConvert.cpp
  mesh/asiAlgo_MeshGen.cpp
  mesh/asiAlgo_MeshInfo.cpp
  mesh/asiAlgo_MeshMerge.cpp
  mesh/asiAlgo_MeshParams.cpp
)

#------------------------------------------------------------------------------
# Optimization
#------------------------------------------------------------------------------

set (opt_H_FILES
  opt/asiAlgo_ArmijoRule.h
  opt/asiAlgo_Function.h
  opt/asiAlgo_FunctionWithGradient.h
  opt/asiAlgo_GradientDescent.h
  opt/asiAlgo_PSO.h
)
set (opt_CPP_FILES 
  opt/asiAlgo_ArmijoRule.cpp
  opt/asiAlgo_Function.cpp
  opt/asiAlgo_FunctionWithGradient.cpp
  opt/asiAlgo_GradientDescent.cpp
  opt/asiAlgo_PSO.cpp
)

#------------------------------------------------------------------------------
# Points
#------------------------------------------------------------------------------

set (points_H_FILES
  points/asiAlgo_BaseCloud.h
  points/asiAlgo_Cloudify.h
  points/asiAlgo_CloudRegion.h
  points/asiAlgo_CommonCloud.h
  points/asiAlgo_FacePointCloud.h
  points/asiAlgo_PlaneOnPoints.h
  points/asiAlgo_PlateOnPoints.h
  points/asiAlgo_PointCloudUtils.h
  points/asiAlgo_PurifyCloud.h
  points/asiAlgo_ReorientNorms.h
)
set (points_CPP_FILES 
  points/asiAlgo_BaseCloud.cpp
  points/asiAlgo_Cloudify.cpp
  points/asiAlgo_CommonCloud.cpp
  points/asiAlgo_FacePointCloud.cpp
  points/asiAlgo_PlaneOnPoints.cpp
  points/asiAlgo_PlateOnPoints.cpp
  points/asiAlgo_PointCloudUtils.cpp
  points/asiAlgo_ReorientNorms.cpp
)

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

set (utils_H_FILES
  utils/asiAlgo_Logger.h
  utils/asiAlgo_MemChecker.h
  utils/asiAlgo_Timer.h
  utils/asiAlgo_Utils.h
  utils/asiAlgo_Variable.h
)
set (utils_CPP_FILES 
  utils/asiAlgo_Logger.cpp
  utils/asiAlgo_Utils.cpp
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

foreach (FILE ${aux_H_FILES})
  source_group ("Header Files\\Auxiliary" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${aux_CPP_FILES})
  source_group ("Source Files\\Auxiliary" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${editing_H_FILES})
  source_group ("Header Files\\Editing" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${editing_CPP_FILES})
  source_group ("Source Files\\Editing" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${features_H_FILES})
  source_group ("Header Files\\Features" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${features_CPP_FILES})
  source_group ("Source Files\\Features" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${interop_H_FILES})
  source_group ("Header Files\\Interoperability" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${interop_CPP_FILES})
  source_group ("Source Files\\Interoperability" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${mesh_H_FILES})
  source_group ("Header Files\\Mesh" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${mesh_CPP_FILES})
  source_group ("Source Files\\Mesh" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${opt_H_FILES})
  source_group ("Header Files\\Optimization" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${opt_CPP_FILES})
  source_group ("Source Files\\Optimization" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${points_H_FILES})
  source_group ("Header Files\\Points" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${points_CPP_FILES})
  source_group ("Source Files\\Points" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_H_FILES})
  source_group ("Header Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_CPP_FILES})
  source_group ("Source Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiAlgo_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/auxiliary;\
  ${CMAKE_CURRENT_SOURCE_DIR}/editing;\
  ${CMAKE_CURRENT_SOURCE_DIR}/features;\
  ${CMAKE_CURRENT_SOURCE_DIR}/interop;\
  ${CMAKE_CURRENT_SOURCE_DIR}/mesh;\
  ${CMAKE_CURRENT_SOURCE_DIR}/opt;\
  ${CMAKE_CURRENT_SOURCE_DIR}/points;\
  ${CMAKE_CURRENT_SOURCE_DIR}/utils;")
#
set (asiAlgo_include_dir ${asiAlgo_include_dir_loc} PARENT_SCOPE)

include_directories ( ${asiAlgo_include_dir_loc}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR}
                      ${3RDPARTY_tbb_INCLUDE_DIR} )

if (USE_MOBIUS)
  include_directories(${3RDPARTY_mobius_INCLUDE_DIR})
endif()

#------------------------------------------------------------------------------
# Create library
#------------------------------------------------------------------------------

add_library (asiAlgo SHARED
  ${H_FILES}          ${CPP_FILES}
  ${aux_H_FILES}      ${aux_CPP_FILES}
  ${editing_H_FILES}  ${editing_CPP_FILES}
  ${features_H_FILES} ${features_CPP_FILES}
  ${interop_H_FILES}  ${interop_CPP_FILES}
  ${mesh_H_FILES}     ${mesh_CPP_FILES}
  ${opt_H_FILES}      ${opt_CPP_FILES}
  ${points_H_FILES}   ${points_CPP_FILES}
  ${utils_H_FILES}    ${utils_CPP_FILES}
)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

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
    target_link_libraries (asiAlgo debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiAlgo optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiAlgo ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiAlgo debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiAlgo optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiAlgo ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiAlgo debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiAlgo optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiAlgo ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
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

  target_link_libraries (asiAlgo debug ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME_DEBUG})
  target_link_libraries (asiAlgo optimized ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME})
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiAlgo RUNTIME DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiAlgo
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         CONFIGURATIONS Release
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         CONFIGURATIONS Release)

install (TARGETS asiAlgo
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         CONFIGURATIONS Debug
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         CONFIGURATIONS Debug)

install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiAlgo.pdb DESTINATION framework/bind CONFIGURATIONS Debug)

install (FILES ${H_FILES}          DESTINATION framework/include)
install (FILES ${aux_H_FILES}      DESTINATION framework/include)
install (FILES ${editing_H_FILES}  DESTINATION framework/include)
install (FILES ${features_H_FILES} DESTINATION framework/include)
install (FILES ${interop_H_FILES}  DESTINATION framework/include)
install (FILES ${mesh_H_FILES}     DESTINATION framework/include)
install (FILES ${opt_H_FILES}      DESTINATION framework/include)
install (FILES ${points_H_FILES}   DESTINATION framework/include)
install (FILES ${utils_H_FILES}    DESTINATION framework/include)
