project(asiUI)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  asiUI.h
)

#------------------------------------------------------------------------------
# Controls
#------------------------------------------------------------------------------

set (controls_H_FILES
  controls/asiUI_IStatusBar.h
  controls/asiUI_StatusBarImpl.h
)
set (controls_H_UI_FILES
  controls/asiUI_BgColorDialog.h
  controls/asiUI_ColorButton.h
  controls/asiUI_Console.h
  controls/asiUI_ControlsFeature.h
  controls/asiUI_ControlsMesh.h
  controls/asiUI_ControlsMeshListener.h
  controls/asiUI_ControlsPart.h
  controls/asiUI_ControlsPartListener.h
  controls/asiUI_LineEdit.h
  controls/asiUI_ObjectBrowser.h
  controls/asiUI_StatusBar.h
  controls/asiUI_StyledTextEdit.h
)
set (controls_CPP_FILES 
  controls/asiUI_BgColorDialog.cpp
  controls/asiUI_ColorButton.cpp
  controls/asiUI_Console.cpp
  controls/asiUI_ControlsFeature.cpp
  controls/asiUI_ControlsMesh.cpp
  controls/asiUI_ControlsMeshListener.cpp
  controls/asiUI_ControlsPart.cpp
  controls/asiUI_ControlsPartListener.cpp
  controls/asiUI_LineEdit.cpp
  controls/asiUI_ObjectBrowser.cpp
  controls/asiUI_StatusBar.cpp
  controls/asiUI_StatusBarImpl.cpp
  controls/asiUI_StyledTextEdit.cpp
)

#------------------------------------------------------------------------------
# Dialogs
#------------------------------------------------------------------------------

set (dialogs_H_UI_FILES
  dialogs/asiUI_DialogCommands.h
  dialogs/asiUI_DialogCommandsDelegate.h
  dialogs/asiUI_DialogEuler.h
  dialogs/asiUI_DialogFindEdge.h
  dialogs/asiUI_DialogFindFace.h
  dialogs/asiUI_DialogGapFilling.h
  dialogs/asiUI_DialogGeomDef.h
  dialogs/asiUI_DialogOCAFDump.h
  dialogs/asiUI_DialogPipelines.h
  dialogs/asiUI_DialogRefineTessellation.h
  dialogs/asiUI_DialogSewing.h
  dialogs/asiUI_DialogSmoothAngleTol.h
  dialogs/asiUI_DialogSTEP.h
  dialogs/asiUI_DialogSTEPDelegate.h
  dialogs/asiUI_DialogTopoGraphDepth.h
)
set (dialogs_CPP_FILES 
  dialogs/asiUI_DialogCommands.cpp
  dialogs/asiUI_DialogCommandsDelegate.cpp
  dialogs/asiUI_DialogEuler.cpp
  dialogs/asiUI_DialogFindEdge.cpp
  dialogs/asiUI_DialogFindFace.cpp
  dialogs/asiUI_DialogGapFilling.cpp
  dialogs/asiUI_DialogGeomDef.cpp
  dialogs/asiUI_DialogOCAFDump.cpp
  dialogs/asiUI_DialogPipelines.cpp
  dialogs/asiUI_DialogRefineTessellation.cpp
  dialogs/asiUI_DialogSewing.cpp
  dialogs/asiUI_DialogSmoothAngleTol.cpp
  dialogs/asiUI_DialogSTEP.cpp
  dialogs/asiUI_DialogSTEPDelegate.cpp
  dialogs/asiUI_DialogTopoGraphDepth.cpp
)

#------------------------------------------------------------------------------
# Graphs
#------------------------------------------------------------------------------

set (graphs_H_FILES
  graphs/asiUI_AAGAdaptor.h
  graphs/asiUI_HistoryGraphAdaptor.h
  graphs/asiUI_TopoGraphAdaptor.h
)
set (graphs_H_UI_FILES
  graphs/asiUI_HistoryGraph.h
  graphs/asiUI_HistoryGraphItem.h
  graphs/asiUI_PartGraph.h
  graphs/asiUI_PartGraphItem.h
)
set (graphs_CPP_FILES 
  graphs/asiUI_AAGAdaptor.cpp
  graphs/asiUI_HistoryGraph.cpp
  graphs/asiUI_HistoryGraphAdaptor.cpp
  graphs/asiUI_HistoryGraphItem.cpp
  graphs/asiUI_PartGraph.cpp
  graphs/asiUI_PartGraphItem.cpp
  graphs/asiUI_TopoGraphAdaptor.cpp
)

#------------------------------------------------------------------------------
# Plots
#------------------------------------------------------------------------------

