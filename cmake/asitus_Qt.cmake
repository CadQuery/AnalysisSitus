# Qt is searched manually first (just determine root)
message (STATUS "Processing Qt 3-rd party")

if (NOT DEFINED ${3RDPARTY_QT_DIR} AND ${3RDPARTY_QT_DIR} STREQUAL "")
  ASITUS_FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" Qt 3RDPARTY_QT_DIR_NAME)
  
  if (NOT DEFINED ${3RDPARTY_QT_DIR_NAME} AND ${3RDPARTY_QT_DIR_NAME} STREQUAL "")
    message (FATAL_ERROR "... Qt root directory was not found")
  endif()

  # Combine directory name with absolute path and show in GUI
  set (3RDPARTY_QT_DIR "${3RDPARTY_DIR}/${3RDPARTY_QT_DIR_NAME}" CACHE PATH "The directory containing Qt" FORCE)
  message (STATUS "... Qt root directory: ${3RDPARTY_QT_DIR}")
endif()

# Now set CMAKE_PREFIX_PATH to point to local Qt installation.
# Without this setting find_package() will not work
set(CMAKE_PREFIX_PATH ${3RDPARTY_QT_DIR})

# Now we can apply standard CMake finder for Qt5. We do this mostly
# to have qt5_wrap_cpp() function available
find_package(Qt5 REQUIRED COMPONENTS Widgets Core)
message (STATUS "... Qt cmake configuration at ${Qt5_DIR}")

# Hide specific paths
mark_as_advanced (Qt5Gui_DIR)
mark_as_advanced (Qt5Widgets_DIR)
mark_as_advanced (Qt5Core_DIR)
mark_as_advanced (Qt5_DIR)

#--------------------------------------------------------------------------
# Installation
if (WIN32)
  message (STATUS "... Qt binaries: ${3RDPARTY_QT_DIR}")

  install (FILES ${3RDPARTY_QT_DIR}/bin/Qt5Core.dll            DESTINATION bin)
  install (FILES ${3RDPARTY_QT_DIR}/bin/Qt5Gui.dll             DESTINATION bin)
  install (FILES ${3RDPARTY_QT_DIR}/bin/Qt5Widgets.dll         DESTINATION bin)
  install (FILES ${3RDPARTY_QT_DIR}/bin/Qt5Svg.dll             DESTINATION bin)

  install (DIRECTORY ${3RDPARTY_QT_DIR}/plugins/imageformats/   DESTINATION bin/imageformats/)
  install (DIRECTORY ${3RDPARTY_QT_DIR}/plugins/platforms/      DESTINATION bin/platforms/)
  install (DIRECTORY ${3RDPARTY_QT_DIR}/qml/Qt/                 DESTINATION bin/Qt/)
  install (DIRECTORY ${3RDPARTY_QT_DIR}/qml/QtGraphicalEffects/ DESTINATION bin/QtGraphicalEffects/)
endif()
