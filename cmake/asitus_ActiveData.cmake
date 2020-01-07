ASITUS_THIRDPARTY_PRODUCT("active_data" "" "ActData.h" "ActiveData")

message (STATUS "... Active Data Include dirs: ${3RDPARTY_active_data_INCLUDE_DIR}")
message (STATUS "... Active Data Library dirs: ${3RDPARTY_active_data_LIBRARY_DIR}")
message (STATUS "... Active Data Binary  dirs: ${3RDPARTY_active_data_DLL_DIR}")

if (3RDPARTY_active_data_LIBRARY_DIR)
  string (REPLACE lib libd 3RDPARTY_active_data_LIBRARY_DIR_DEBUG ${3RDPARTY_active_data_LIBRARY_DIR})
endif()

if (3RDPARTY_active_data_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}")
  if (WIN32)
    if (NOT EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/ActiveData.lib")
      set (3RDPARTY_active_data_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  else()
    if (NOT EXISTS "${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}/libActiveData.so")
      set (3RDPARTY_active_data_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

if (WIN32)
  string (REPLACE bin bind 3RDPARTY_active_data_DLL_DIR_DEBUG ${3RDPARTY_active_data_DLL_DIR})
  if (3RDPARTY_active_data_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_active_data_DLL_DIR_DEBUG}")
    if (NOT EXISTS "${3RDPARTY_active_data_DLL_DIR_DEBUG}/ActiveData.dll")
      set (3RDPARTY_active_data_DLL_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

message (STATUS "... Active Data Debug Library dirs: ${3RDPARTY_active_data_LIBRARY_DIR_DEBUG}")
message (STATUS "... Active Data Debug Binary  dirs: ${3RDPARTY_active_data_DLL_DIR_DEBUG}")

#--------------------------------------------------------------------------
# Installation
if (WIN32)
  install (FILES ${3RDPARTY_active_data_DLL_DIR}/ActiveData.dll DESTINATION bin)
  install (FILES ${3RDPARTY_active_data_DLL_DIR}/ActiveDataAPI.dll DESTINATION bin)
  install (FILES ${3RDPARTY_active_data_DLL_DIR}/ActiveDataAux.dll DESTINATION bin)
endif()
