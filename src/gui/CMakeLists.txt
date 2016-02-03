# List files
#------------------------------------------------------------------------------
set (H_UI_FILES

  gui_controls_mesh.h
  gui_controls_part.h
  gui_controls_section.h
  gui_controls_skinning.h
  gui_line_edit.h
  gui_main_window_asitus.h
  gui_main_window_dmu.h
  gui_main_window_meshedit.h
  gui_main_window_skinning.h
  gui_main_window_ubend.h
  gui_object_browser.h
  gui_prim_data_view.h
  gui_splitter.h
  gui_viewer.h
  gui_viewer_domain.h
  gui_viewer_mesh.h
  gui_viewer_part.h
  gui_viewer_section.h
  gui_viewer_skinner.h
  gui_viewer_surface.h
  gui_welcome_window.h

  PARENT_SCOPE
)
#------------------------------------------------------------------------------
set (H_FILES

  gui_common.h
  gui_data_adaptor.h
  gui_prim_data_adaptor.h
  gui_splitter.h

  PARENT_SCOPE
)
#------------------------------------------------------------------------------
set (CPP_FILES

  gui_controls_mesh.cpp
  gui_controls_part.cpp
  gui_controls_section.cpp
  gui_controls_skinning.cpp
  gui_data_adaptor.cpp
  gui_line_edit.cpp
  gui_main_window_asitus.cpp
  gui_main_window_dmu.cpp
  gui_main_window_meshedit.cpp
  gui_main_window_skinning.cpp
  gui_main_window_ubend.cpp
  gui_object_browser.cpp
  gui_prim_data_adaptor.cpp
  gui_prim_data_view.cpp
  gui_splitter.cpp
  gui_viewer.cpp
  gui_viewer_domain.cpp
  gui_viewer_mesh.cpp
  gui_viewer_part.cpp
  gui_viewer_section.cpp
  gui_viewer_skinner.cpp
  gui_viewer_surface.cpp
  gui_welcome_window.cpp

  PARENT_SCOPE
)
