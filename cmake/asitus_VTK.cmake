ASITUS_THIRDPARTY_PRODUCT("vtk" "" "vtkAlgorithm.h" "vtkCommonCore-7.1")

if (3RDPARTY_vtk_INCLUDE_DIR STREQUAL "")
  message (STATUS "... VTK Include dir is not conventional")
  list (REMOVE_ITEM 3RDPARTY_NOT_INCLUDED 3RDPARTY_vtk_INCLUDE_DIR)
  set (3RDPARTY_vtk_INCLUDE_DIR ${3RDPARTY_vtk_DIR}/include/vtk-7.0 CACHE FILEPATH "Non-conventional inc dir" FORCE)
endif()

message (STATUS "... VTK Include dirs: ${3RDPARTY_vtk_INCLUDE_DIR}")
message (STATUS "... VTK Library dirs: ${3RDPARTY_vtk_LIBRARY_DIR}")
message (STATUS "... VTK Binary  dirs: ${3RDPARTY_vtk_DLL_DIR}")

string (REPLACE lib libd 3RDPARTY_vtk_LIBRARY_DIR_DEBUG ${3RDPARTY_vtk_LIBRARY_DIR})
if (3RDPARTY_vtk_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}")
  if (WIN32)
    if (NOT EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/vtkCommonCore-7.0.lib")
      set (3RDPARTY_vtk_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  else()
    if (NOT EXISTS "${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}/libvtkCommonCore-7.0.so")
      set (3RDPARTY_vtk_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

if (WIN32)
  string (REPLACE bin bind 3RDPARTY_vtk_DLL_DIR_DEBUG ${3RDPARTY_vtk_DLL_DIR})
  if (3RDPARTY_vtk_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_vtk_DLL_DIR_DEBUG}")
    if (NOT EXISTS "${3RDPARTY_vtk_DLL_DIR_DEBUG}/vtkCommonCore-7.0.dll")
      set (3RDPARTY_vtk_DLL_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

message (STATUS "... VTK Debug Library dirs: ${3RDPARTY_vtk_LIBRARY_DIR_DEBUG}")
message (STATUS "... VTK Debug Binary  dirs: ${3RDPARTY_vtk_DLL_DIR_DEBUG}")

#--------------------------------------------------------------------------
# Installation
if (WIN32)

  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonCore-7.0.dll           DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonDataModel-7.0.dll      DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonExecutionModel-7.0.dll DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonMath-7.0.dll           DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonMisc-7.0.dll           DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonSystem-7.0.dll         DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkCommonTransforms-7.0.dll     DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtkFiltersCore-7.0.dll          DESTINATION bin)
  install (FILES ${3RDPARTY_vtk_DLL_DIR}/vtksys-7.0.dll                  DESTINATION bin)

endif()
