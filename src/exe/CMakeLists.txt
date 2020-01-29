project(asiExe)

#------------------------------------------------------------------------------
# Common
#------------------------------------------------------------------------------

set (H_FILES
  exe_Keywords.h
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
set (VTK_LIB_FILES
  vtkChartsCore-8.2
  vtkCommonCore-8.2
  vtkCommonColor-8.2
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
  vtkIOExportOpenGL2-8.2
  vtkImagingCore-8.2
  vtkInteractionStyle-8.2
  vtkInteractionWidgets-8.2
  vtkRenderingAnnotation-8.2
  vtkRenderingContext2D-8.2
  vtkRenderingContextOpenGL2-8.2
  vtkRenderingCore-8.2
  vtkRenderingFreeType-8.2
  vtkRenderingOpenGL2-8.2
  vtkRenderingGL2PSOpenGL2-8.2
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
# Resources
#------------------------------------------------------------------------------

source_group ("Resources" FILES resources/asiExeDictionary.xml)

#------------------------------------------------------------------------------
# Configure includes
#------------------------------------------------------------------------------

# Create include variable
set (exe_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};\
  ${CMAKE_CURRENT_SOURCE_DIR}/ui;")
#
set (exe_include_dir ${exe_include_dir_loc} PARENT_SCOPE)

include_directories ( SYSTEM
                      ${exe_include_dir_loc}
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

if (USE_MOBIUS)
  include_directories(SYSTEM ${3RDPARTY_mobius_INCLUDE_DIR})
endif()

#------------------------------------------------------------------------------
# Qt resources
#------------------------------------------------------------------------------

qt5_add_resources(RCC_FILE resources/style.qrc)
source_group ("Resources" FILES resources/style.qrc)
source_group ("Generated" FILES "${RCC_FILE}")

#------------------------------------------------------------------------------
# Create executable
#------------------------------------------------------------------------------

add_executable(asiExe ${ui_moc_files} ${src_files} ${RCC_FILE} resources/asiExeDictionary.xml)

if (WIN32)
  COPY_RESOURCES_TO_BINARY_DIRS(resources/asiExeDictionary.xml)
endif()

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

#------------------------------------------------------------------------------
# Installation of Analysis Situs as a software
#------------------------------------------------------------------------------

if (WIN32)
  install (TARGETS asiExe RUNTIME DESTINATION bin COMPONENT Runtime)
else()
  install (FILES ${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin/asiExe DESTINATION bin
           PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                       GROUP_EXECUTE GROUP_READ
                       WORLD_EXECUTE WORLD_READ)
  install (FILES ${CMAKE_SOURCE_DIR}/asiExe.sh DESTINATION bin
           PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                       GROUP_EXECUTE GROUP_READ
                       WORLD_EXECUTE WORLD_READ)
endif()

install (FILES resources/asitus/icon.ico DESTINATION bin)
install (FILES resources/asitus/icon64.ico DESTINATION bin)
install (FILES resources/asiExeDictionary.xml DESTINATION bin/resources)
