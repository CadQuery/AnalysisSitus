//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_skinning.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (common) includes
#include <common_facilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_skinning::gui_main_window_skinning() : QMainWindow()
{
  this->createSkinViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerSkin);
  this->setWindowTitle("Analysis Situs [Skinning]");
}

//! Destructor.
gui_main_window_skinning::~gui_main_window_skinning()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_skinning::closeEvent(QCloseEvent* evt)
{
  // It seems that we have to destruct objects properly and manually in
  // order to avoid some side effects from VTK. E.g. if we don't kill the
  // widgets explicitly here, we may sometimes get a warning window of VTK
  // saying that it lacks some resources
  m_widgets.Release();
  //
  evt->accept();
}

//-----------------------------------------------------------------------------

//! Creates main (part) viewer.
void gui_main_window_skinning::createSkinViewer()
{
  m_widgets.wViewerSkin = new gui_viewer_skinner();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.8;
  const int height = side*0.7;
  //
  m_widgets.wViewerSkin->setMinimumSize(width, height);

  // Actualize Presentation of skinning
  common_facilities::Instance()->Prs.Skinner->Actualize( common_facilities::Instance()->Model->GetSectionsNode().get() );
}

//! Creates main dockable widgets.
void gui_main_window_skinning::createDockWindows()
{
  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.2;

  //---------------------------------------------------------------------------
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("Stored Objects", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);

    // NOTE: in today's version it is important to construct Object Browser
    //       before section view as the section view will set itself as a
    //       listener to selection in browser
  }
  //---------------------------------------------------------------------------
  // Section viewer
  {
    QDockWidget* pDock = new QDockWidget("Single Section", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerSection = new gui_viewer_section(true, pDock);
    pDock->setWidget(m_widgets.wViewerSection);
    pDock->setMinimumWidth(width);
    pDock->setMinimumHeight(500);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
}
