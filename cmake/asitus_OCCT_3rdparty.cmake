ASITUS_THIRDPARTY_PRODUCT("tcl" "" "tcl.h" "tcl86")

#--------------------------------------------------------------------------
# Installation
#--------------------------------------------------------------------------

if (WIN32)
  install (FILES ${3RDPARTY_tcl_DIR}/bin/tcl86.dll DESTINATION bin)
  install (FILES ${3RDPARTY_tcl_DIR}/bin/zlib1.dll DESTINATION bin)
else()
  install (FILES ${3RDPARTY_tcl_DIR}/lib/libtcl8.6.so DESTINATION bin)
endif()
