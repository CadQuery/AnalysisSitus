project (caf_browser)

#------------------------------------------------------------------------------

set (H_MOC_FILES
  DFBrowser_QTGUI2.h
)

# List files
set (H_FILES
  caf_browser.h
  DFBrowser.hxx
  DFBrowser_Attr.hxx
  DFBrowser_AttrNode.hxx
  DFBrowser_AttrPrs.hxx
  DFBrowser_Colors.hxx
  DFBrowser_DFNode.hxx
  DFBrowser_DFTree.hxx
  DFBrowser_GUI.hxx
  DFBrowser_LabelNode.hxx
  DFBrowser_Level.hxx
  DFBrowser_ListNode.hxx
  DFBrowser_NodeType.hxx
  DFBrowser_NSNode.hxx
  DFBrowser_Picture.hxx
  DFBrowser_PtrGUI.hxx
  DFBrowser_PtrQTGUI.hxx
  DFBrowser_PtrQTGUIMOC.hxx
  DFBrowser_QTGUI.hxx
  Handle_DFBrowser_Attr.hxx
  Handle_DFBrowser_AttrNode.hxx
  Handle_DFBrowser_DFNode.hxx
  Handle_DFBrowser_DFTree.hxx
  Handle_DFBrowser_LabelNode.hxx
  Handle_DFBrowser_ListNode.hxx
  Handle_DFBrowser_NSNode.hxx
)

#------------------------------------------------------------------------------

set (CPP_FILES
  DFBrowser.cxx
  DFBrowser_Attr.cxx
  DFBrowser_AttrNode.cxx
  DFBrowser_DFNode.cxx
  DFBrowser_DFTree.cxx
  DFBrowser_GUI.cxx
  DFBrowser_LabelNode.cxx
  DFBrowser_ListNode.cxx
  DFBrowser_NSNode.cxx
  DFBrowser_QTGUI.cxx
  DFBrowser_QTGUI2.cxx
)

#------------------------------------------------------------------------------

set (LIB_FILES
  TKernel
  TKMath
  TKBRep
  TKOffset
  TKTopAlgo
  TKG2d
  TKG3d
  TKGeomBase
  TKGeomAlgo
  TKMesh
  TKShHealing
  TKBO
  TKPrim
  TKService
  TKGeomBase
  TKCDF
  TKV3d
  TKCAF
  TKVCAF
  TKLCAF
)

#------------------------------------------------------------------------------

foreach (FILE ${H_FILES})
  source_group ("Header Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${H_MOC_FILES})

  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  message (STATUS "... Info: next MOC file ${MOC_FILE}")
  set (caf_moc_files ${caf_moc_files} ${MOC_FILE})

  source_group ("Generated" FILES "${MOC_FILE}")
  source_group ("Header Files" FILES "${FILE}")
endforeach (FILE)

foreach (FILE ${CPP_FILES})
  source_group ("Source Files" FILES "${FILE}")
endforeach (FILE)

# Create include variable
set (caf_browser_include_dir_loc "${CMAKE_CURRENT_SOURCE_DIR};")
#
set (caf_browser_include_dir ${caf_browser_include_dir_loc} PARENT_SCOPE)

include_directories(${caf_browser_include_dir_loc} ${3RDPARTY_OCCT_INCLUDE_DIR})

add_library (caf_browser SHARED ${H_FILES} ${caf_moc_files} ${CPP_FILES})
qt5_use_modules(caf_browser Widgets)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

LINK_PROJECT_TO_LIBRARIES_SET(caf_browser 3RDPARTY_OCCT_LIBRARY_DIR LIB_FILES)

#------------------------------------------------------------------------------

install (TARGETS caf_browser RUNTIME DESTINATION bin)