set (plots_H_UI_FILES
  plots/asiUI_Plot2d.h
)
set (plots_CPP_FILES 
  plots/asiUI_Plot2d.cpp
)

#------------------------------------------------------------------------------
# Interaction
#------------------------------------------------------------------------------

set (interaction_H_FILES
  interaction/asiUI_PickEdgeCallback.h
  interaction/asiUI_PickFacetCallback.h
  interaction/asiUI_RotationCallback.h
  interaction/asiUI_ViewerCallback.h
)
set (interaction_H_UI_FILES
  interaction/asiUI_AddCurveHandleCallback.h
  interaction/asiUI_BaseCurveCallback.h
  interaction/asiUI_MoveCurveHandleCallback.h
  interaction/asiUI_PartCallback.h
  interaction/asiUI_PDomainCallback.h
  interaction/asiUI_PickCallback.h
  interaction/asiUI_PickContourCallback.h
  interaction/asiUI_PickCurveCallback.h
)
set (interaction_CPP_FILES 
  interaction/asiUI_AddCurveHandleCallback.cpp
  interaction/asiUI_BaseCurveCallback.cpp
  interaction/asiUI_MoveCurveHandleCallback.cpp
  interaction/asiUI_PartCallback.cpp
  interaction/asiUI_PDomainCallback.cpp
  interaction/asiUI_PickCallback.cpp
  interaction/asiUI_PickContourCallback.cpp
  interaction/asiUI_PickCurveCallback.cpp
  interaction/asiUI_PickEdgeCallback.cpp
  interaction/asiUI_PickFacetCallback.cpp
  interaction/asiUI_RotationCallback.cpp
  interaction/asiUI_ViewerCallback.cpp
)

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

set (utils_H_FILES
  utils/asiUI_Common.h
  utils/asiUI_CommonFacilities.h
  utils/asiUI_DataAdaptor.h
  utils/asiUI_IV.h
  utils/asiUI_Logger.h
  utils/asiUI_OverrideCursor.h
  utils/asiUI_PrimDataAdaptor.h
)
set (utils_H_UI_FILES
  utils/asiUI_OverrideCursor.h
  utils/asiUI_PrimDataView.h
  utils/asiUI_ProgressListener.h
  utils/asiUI_ProgressNotifier.h
)
set (utils_CPP_FILES 
  utils/asiUI_Common.cpp
  utils/asiUI_DataAdaptor.cpp
  utils/asiUI_IV.cpp
  utils/asiUI_Logger.cpp
  utils/asiUI_OverrideCursor.cpp
  utils/asiUI_PrimDataAdaptor.cpp
  utils/asiUI_PrimDataView.cpp
  utils/asiUI_ProgressListener.cpp
  utils/asiUI_ProgressNotifier.cpp
)

#------------------------------------------------------------------------------
# Viewers
#------------------------------------------------------------------------------

set (viewers_H_UI_FILES
  viewers/asiUI_Viewer.h
  viewers/asiUI_Viewer3dListener.h
  viewers/asiUI_ViewerCurve2d.h
  viewers/asiUI_ViewerDomain.h
  viewers/asiUI_ViewerDomainListener.h
  viewers/asiUI_ViewerFuncUnivariate.h
  viewers/asiUI_ViewerHost.h
  viewers/asiUI_ViewerHostListener.h
  viewers/asiUI_ViewerPart.h
  viewers/asiUI_ViewerPartListener.h
  viewers/asiUI_VtkWindow.h
)
set (viewers_CPP_FILES 
  viewers/asiUI_Viewer.cpp
  viewers/asiUI_Viewer3dListener.cpp
  viewers/asiUI_ViewerCurve2d.cpp
  viewers/asiUI_ViewerDomain.cpp
  viewers/asiUI_ViewerDomainListener.cpp
  viewers/asiUI_ViewerFuncUnivariate.cpp
  viewers/asiUI_ViewerHost.cpp
  viewers/asiUI_ViewerHostListener.cpp
  viewers/asiUI_ViewerPart.cpp
  viewers/asiUI_ViewerPartListener.cpp
  viewers/asiUI_VtkWindow.cpp
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
  vtkChartsCore-8.0
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
  vtkGUISupportQt-8.0
  vtkInfovisLayout-8.0
  vtkIOCore-8.0
  vtkIOImage-8.0
  vtkImagingCore-8.0
  vtkInteractionStyle-8.0
  vtkInteractionWidgets-8.0
  vtkRenderingAnnotation-8.0
  vtkRenderingContext2D-8.0
  vtkRenderingCore-8.0
  vtkViewsContext2D-8.0
  vtkViewsInfovis-8.0
)

