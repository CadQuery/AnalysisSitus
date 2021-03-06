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
  controls/asiUI_IParamEditor.h
  controls/asiUI_IStatusBar.h
  controls/asiUI_ItemRoles.h
  controls/asiUI_HeaderViewAPI.h
  controls/asiUI_ParamEditorImpl.h
  controls/asiUI_ParameterEditorItem.h
  controls/asiUI_StatusBarImpl.h
  controls/asiUI_WidgetFactory.h
)
set (controls_H_UI_FILES
  controls/asiUI_BgColorDialog.h
  controls/asiUI_ColorButton.h
  controls/asiUI_Console.h
  controls/asiUI_ControlsAnalysis.h
  controls/asiUI_ControlsAnalysisListener.h
  controls/asiUI_ControlsMesh.h
  controls/asiUI_ControlsMeshListener.h
  controls/asiUI_ControlsModeling.h
  controls/asiUI_ControlsModelingListener.h
  controls/asiUI_ControlsPart.h
  controls/asiUI_ControlsPartListener.h
  controls/asiUI_EvalButton.h
  controls/asiUI_LineEdit.h
  controls/asiUI_ObjectBrowser.h
  controls/asiUI_ObjectBrowserListener.h
  controls/asiUI_ParameterEditor.h
  controls/asiUI_ParameterEditorDelegate.h
  controls/asiUI_ParameterEditorListener.h
  controls/asiUI_ParameterEditorListenerDefault.h
  controls/asiUI_ParameterEditorView.h
  controls/asiUI_StatusBar.h
  controls/asiUI_StyledTextEdit.h
)
set (controls_CPP_FILES 
  controls/asiUI_BgColorDialog.cpp
  controls/asiUI_ColorButton.cpp
  controls/asiUI_Console.cpp
  controls/asiUI_ControlsAnalysis.cpp
  controls/asiUI_ControlsAnalysisListener.cpp
  controls/asiUI_ControlsMesh.cpp
  controls/asiUI_ControlsMeshListener.cpp
  controls/asiUI_ControlsModeling.cpp
  controls/asiUI_ControlsModelingListener.cpp
  controls/asiUI_ControlsPart.cpp
  controls/asiUI_ControlsPartListener.cpp
  controls/asiUI_EvalButton.cpp
  controls/asiUI_LineEdit.cpp
  controls/asiUI_ObjectBrowser.cpp
  controls/asiUI_ObjectBrowserListener.cpp
  controls/asiUI_ParamEditorImpl.cpp
  controls/asiUI_ParameterEditor.cpp
  controls/asiUI_ParameterEditorDelegate.cpp
  controls/asiUI_ParameterEditorListener.cpp
  controls/asiUI_ParameterEditorListenerDefault.cpp
  controls/asiUI_ParameterEditorItem.cpp
  controls/asiUI_ParameterEditorView.cpp
  controls/asiUI_StatusBar.cpp
  controls/asiUI_StatusBarImpl.cpp
  controls/asiUI_StyledTextEdit.cpp
  controls/asiUI_WidgetFactory.cpp
)

#------------------------------------------------------------------------------
# Salome classes
#------------------------------------------------------------------------------

set (salome_H_FILES
  salome/QDS.h
  salome/QDS_Validator.h
  salome/Qtx.h
  salome/SUIT_TreeSync.h
)
set (salome_H_UI_FILES
  salome/QDS_CheckBox.h
  salome/QDS_ComboBox.h
  salome/QDS_Datum.h
  salome/QDS_LineEdit.h
  salome/QDS_RadioBox.h
  salome/QDS_SpinBox.h
  salome/QDS_SpinBoxDbl.h
  salome/QDS_TextEdit.h
  salome/QtxColorButton.h
  salome/QtxComboBox.h
  salome/QtxDoubleSpinBox.h
  salome/QtxIntSpinBox.h
  salome/SUIT_PopupClient.h
)
set (salome_CPP_FILES 
  salome/QDS.cxx
  salome/QDS_CheckBox.cxx
  salome/QDS_ComboBox.cxx
  salome/QDS_Datum.cxx
  salome/QDS_LineEdit.cxx
  salome/QDS_RadioBox.cxx
  salome/QDS_SpinBox.cxx
  salome/QDS_SpinBoxDbl.cxx
  salome/QDS_TextEdit.cxx
  salome/QDS_Validator.cxx
  salome/Qtx.cxx
  salome/QtxColorButton.cxx
  salome/QtxComboBox.cxx
  salome/QtxDoubleSpinBox.cxx
  salome/QtxIntSpinBox.cxx
  salome/SUIT_PopupClient.cxx
)

