# Useful macro

#-------------------------------------------------------------------------------
# Name:    COPY_RESOURCES_TO_BINARY_DIRS
# Purpose: Copies resource files to all binary directories - bin, bind, bini
#-------------------------------------------------------------------------------
macro (COPY_RESOURCES_TO_BINARY_DIRS RESOURCE_FILEPATH)
  message (STATUS "... Copying resource file ${PROJECT_SOURCE_DIR}/${RESOURCE_FILEPATH}")

    if ( TARGET ${PROJECT_NAME} )
      add_custom_command( TARGET ${PROJECT_NAME}
                          POST_BUILD
                          COMMENT "Updating resource-file '${RESOURCE_FILEPATH}'"
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/${RESOURCE_FILEPATH}" "\$(OutDir)/${RESOURCE_FILEPATH}" )
    endif()

  foreach(CONF "RELEASE" "RELWITHDEBINFO" "DEBUG")
    file (COPY "${RESOURCE_FILEPATH}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}}/resources")
  endforeach()
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

macro (ASITUS_UNSET VARNAME)
  if (DEFINED ${VARNAME})
    unset (${VARNAME} CACHE)
  endif()
endmacro()
