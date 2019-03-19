//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoExe_MainWindow_h
#define ottoExe_MainWindow_h

// asiUI includes
#include <asiUI_Console.h>
#include <asiUI_ControlsMeshListener.h>
#include <asiUI_ControlsPartListener.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ParameterEditorListener.h>
#include <asiUI_ViewerPartListener.h>

// ottoExe includes
#include <ottoExe_Controls.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#include <QTextEdit>
#pragma warning(pop)

//! Main window for Blades composition.
class ottoExe_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  ottoExe_MainWindow();
  virtual ~ottoExe_MainWindow();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private:

  //! Widgets.
  struct t_widgets
  {
    asiUI_ObjectBrowser*    wBrowser;       //!< Object browser.
    asiUI_ViewerPart*       wViewerPart;    //!< Part viewer.
    asiUI_ControlsPart*     wControlsPart;  //!< Part controls.
    asiUI_ControlsMesh*     wControlsMesh;  //!< Mesh controls.
    ottoExe_Controls*       wControls;      //!< Controls for blade builder.
    asiUI_StyledTextEdit*   wLogger;        //!< Logger.
    asiUI_Console*          wConsole;       //!< Console for scripting.
    asiUI_ParameterEditor*  wParamEditor;   //!< Parameter editor.

    t_widgets() : wBrowser          (NULL),
                  wViewerPart       (NULL),
                  wControlsPart     (NULL),
                  wControlsMesh     (NULL),
                  wControls         (NULL),
                  wLogger           (NULL),
                  wConsole          (NULL),
                  wParamEditor      (NULL)
    {}

    void Release()
    {
      delete wBrowser;          wBrowser          = NULL;
      delete wViewerPart;       wViewerPart       = NULL;
      delete wControlsMesh;     wControlsMesh     = NULL;
      delete wControlsPart;     wControlsPart     = NULL;
      delete wControls;         wControls         = NULL;
      delete wLogger;           wLogger           = NULL;
      delete wConsole;          wConsole          = NULL;
      delete wParamEditor;      wParamEditor      = NULL;
    }
  };

  //! Listeners.
  struct t_listeners
  {
    asiUI_ControlsPartListener*    pControlsPart; //!< Listener for part controls.
    asiUI_ControlsMeshListener*    pControlsMesh; //!< Listener for mesh controls.
    asiUI_ViewerPartListener*      pViewerPart;   //!< Listener for part viewer.
    asiUI_ParameterEditorListener* pParamEditor;  //!< Listener for parameter editor.

    t_listeners() : pControlsPart (NULL),
                    pControlsMesh (NULL),
                    pViewerPart   (NULL),
                    pParamEditor  (NULL)
    {}

    void Release()
    {
      delete pControlsPart; pControlsPart = NULL;
      delete pControlsMesh; pControlsMesh = NULL;
      delete pViewerPart;   pViewerPart   = NULL;
      delete pParamEditor;  pParamEditor  = NULL;
    }
  };

  t_widgets   m_widgets;
  t_listeners m_listeners;

};

#endif
