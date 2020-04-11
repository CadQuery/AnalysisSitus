ASITUS_THIRDPARTY_PRODUCT("tcl" "" "tcl.h" "tcl86")

# TBB
set (INSTALL_TBB OFF CACHE INTERNAL "" FORCE)
if (USE_THREADING OR OpenCASCADE_WITH_TBB)
  set (INSTALL_TBB ON CACHE INTERNAL "" FORCE)

  ASITUS_THIRDPARTY_PRODUCT("tbb" "tbb" "tbb.h" "tbb")
  if (USE_THREADING)
    add_definitions (-DUSE_THREADING)
  endif()
else()
  ASITUS_UNSET_3RDPARTY("tbb")
endif()

#--------------------------------------------------------------------------
# Installation
#--------------------------------------------------------------------------

if (WIN32)
  install (FILES ${3RDPARTY_tcl_DIR}/bin/tcl86.dll DESTINATION bin)
  install (FILES ${3RDPARTY_tcl_DIR}/bin/zlib1.dll DESTINATION bin)
else()
  install (FILES ${3RDPARTY_tcl_DIR}/lib/libtcl8.6.so DESTINATION bin)
endif()
