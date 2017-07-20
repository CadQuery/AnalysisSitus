project(setup)

if (NOT "${INSTALL_DIR}" STREQUAL "")
  set (SETUP_BIN_INPUT "${INSTALL_DIR}/bin" CACHE INTERNAL "" FORCE)
  set (SETUP_OUTPUT "${INSTALL_DIR}/setup" CACHE INTERNAL "" FORCE)
endif()

configure_file(config.iss.in ${setup_BINARY_DIR}/config.iss @ONLY)
configure_file(setup.iss ${setup_BINARY_DIR}/setup.iss COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/exe/resources/asitus/icon.ico ${setup_BINARY_DIR}/imgs/icon.ico COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/exe/resources/asitus/icon64.ico ${setup_BINARY_DIR}/imgs/icon64.ico COPYONLY)
configure_file(imgs/setup_image.bmp ${setup_BINARY_DIR}/imgs/setup_image.bmp COPYONLY)
configure_file(imgs/setup_image_small.bmp ${setup_BINARY_DIR}/imgs/setup_image_small.bmp COPYONLY)

source_group ("Source files" FILES setup.iss)
source_group ("Source files" FILES config.iss.in)

add_custom_target(setup
                  COMMAND ${CMAKE_COMMAND} -E echo_append "Compiling setup..."
                  COMMAND ${INNOSETUP_EXE} ${setup_BINARY_DIR}/setup.iss
                  COMMAND ${CMAKE_COMMAND} -E echo "Done."
                  SOURCES setup.iss config.iss.in)
