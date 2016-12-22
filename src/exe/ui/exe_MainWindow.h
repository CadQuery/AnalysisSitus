//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef exe_MainWindow_h
#define exe_MainWindow_h

// asiUI includes
#include <asiUI_ControlsFeature.h>
#include <asiUI_ControlsPartListener.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerPartListener.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Analysis Situs.
class exe_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  exe_MainWindow();
  virtual ~exe_MainWindow();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    asiUI_ObjectBrowser*   wBrowser;         //!< Object browser.
    asiUI_ViewerDomain*    wViewerDomain;    //!< Parametric domain viewer.
    asiUI_ViewerPart*      wViewerPart;      //!< Part viewer.
    asiUI_ViewerSurface*   wViewerSurface;   //!< Surface viewer.
    asiUI_ControlsPart*    wControlsPart;    //!< Part controls.
    asiUI_ControlsFeature* wControlsFeature; //!< Feature controls.

    t_widgets() : wBrowser         (NULL),
                  wViewerDomain    (NULL),
                  wViewerPart      (NULL),
                  wViewerSurface   (NULL),
                  wControlsPart    (NULL),
                  wControlsFeature (NULL)
    {}

    void Release()
    {
      delete wBrowser;         wBrowser         = NULL;
      delete wViewerDomain;    wViewerDomain    = NULL;
      delete wViewerPart;      wViewerPart      = NULL;
      delete wViewerSurface;   wViewerSurface   = NULL;
      delete wControlsPart;    wControlsPart    = NULL;
      delete wControlsFeature; wControlsFeature = NULL;
    }
  };

  //! Listeners.
  struct t_listeners
  {
    asiUI_ControlsPartListener* pControlsPart; //!< Listener for part controls.
    asiUI_ViewerPartListener*   pViewerPart;   //!< Listener for part viewer.

    t_listeners() : pControlsPart (NULL),
                    pViewerPart   (NULL)
    {}

    void Release()
    {
      delete pControlsPart; pControlsPart = NULL;
      delete pViewerPart;   pViewerPart   = NULL;
    }
  };

  t_widgets   m_widgets;
  t_listeners m_listeners;

};

#endif
