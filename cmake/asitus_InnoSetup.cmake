message (STATUS "Processing Inno Setup 3-rd party")

DMU_FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" "innosetup" INNOSETUP_DIR)
set (INNOSETUP_DIR "${3RDPARTY_DIR}/${INNOSETUP_DIR}")
message (STATUS "... Inno Setup dir: ${INNOSETUP_DIR}")
set (INNOSETUP_EXE "${INNOSETUP_DIR}/ISCC${CMAKE_EXECUTABLE_SUFFIX}")
if (EXISTS "${INNOSETUP_EXE}")
  message (STATUS "... Inno Setup executable found: ${INNOSETUP_EXE}")
  set (INNOSETUP_EXE "${INNOSETUP_EXE}" CACHE PATH "Inno Setup - installator compiler" FORCE)
else()
  set (INNOSETUP_EXE "" CACHE PATH "Inno Setup - installator compiler" FORCE)
endif()


