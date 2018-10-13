project(asiExe)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  exe_Version.h
)
set (CPP_FILES
  exe_Main.cpp
)

#------------------------------------------------------------------------------
# UI
#------------------------------------------------------------------------------

set (ui_H_FILES
  ui/exe_CommonFacilities.h
)
set (ui_H_UI_FILES
  ui/exe_MainWindow.h
)
set (ui_CPP_FILES 
  ui/exe_CommonFacilities.cpp
  ui/exe_MainWindow.cpp
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
  vtkIOExportOpenGL2-8.0
  vtkImagingCore-8.0
  vtkInteractionStyle-8.0
  vtkInteractionWidgets-8.0
  vtkRenderingAnnotation-8.0
  vtkRenderingContext2D-8.0
  vtkRenderingContextOpenGL2-8.0
  vtkRenderingCore-8.0
  vtkRenderingFreeType-8.0
  vtkRenderingOpenGL2-8.0
  vtkRenderingGL2PSOpenGL2-8.0
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

foreach (FILE ${CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${ui_H_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Header Files\\UI" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${ui_H_UI_FILES})
  set (src_files ${src_files} ${FILE})
  #
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
  #
  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files\\UI" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${ui_CPP_FILES})
  set (src_files ${src_files} ${FILE})
  source_group ("Source Files\\UI" FILES "${FILE}")
endforeach (FILE)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (exe_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/ui;")
#
set (exe_include_dir ${exe_include_dir_loc} PARENT_SCOPE)

include_directories ( ${exe_include_dir_loc}
                      ${asiTcl_include_dir}
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
                      ${asiEngine_include_dir}
                      ${asiUI_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_EIGEN_DIR}
                      ${3RDPARTY_vtk_INCLUDE_DIR}
                      ${3RDPARTY_tbb_INCLUDE_DIR} )

#------------------------------------------------------------------------------
# Qt resources
#------------------------------------------------------------------------------

qt5_add_resources(RCC_FILE resources/style.qrc)
source_group ("Resources" FILES resources/style.qrc)
source_group ("Generated" FILES "${RCC_FILE}")

#------------------------------------------------------------------------------
# Create executable
#------------------------------------------------------------------------------

add_executable(asiExe ${ui_moc_files} ${src_files} ${RCC_FILE})

set (USE_CONSOLE OFF CACHE BOOL "Using of debug console")
if (WIN32 AND MSVC)
  if (USE_CONSOLE)
    set_target_properties(asiExe PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup" OUTPUT_NAME "asiExe")
  else ()
    set_target_properties(asiExe PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup" OUTPUT_NAME "asiExe")
  endif ()
endif()

#------------------------------------------------------------------------------
# Configure template
#------------------------------------------------------------------------------

set (X_COMPILER_BITNESS "x${COMPILER_BITNESS}")

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/exePROTOTYPE.vcxproj.user.in
               ${asiExe_BINARY_DIR}/asiExe.vcxproj.user @ONLY)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

qt5_use_modules(asiExe Core Widgets)

target_link_libraries(asiExe asiTcl asiAlgo asiData asiVisu asiEngine asiUI)

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
    target_link_libraries (asiExe debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiExe optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiExe ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiExe debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiExe optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiExe ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (asiExe debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (asiExe optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (asiExe ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
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

  target_link_libraries (asiExe debug ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME_DEBUG})
  target_link_libraries (asiExe optimized ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME})
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS asiExe RUNTIME DESTINATION bin COMPONENT Runtime)
install (FILES resources/asitus/icon.ico DESTINATION bin)
install (FILES resources/asitus/icon64.ico DESTINATION bin)
