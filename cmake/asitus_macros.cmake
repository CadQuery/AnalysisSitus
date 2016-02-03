# Useful macro

#-------------------------------------------------------------------------------
# Name:    ASITUS_ADD_SUBDIR0
# Purpose: adds sources of the given directory into the project
#-------------------------------------------------------------------------------
macro (ASITUS_ADD_SUBDIR0 DIRNAME)
  unset (H_UI_FILES)
  unset (H_FILES)
  unset (CPP_FILES)
  #
  add_subdirectory (${DIRNAME})
  #
  foreach (FILE ${H_UI_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    #
    unset (MOC_FILE)
    qt4_wrap_cpp(MOC_FILE ${DIRNAME}/${FILE})
    message (STATUS "... Info: next MOC file ${MOC_FILE}")
    set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
    #
    source_group ("MOC" FILES "${MOC_FILE}")
  endforeach (FILE)
  #
  foreach (FILE ${H_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
  endforeach (FILE)
  #
  foreach (FILE ${CPP_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
  endforeach (FILE)
endmacro()

#-------------------------------------------------------------------------------
# Name:    ASITUS_ADD_SUBDIR1
# Purpose: adds sources of the given directory into the project. Passing
#          GROUPNAME you specify an MSVC filter where to put all sources
#          from the DIRNAME directory
#-------------------------------------------------------------------------------
macro (ASITUS_ADD_SUBDIR1 GROUPNAME DIRNAME)
  unset (H_UI_FILES)
  unset (H_FILES)
  unset (CPP_FILES)
  #
  add_subdirectory (${DIRNAME})
  #
  foreach (FILE ${H_UI_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    source_group ("Header Files\\${GROUPNAME}" FILES "${DIRNAME}/${FILE}")
    #
    unset (MOC_FILE)
    qt4_wrap_cpp(MOC_FILE ${DIRNAME}/${FILE})
    message (STATUS "... Info: next MOC file ${MOC_FILE}")
    set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
    #
    source_group ("MOC" FILES "${MOC_FILE}")
  endforeach (FILE)
  #
  foreach (FILE ${H_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    source_group ("Header Files\\${GROUPNAME}" FILES "${DIRNAME}/${FILE}")
  endforeach (FILE)
  #
  foreach (FILE ${CPP_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    source_group ("Source Files\\${GROUPNAME}" FILES "${DIRNAME}/${FILE}")
  endforeach (FILE)
endmacro()

#-------------------------------------------------------------------------------
# Name:    ASITUS_ADD_SUBDIR2
# Purpose: adds sources of the given directory into the project. Passing
#          GROUPNAME and SUBGROUPNAME you specify an MSVC filter with a
#          nested filter where to put all sources from the DIRNAME directory
#-------------------------------------------------------------------------------
macro (ASITUS_ADD_SUBDIR2 GROUPNAME SUBGROUPNAME DIRNAME)
  unset (H_UI_FILES)
  unset (H_FILES)
  unset (CPP_FILES)
  #
  add_subdirectory (${DIRNAME})
  #
  foreach (FILE ${H_UI_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    source_group ("Header Files\\${GROUPNAME}\\${SUBGROUPNAME}"
                   FILES
                  "${DIRNAME}/${FILE}")
    #
    unset (MOC_FILE)
    qt4_wrap_cpp(MOC_FILE ${DIRNAME}/${FILE})
    message (STATUS "... Info: next MOC file ${MOC_FILE}")
    set (ui_moc_files ${ui_moc_files} ${MOC_FILE})
    #
    source_group ("MOC" FILES "${MOC_FILE}")
  endforeach (FILE)
  #
  foreach (FILE ${H_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    source_group ("Header Files\\${GROUPNAME}\\${SUBGROUPNAME}"
                   FILES
                  "${DIRNAME}/${FILE}")
  endforeach (FILE)
  #
  foreach (FILE ${CPP_FILES})
    set (src_files ${src_files} ${DIRNAME}/${FILE})
    source_group ("Source Files\\${GROUPNAME}\\${SUBGROUPNAME}"
                   FILES
                  "${DIRNAME}/${FILE}")
  endforeach (FILE)
endmacro()

#-------------------------------------------------------------------------------
# Name:    ASITUS_MAKE_PLATFORM_SHORT_NAME
# Purpose: initializes PLATFORM variable with a relevant value
#-------------------------------------------------------------------------------
macro (ASITUS_MAKE_PLATFORM_SHORT_NAME)
  if (MSVC)
    set (PLATFORM win)
  else()
    set (PLATFORM lin)
  endif()
endmacro()

#-------------------------------------------------------------------------------
# Name:    ASITUS_MAKE_COMPILER_SHORT_NAME
# Purpose: initializes COMPILER variable with a relevant value
#-------------------------------------------------------------------------------
macro (ASITUS_MAKE_COMPILER_SHORT_NAME)
  if (MSVC)
    if (MSVC70)
      set (COMPILER vc7)
    elseif (MSVC80)
      set (COMPILER vc8)
    elseif (MSVC90)
      set (COMPILER vc9)
    elseif (MSVC10)
      set (COMPILER vc10)
    elseif (MSVC11)
      set (COMPILER vc11)
    elseif (MSVC12)
      set (COMPILER vc12)
    elseif (MSVC14)
      set (COMPILER vc14)
    endif()
  elseif (DEFINED CMAKE_COMPILER_IS_GNUCC)
    set (COMPILER gcc)
  elseif (DEFINED CMAKE_COMPILER_IS_GNUCXX)
    set (COMPILER gxx)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (COMPILER clang)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set (COMPILER icc)
  else()
    set (COMPILER ${CMAKE_GENERATOR})
    string (REGEX REPLACE " " "" COMPILER ${COMPILER})
  endif()
endmacro()

#-------------------------------------------------------------------------------
# Name:    ASITUS_MAKE_COMPILER_BITNESS
# Purpose: initializes COMPILER_BITNESS variable with a relevant value
#-------------------------------------------------------------------------------
macro (ASITUS_MAKE_COMPILER_BITNESS)
  math (EXPR COMPILER_BITNESS "32 + 32*(${CMAKE_SIZEOF_VOID_P}/8)")
endmacro()
