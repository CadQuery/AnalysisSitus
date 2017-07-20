# Useful functions

# Macro
include (${CMAKE_SOURCE_DIR}/cmake/ASITUS_macros.cmake)

#-------------------------------------------------------------------------------
# Name:    ASITUS_RETRIEVE_VERSION
# Purpose: retrieves app version from version file
#-------------------------------------------------------------------------------
function (ASITUS_RETRIEVE_VERSION VERSION_FILE MAJOR MINOR PATCH)
  file(STRINGS ${VERSION_FILE} VERSION_FILE_DATA)
  foreach (line ${VERSION_FILE_DATA})
    string(REGEX MATCH "ASITUS_VERSION_MAJOR *[0-9]" match ${line})
    if (NOT "${match}" STREQUAL "")
      separate_arguments(match)
      list(GET match 1 num)
      set (${MAJOR} ${num} PARENT_SCOPE)
    endif()

    string(REGEX MATCH "ASITUS_VERSION_MINOR *[0-9]" match ${line})
    if (NOT "${match}" STREQUAL "")
      separate_arguments(match)
      list(GET match 1 num)
      set (${MINOR} ${num} PARENT_SCOPE)
    endif()

    string(REGEX MATCH "ASITUS_VERSION_PATCH *[0-9]" match ${line})
    if (NOT "${match}" STREQUAL "")
      separate_arguments(match)
      list(GET match 1 num)
      set (${PATCH} ${num} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

#-------------------------------------------------------------------------------
# Name:    ASITUS_SUBDIRECTORY_NAMES
# Purpose: retrieves subdirectory names
#-------------------------------------------------------------------------------
function (ASITUS_SUBDIRECTORY_NAMES MAIN_DIRECTORY RESULT)
  file (GLOB SUB_ITEMS "${MAIN_DIRECTORY}/*")

  foreach (ITEM ${SUB_ITEMS})
    if (IS_DIRECTORY "${ITEM}")
      get_filename_component (ITEM_NAME "${ITEM}" NAME)
      list (APPEND LOCAL_RESULT "${ITEM_NAME}")
    endif()
  endforeach()
  set (${RESULT} ${LOCAL_RESULT} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# Name:    ASITUS_FIND_PRODUCT_DIR
# Purpose: searches for a directory containing the 3-rd party product of the
#          given name
#-------------------------------------------------------------------------------
function (ASITUS_FIND_PRODUCT_DIR ROOT_DIR PRODUCT_NAME RESULT)
  ASITUS_MAKE_COMPILER_SHORT_NAME()
  ASITUS_MAKE_COMPILER_BITNESS()

  string (TOLOWER "${PRODUCT_NAME}" lower_PRODUCT_NAME)
  
  message (STATUS "... Searching for ${PRODUCT_NAME} in ${ROOT_DIR} for compiler ${COMPILER} and bitness ${COMPILER_BITNESS}")

  list (APPEND
        SEARCH_TEMPLATES
        "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*${COMPILER}.*${COMPILER_BITNESS}")

  list (APPEND
        SEARCH_TEMPLATES
        "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER}.*${COMPILER_BITNESS}")

  list (APPEND
        SEARCH_TEMPLATES
        "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER_BITNESS}")

  list (APPEND
        SEARCH_TEMPLATES
        "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+")

  list (APPEND
        SEARCH_TEMPLATES
        "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*")

  ASITUS_SUBDIRECTORY_NAMES ("${ROOT_DIR}" SUBDIR_NAME_LIST)

  foreach (SEARCH_TEMPLATE ${SEARCH_TEMPLATES})
    if (LOCAL_RESULT)
      BREAK()
    endif()

    foreach (SUBDIR_NAME ${SUBDIR_NAME_LIST})
      string (TOLOWER "${SUBDIR_NAME}" lower_SUBDIR_NAME)

      string (REGEX MATCH "${SEARCH_TEMPLATE}" DUMMY_VAR "${lower_SUBDIR_NAME}")
      if (DUMMY_VAR)
        list (APPEND LOCAL_RESULT ${SUBDIR_NAME})
      endif()
    endforeach()
  endforeach()

  if (LOCAL_RESULT)
    list (GET LOCAL_RESULT -1 DUMMY)
    set (${RESULT} ${DUMMY} PARENT_SCOPE)
  endif()

  message(STATUS "... Found: ${RESULT} = ${DUMMY}")
endfunction()