#------------------------------------------------------------------------------
# Add sources
#------------------------------------------------------------------------------

foreach (FILE ${H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${controls_H_FILES})
  source_group ("Header Files\\Controls" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${controls_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")

  message (STATUS "... Info: next header file ${FILE}")
  source_group ("Header Files\\Controls" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${controls_CPP_FILES})
  source_group ("Source Files\\Controls" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${dialogs_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Dialogs" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${dialogs_CPP_FILES})
  source_group ("Source Files\\Dialogs" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${graphs_H_FILES})
  source_group ("Header Files\\Graphs" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${graphs_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Graphs" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${graphs_CPP_FILES})
  source_group ("Source Files\\Graphs" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${plots_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Plots" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${plots_CPP_FILES})
  source_group ("Source Files\\Plots" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${interaction_H_FILES})
  source_group ("Header Files\\Interaction" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${interaction_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Interaction" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${interaction_CPP_FILES})
  source_group ("Source Files\\Interaction" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_H_FILES})
  source_group ("Header Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${utils_CPP_FILES})
  source_group ("Source Files\\Utilities" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${viewers_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Viewers" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${viewers_CPP_FILES})
  source_group ("Source Files\\Viewers" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (asiUI_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/controls;\
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs;\
  ${CMAKE_CURRENT_SOURCE_DIR}/graphs;\
  ${CMAKE_CURRENT_SOURCE_DIR}/plots;\
  ${CMAKE_CURRENT_SOURCE_DIR}/interaction;\
  ${CMAKE_CURRENT_SOURCE_DIR}/utils;\
  ${CMAKE_CURRENT_SOURCE_DIR}/viewers;")
#
set (asiUI_include_dir ${asiUI_include_dir_loc} PARENT_SCOPE)

include_directories ( ${asiUI_include_dir_loc}
                      ${asiTcl_include_dir}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
                      ${asiEngine_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_tcl_INCLUDE_DIR}
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

add_library (asiUI SHARED
  ${H_FILES} ${CPP_FILES} ${ui_moc_files}
  ${controls_H_FILES}     ${controls_H_UI_FILES}    ${controls_CPP_FILES}
  ${dialogs_H_FILES}      ${dialogs_H_UI_FILES}     ${dialogs_CPP_FILES}
  ${graphs_H_FILES}       ${graphs_H_UI_FILES}      ${graphs_CPP_FILES}
  ${plots_H_FILES}        ${plots_H_UI_FILES}       ${plots_CPP_FILES}
  ${interaction_H_FILES}  ${interaction_H_UI_FILES} ${interaction_CPP_FILES}
  ${utils_H_FILES}        ${utils_H_UI_FILES}       ${utils_CPP_FILES}
  ${viewers_H_FILES}      ${viewers_H_UI_FILES}     ${viewers_CPP_FILES}
)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

target_link_libraries(asiUI asiTcl asiAlgo asiData asiVisu asiEngine)

qt5_use_modules(asiUI Core Widgets)

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
    target_link_libraries (asiUI debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiUI optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiUI ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiUI debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiUI optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiUI ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiUI debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiUI optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiUI ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
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

  target_link_libraries (asiUI debug ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME_DEBUG})
  target_link_libraries (asiUI optimized ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME})
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiUI RUNTIME DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiUI
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         CONFIGURATIONS Release
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         CONFIGURATIONS Release)

install (TARGETS asiUI
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         CONFIGURATIONS Debug
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         CONFIGURATIONS Debug)

install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiUI.pdb DESTINATION framework/bind CONFIGURATIONS Debug)

install (FILES ${H_FILES}                DESTINATION framework/include)
install (FILES ${controls_H_FILES}       DESTINATION framework/include)
install (FILES ${controls_H_UI_FILES}    DESTINATION framework/include)
install (FILES ${dialogs_H_FILES}        DESTINATION framework/include)
install (FILES ${dialogs_H_UI_FILES}     DESTINATION framework/include)
install (FILES ${graphs_H_FILES}         DESTINATION framework/include)
install (FILES ${graphs_H_UI_FILES}      DESTINATION framework/include)
install (FILES ${plots_H_UI_FILES}       DESTINATION framework/include)
install (FILES ${interaction_H_FILES}    DESTINATION framework/include)
install (FILES ${interaction_H_UI_FILES} DESTINATION framework/include)
install (FILES ${utils_H_FILES}          DESTINATION framework/include)
install (FILES ${utils_H_UI_FILES}       DESTINATION framework/include)
install (FILES ${viewers_H_FILES}        DESTINATION framework/include)
install (FILES ${viewers_H_UI_FILES}     DESTINATION framework/include)