#------------------------------------------------------------------------------
# Datums
#------------------------------------------------------------------------------

set (datum_H_FILES
  datum/asiUI_DatumFormat.h
  datum/asiUI_DatumViewApi.h
  datum/asiUI_DatumViewItem.h
)
set (datum_H_UI_FILES
  datum/asiUI_Datum.h
  datum/asiUI_DatumCheckBox.h
  datum/asiUI_DatumColorEdit.h
  datum/asiUI_DatumColorItem.h
  datum/asiUI_DatumComboBox.h
  datum/asiUI_DatumCustomSelector.h
  datum/asiUI_DatumDateEdit.h
  datum/asiUI_DatumDelegate.h
  datum/asiUI_DatumItemEditor.h
  datum/asiUI_DatumLabel.h
  datum/asiUI_DatumLineEdit.h
  datum/asiUI_DatumPathEdit.h
  datum/asiUI_DatumPathItem.h
  datum/asiUI_DatumRadioBox.h
  datum/asiUI_DatumSelector.h
  datum/asiUI_DatumSpinBox.h
  datum/asiUI_DatumSpinBoxDbl.h
  datum/asiUI_DatumViewBase.h
  datum/asiUI_DatumViewDelegate.h
  datum/asiUI_DatumViewModel.h
)
set (datum_CPP_FILES 
  datum/asiUI_Datum.cpp
  datum/asiUI_DatumCheckBox.cpp
  datum/asiUI_DatumColorEdit.cpp
  datum/asiUI_DatumColorItem.cpp
  datum/asiUI_DatumComboBox.cpp
  datum/asiUI_DatumCustomSelector.cpp
  datum/asiUI_DatumDateEdit.cpp
  datum/asiUI_DatumDelegate.cpp
  datum/asiUI_DatumFormat.cpp
  datum/asiUI_DatumItemEditor.cpp
  datum/asiUI_DatumLabel.cpp
  datum/asiUI_DatumLineEdit.cpp
  datum/asiUI_DatumPathEdit.cpp
  datum/asiUI_DatumPathItem.cpp
  datum/asiUI_DatumRadioBox.cpp
  datum/asiUI_DatumSelector.cpp
  datum/asiUI_DatumSpinBox.cpp
  datum/asiUI_DatumSpinBoxDbl.cpp
  datum/asiUI_DatumViewBase.cpp
  datum/asiUI_DatumViewItem.cpp
  datum/asiUI_DatumViewDelegate.cpp
  datum/asiUI_DatumViewModel.cpp
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
  graphs/asiUI_DependencyGraph.h
  graphs/asiUI_DependencyGraphItem.h
  graphs/asiUI_HistoryGraph.h
  graphs/asiUI_HistoryGraphItem.h
  graphs/asiUI_PartGraph.h
  graphs/asiUI_PartGraphItem.h
)
set (graphs_CPP_FILES 
  graphs/asiUI_AAGAdaptor.cpp
  graphs/asiUI_DependencyGraph.cpp
  graphs/asiUI_DependencyGraphItem.cpp
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
  utils/asiUI_BatchFacilities.h
  utils/asiUI_BatchNotifier.h
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
  utils/asiUI_UnitManager.h
)
set (utils_CPP_FILES
  utils/asiUI_BatchFacilities.cpp
  utils/asiUI_BatchNotifier.cpp
  utils/asiUI_Common.cpp
  utils/asiUI_CommonFacilities.cpp
  utils/asiUI_DataAdaptor.cpp
  utils/asiUI_IV.cpp
  utils/asiUI_Logger.cpp
  utils/asiUI_OverrideCursor.cpp
  utils/asiUI_PrimDataAdaptor.cpp
  utils/asiUI_PrimDataView.cpp
  utils/asiUI_ProgressListener.cpp
  utils/asiUI_ProgressNotifier.cpp
  utils/asiUI_UnitManager.cpp
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
set (VTK_LIB_FILES
  vtkChartsCore-8.2
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
  vtkGUISupportQt-8.2
  vtkInfovisLayout-8.2
  vtkIOCore-8.2
  vtkIOImage-8.2
  vtkImagingCore-8.2
  vtkInteractionStyle-8.2
  vtkInteractionWidgets-8.2
  vtkRenderingAnnotation-8.2
  vtkRenderingContext2D-8.2
  vtkRenderingCore-8.2
  vtkViewsCore-8.2
  vtkViewsContext2D-8.2
  vtkViewsInfovis-8.2
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

foreach (FILE ${salome_H_FILES})
  source_group ("Header Files\\Salome" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${salome_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Salome" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${salome_CPP_FILES})
  source_group ("Source Files\\Salome" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${datum_H_FILES})
  source_group ("Header Files\\Datum" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${datum_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\Datum" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${datum_CPP_FILES})
  source_group ("Source Files\\Datum" FILES "${FILE}")
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
  ${CMAKE_CURRENT_SOURCE_DIR}/datum;\
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs;\
  ${CMAKE_CURRENT_SOURCE_DIR}/graphs;\
  ${CMAKE_CURRENT_SOURCE_DIR}/plots;\
  ${CMAKE_CURRENT_SOURCE_DIR}/salome;\
  ${CMAKE_CURRENT_SOURCE_DIR}/interaction;\
  ${CMAKE_CURRENT_SOURCE_DIR}/utils;\
  ${CMAKE_CURRENT_SOURCE_DIR}/viewers;")
#
set (asiUI_include_dir ${asiUI_include_dir_loc} PARENT_SCOPE)

include_directories ( SYSTEM
                      ${asiUI_include_dir_loc}
                      ${asiTcl_include_dir}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
                      ${asiEngine_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_tcl_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR} )

if (USE_MOBIUS)
  include_directories(SYSTEM ${3RDPARTY_mobius_INCLUDE_DIR})
endif()

#------------------------------------------------------------------------------
# Create library
#------------------------------------------------------------------------------

add_library (asiUI SHARED
  ${H_FILES} ${CPP_FILES} ${ui_moc_files}
  ${controls_H_FILES}     ${controls_H_UI_FILES}    ${controls_CPP_FILES}
  ${salome_H_FILES}       ${salome_H_UI_FILES}      ${salome_CPP_FILES}
  ${datum_H_FILES}        ${datum_H_UI_FILES}       ${datum_CPP_FILES}
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

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiUI RUNTIME DESTINATION bin LIBRARY DESTINATION bin COMPONENT Runtime)

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a framework
#------------------------------------------------------------------------------

install (TARGETS asiUI
         CONFIGURATIONS Release
         RUNTIME DESTINATION framework/bin COMPONENT Runtime
         ARCHIVE DESTINATION framework/lib COMPONENT Library
         LIBRARY DESTINATION framework/lib COMPONENT Library)

install (TARGETS asiUI
         CONFIGURATIONS RelWithDebInfo
         RUNTIME DESTINATION framework/bini COMPONENT Runtime
         ARCHIVE DESTINATION framework/libi COMPONENT Library
         LIBRARY DESTINATION framework/libi COMPONENT Library)

install (TARGETS asiUI
         CONFIGURATIONS Debug
         RUNTIME DESTINATION framework/bind COMPONENT Runtime
         ARCHIVE DESTINATION framework/libd COMPONENT Library
         LIBRARY DESTINATION framework/libd COMPONENT Library)

if (MSVC)
  install (FILES ${PROJECT_BINARY_DIR}/../../${PLATFORM}${COMPILER_BITNESS}/${COMPILER}/bind/asiUI.pdb DESTINATION framework/bind CONFIGURATIONS Debug)
endif()

install (FILES ${H_FILES}                DESTINATION framework/include)
install (FILES ${controls_H_FILES}       DESTINATION framework/include)
install (FILES ${controls_H_UI_FILES}    DESTINATION framework/include)
install (FILES ${salome_H_FILES}         DESTINATION framework/include)
install (FILES ${salome_H_UI_FILES}      DESTINATION framework/include)
install (FILES ${datum_H_FILES}          DESTINATION framework/include)
install (FILES ${datum_H_UI_FILES}       DESTINATION framework/include)
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
