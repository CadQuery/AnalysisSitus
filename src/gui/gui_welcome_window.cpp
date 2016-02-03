//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_welcome_window.h>

// A-Situs (GUI) includes
#include <gui_main_window_asitus.h>
#include <gui_main_window_dmu.h>
#include <gui_main_window_meshedit.h>
#include <gui_main_window_skinning.h>
#include <gui_main_window_ubend.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define BTN_STANDARD_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_welcome_window::gui_welcome_window() : QMainWindow()
{
  this->createControls();

  this->setCentralWidget(m_widgets.pMainPane);
  this->setWindowTitle("Analysis Situs: choose composition");
}

//! Destructor.
gui_welcome_window::~gui_welcome_window()
{}

//-----------------------------------------------------------------------------

//! Creates all widgets.
void gui_welcome_window::createControls()
{
  // Buttons
  m_widgets.pDMU      = new QPushButton("dMU");
  m_widgets.pMeshEdit = new QPushButton("Mesh Editor");
  m_widgets.pAnalysis = new QPushButton("Analysis");
  m_widgets.pSkinning = new QPushButton("Skinning");
  m_widgets.pUBend    = new QPushButton("U-bend");
  //
  m_widgets.pDMU      ->setMaximumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pMeshEdit ->setMaximumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pAnalysis ->setMaximumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pSkinning ->setMaximumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pUBend    ->setMaximumWidth(BTN_STANDARD_WIDTH);
  //
  // Layout for buttons
  QVBoxLayout* pMainLayout = new QVBoxLayout();
  //
  pMainLayout->addWidget(m_widgets.pDMU);
  pMainLayout->addWidget(m_widgets.pMeshEdit);
  pMainLayout->addWidget(m_widgets.pAnalysis);
  pMainLayout->addWidget(m_widgets.pSkinning);
  pMainLayout->addWidget(m_widgets.pUBend);
  //
  pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  pMainLayout->setContentsMargins(10, 10, 10, 10);

  // Sizing
  const int width  = 150;
  const int height = 80;
  //
  m_widgets.pMainPane = new QWidget();
  m_widgets.pMainPane->setMinimumSize(width, height);
  m_widgets.pMainPane->setLayout(pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pUBend,    SIGNAL( clicked() ), SLOT( onUBend    () ) );
  connect( m_widgets.pDMU,      SIGNAL( clicked() ), SLOT( onDMU      () ) );
  connect( m_widgets.pMeshEdit, SIGNAL( clicked() ), SLOT( onMeshEdit () ) );
  connect( m_widgets.pAnalysis, SIGNAL( clicked() ), SLOT( onAnalysis () ) );
  connect( m_widgets.pSkinning, SIGNAL( clicked() ), SLOT( onSkinning () ) );
}

//-----------------------------------------------------------------------------

//! Reaction to clicking "U-bend" button.
void gui_welcome_window::onUBend()
{
  this->hide();
  //
  gui_main_window_ubend* pMainWindow = new gui_main_window_ubend();
  pMainWindow->show();
}

//! Reaction to clicking "DMU" button.
void gui_welcome_window::onDMU()
{
  this->hide();
  //
  gui_main_window_dmu* pMainWindow = new gui_main_window_dmu();
  pMainWindow->show();
}

//! Reaction to clicking "Mesh Edit" button.
void gui_welcome_window::onMeshEdit()
{
  this->hide();
  //
  gui_main_window_meshedit* pMainWindow = new gui_main_window_meshedit();
  pMainWindow->show();
}

//! Reaction to clicking "Analysis" button.
void gui_welcome_window::onAnalysis()
{
  this->hide();
  //
  gui_main_window_asitus* pMainWindow = new gui_main_window_asitus();
  pMainWindow->show();
}

//! Reaction to clicking "Skinning" button.
void gui_welcome_window::onSkinning()
{
  this->hide();
  //
  gui_main_window_skinning* pMainWindow = new gui_main_window_skinning();
  pMainWindow->show();
}
