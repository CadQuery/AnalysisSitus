//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_main_window_skinning_h
#define gui_main_window_skinning_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>
#include <gui_viewer_section.h>
#include <gui_viewer_skinner.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Skinning composition.
class gui_main_window_skinning : public QMainWindow
{
  Q_OBJECT

public:

  gui_main_window_skinning();
  virtual ~gui_main_window_skinning();

public:

  void closeEvent(QCloseEvent* evt);

protected:

  void createSkinViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    gui_object_browser* wBrowser;       //!< Object browser.
    gui_viewer_skinner* wViewerSkin;    //!< Skin viewer.
    gui_viewer_section* wViewerSection; //!< Section viewer.

    t_widgets() : wBrowser(NULL),
                  wViewerSkin(NULL),
                  wViewerSection(NULL)
    {}

    void Release()
    {
      delete wBrowser;       wBrowser       = NULL;
      delete wViewerSkin;    wViewerSkin    = NULL;
      delete wViewerSection; wViewerSection = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif
