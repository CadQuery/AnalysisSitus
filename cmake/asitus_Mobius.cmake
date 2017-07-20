ASITUS_THIRDPARTY_PRODUCT("mobius" "" "core.h" "mobiusCore")

message (STATUS "... Mobius Include dirs: ${3RDPARTY_mobius_INCLUDE_DIR}")
message (STATUS "... Mobius Library dirs: ${3RDPARTY_mobius_LIBRARY_DIR}")
message (STATUS "... Mobius Binary  dirs: ${3RDPARTY_mobius_DLL_DIR}")

string (REPLACE lib libd 3RDPARTY_mobius_LIBRARY_DIR_DEBUG ${3RDPARTY_mobius_LIBRARY_DIR})
if (3RDPARTY_mobius_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_mobius_LIBRARY_DIR_DEBUG}")
  if (WIN32)
    if (NOT EXISTS "${3RDPARTY_mobius_LIBRARY_DIR_DEBUG}/mobiusCore.lib")
      set (3RDPARTY_mobius_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  else()
    if (NOT EXISTS "${3RDPARTY_mobius_LIBRARY_DIR_DEBUG}/libmobiusCore.so")
      set (3RDPARTY_mobius_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

if (WIN32)
  string (REPLACE bin bind 3RDPARTY_mobius_DLL_DIR_DEBUG ${3RDPARTY_mobius_DLL_DIR})
  if (3RDPARTY_mobius_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_mobius_DLL_DIR_DEBUG}")
    if (NOT EXISTS "${3RDPARTY_mobius_DLL_DIR_DEBUG}/mobiusCore.dll")
      set (3RDPARTY_mobius_DLL_DIR_DEBUG "" CACHE INTERNAL FORCE)
    endif()
  endif()
endif()

message (STATUS "... Mobius Debug Library dirs: ${3RDPARTY_mobius_LIBRARY_DIR_DEBUG}")
message (STATUS "... Mobius Debug Binary  dirs: ${3RDPARTY_mobius_DLL_DIR_DEBUG}")

#--------------------------------------------------------------------------
# Installation
if (WIN32)
  install (FILES ${3RDPARTY_mobius_DLL_DIR}/mobiusCore.dll DESTINATION bin)
  install (FILES ${3RDPARTY_mobius_DLL_DIR}/mobiusBSpl.dll DESTINATION bin)
endif()
