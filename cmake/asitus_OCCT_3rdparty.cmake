ASITUS_THIRDPARTY_PRODUCT("tcl" "" "tcl.h" "tcl86")
if (3RDPARTY_tcl_DIR AND NOT 3RDPARTY_tk_DIR)
  if (EXISTS ${3RDPARTY_tcl_INCLUDE_DIR}/tk.h)
    set (3RDPARTY_tk_DIR "${3RDPARTY_tcl_DIR}" CACHE PATH "The directory containing tk" FORCE)
  endif()
endif()
ASITUS_THIRDPARTY_PRODUCT("tk" "" "tk.h" "tk86")

ASITUS_THIRDPARTY_PRODUCT("freeimage" "" "FreeImage.h" "FreeImage")
if (3RDPARTY_freeimage_DIR AND NOT 3RDPARTY_freeimageplus_DIR)
  set (3RDPARTY_freeimageplus_DIR "${3RDPARTY_freeimage_DIR}" CACHE PATH "The directory containing freeimageplus" FORCE)
endif()
ASITUS_THIRDPARTY_PRODUCT("freeimageplus" "" "FreeImagePlus.h" "FreeImagePlus")

ASITUS_THIRDPARTY_PRODUCT("freetype" "" "ft2build.h" "freetype")
ASITUS_THIRDPARTY_PRODUCT("gl2ps" "" "gl2ps.h" "gl2ps")
ASITUS_THIRDPARTY_PRODUCT("tbb" "tbb" "tbb.h" "tbb")

#--------------------------------------------------------------------------
# Installation
if (WIN32)

  # Tcl
  install (FILES ${3RDPARTY_tcl_DIR}/bin/tcl86.dll DESTINATION bin)
  install (FILES ${3RDPARTY_tcl_DIR}/bin/zlib1.dll DESTINATION bin)

  # TBB
  install (FILES ${3RDPARTY_tbb_DIR}/bin/intel64/vc12/tbb.dll       DESTINATION bin)
  install (FILES ${3RDPARTY_tbb_DIR}/bin/intel64/vc12/tbbmalloc.dll DESTINATION bin)

  # FreeImage
  install (FILES ${3RDPARTY_freeimage_DIR}/bin/FreeImage.dll     DESTINATION bin)
  install (FILES ${3RDPARTY_freeimage_DIR}/bin/FreeImagePlus.dll DESTINATION bin)

  # Freetype
  install (FILES ${3RDPARTY_freetype_DIR}/bin/freetype.dll DESTINATION bin)

  # gl2ps
  install (FILES ${3RDPARTY_gl2ps_DIR}/bin/gl2ps.dll DESTINATION bin)

endif()
