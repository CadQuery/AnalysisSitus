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

#-------------------------------------------------------------------------------
# Name:    GENERATE_3RDPARTY_DEBUG_VARIABLES
# Purpose: Creates internal cache variables for 3rd-party debug directories
#          It is possible to use optional 3rd argument - LINUX_PREFIX
#          It is equal to "" by default
#-------------------------------------------------------------------------------
macro (GENERATE_3RDPARTY_DEBUG_VARIABLES 3RDPARTY_NAME 3RDPARTY_LIBRARY_NAME)
  set (LINUX_PREFIX "")
  if ( ${ARGC} EQUAL 3)
    set (LINUX_PREFIX "${ARGV2}")
  endif()

  if (3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR)
    string (REPLACE lib libd 3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG ${3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR})
    if (3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG AND EXISTS "${3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG}")
      if (WIN32)
        if (NOT EXISTS "${3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG}/${3RDPARTY_LIBRARY_NAME}.lib")
          set (3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
        endif()
      else()
        if (NOT EXISTS "${3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG}/${LINUX_PREFIX}${3RDPARTY_LIBRARY_NAME}.so")
          set (3RDPARTY_${3RDPARTY_NAME}_LIBRARY_DIR_DEBUG "" CACHE INTERNAL FORCE)
        endif()
      endif()
    endif()
  endif()

  if (WIN32 AND 3RDPARTY_${3RDPARTY_NAME}_DLL_DIR)
    string (REPLACE bin bind 3RDPARTY_${3RDPARTY_NAME}_DLL_DIR_DEBUG ${3RDPARTY_${3RDPARTY_NAME}_DLL_DIR})
    if (3RDPARTY_${3RDPARTY_NAME}_DLL_DIR_DEBUG AND EXISTS "${3RDPARTY_${3RDPARTY_NAME}_DLL_DIR_DEBUG}")
      if (NOT EXISTS "${3RDPARTY_${3RDPARTY_NAME}_DLL_DIR_DEBUG}/${3RDPARTY_LIBRARY_NAME}.dll")
        set (3RDPARTY_${3RDPARTY_NAME}_DLL_DIR_DEBUG "" CACHE INTERNAL FORCE)
      endif()
    endif()
  endif()
endmacro()

#-------------------------------------------------------------------------------
# Name:    ASITUS_UNSET
# Purpose: Unsets variable if it is defined
#-------------------------------------------------------------------------------
macro (ASITUS_UNSET VARNAME)
  if (DEFINED ${VARNAME})
    unset (${VARNAME} CACHE)
  endif()
endmacro()
