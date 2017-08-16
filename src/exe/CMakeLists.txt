project(exe)

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
  vtkGUISupportQt-7.1
  vtkInfovisLayout-7.1
  vtkIOCore-7.1
  vtkIOImage-7.1
  vtkImagingCore-7.1
  vtkInteractionStyle-7.1
  vtkInteractionWidgets-7.1
  vtkRenderingAnnotation-7.1
  vtkRenderingContext2D-7.1
  vtkRenderingContextOpenGL2-7.1
  vtkRenderingCore-7.1
  vtkRenderingFreeType-7.1
  vtkRenderingOpenGL2-7.1
  vtkViewsContext2D-7.1
  vtkViewsInfovis-7.1
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
                      ${asiAlgo_include_dir}
                      ${asiData_include_dir}
                      ${asiVisu_include_dir}
                      ${asiEngine_include_dir}
                      ${asiUI_include_dir}
                      ${3RDPARTY_OCCT_INCLUDE_DIR}
                      ${3RDPARTY_active_data_INCLUDE_DIR}
                      ${3RDPARTY_mobius_INCLUDE_DIR}
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

add_executable(exe ${ui_moc_files} ${src_files} ${RCC_FILE})

#------------------------------------------------------------------------------
# Configure template
#------------------------------------------------------------------------------

set (X_COMPILER_BITNESS "x${COMPILER_BITNESS}")

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/exe.vcxproj.user.in
               ${exe_BINARY_DIR}/exe.vcxproj.user @ONLY)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

qt5_use_modules(exe Core Widgets)

target_link_libraries(exe asiAlgo asiData asiVisu asiEngine asiUI)

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
    target_link_libraries (exe debug ${3RDPARTY_OCCT_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (exe optimized ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (exe ${3RDPARTY_OCCT_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${AD_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (exe debug ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (exe optimized ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (exe ${3RDPARTY_active_data_LIBRARY_DIR}/${LIB_FILENAME})
  endif()
endforeach()

foreach (LIB_FILE ${VTK_LIB_FILES})
  if (WIN32)
    set (LIB_FILENAME "${LIB_FILE}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set (LIB_FILENAME "lib${LIB_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME}")
    target_link_libraries (exe debug ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/${LIB_FILENAME})
    target_link_libraries (exe optimized ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
  else()
    target_link_libraries (exe ${3RDPARTY_vtk_LIBRARY_DIR}/${LIB_FILENAME})
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

  target_link_libraries (exe debug ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME_DEBUG})
  target_link_libraries (exe optimized ${3RDPARTY_tbb_LIBRARY_DIR}/${LIB_FILENAME})
endforeach()

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

install (TARGETS exe RUNTIME DESTINATION bin COMPONENT Runtime)
install (FILES resources/asitus/icon.ico DESTINATION bin)
install (FILES resources/asitus/icon64.ico DESTINATION bin)
