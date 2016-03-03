//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
#include <QGroupBox>
#include <QVBoxLayout>
#pragma warning(pop)

///
#include <DFBrowser.hxx>
#include <ActData_Application.h>

//-----------------------------------------------------------------------------

#define BTN_STANDARD_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_welcome_window::gui_welcome_window() : QMainWindow()
{
  this->createControls();
  //
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
  // Group decorators
  QGroupBox* pUtilitiesGroup = new QGroupBox("Utilities");
  QGroupBox* pSkinningGroup  = new QGroupBox("Skinning");

  // Buttons
  m_widgets.pDMU        = new QPushButton("CAD Simplification");
  m_widgets.pMeshEdit   = new QPushButton("Mesh View");
  m_widgets.pAnalysis   = new QPushButton("Analysis");
  m_widgets.pCAFBrowser = new QPushButton("CAF Browser");
  //
  m_widgets.pSkinning        = new QPushButton("Thru-Sections");
  m_widgets.pUBend           = new QPushButton("U-bend");
  m_widgets.pHullReconstruct = new QPushButton("Hull Reconstruction");
  //
  m_widgets.pDMU             -> setMinimumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pMeshEdit        -> setMinimumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pAnalysis        -> setMinimumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pCAFBrowser      -> setMinimumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pSkinning        -> setMinimumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pUBend           -> setMinimumWidth(BTN_STANDARD_WIDTH);
  m_widgets.pHullReconstruct -> setMinimumWidth(BTN_STANDARD_WIDTH);

  // Layouts for buttons
  QVBoxLayout* pUtilitiesLay = new QVBoxLayout(pUtilitiesGroup);
  QVBoxLayout* pSkinningLay  = new QVBoxLayout(pSkinningGroup);
  //
  pUtilitiesLay->addWidget(m_widgets.pDMU);
  pUtilitiesLay->addWidget(m_widgets.pMeshEdit);
  pUtilitiesLay->addWidget(m_widgets.pAnalysis);
  pUtilitiesLay->addWidget(m_widgets.pCAFBrowser);
  //
  pSkinningLay->addWidget(m_widgets.pSkinning);
  pSkinningLay->addWidget(m_widgets.pUBend);
  pSkinningLay->addWidget(m_widgets.pHullReconstruct);

  QVBoxLayout* pMainLayout = new QVBoxLayout();
  //
  pMainLayout->addWidget(pUtilitiesGroup);
  pMainLayout->addWidget(pSkinningGroup);
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
  connect( m_widgets.pDMU,        SIGNAL( clicked() ), SLOT( onDMU        () ) );
  connect( m_widgets.pMeshEdit,   SIGNAL( clicked() ), SLOT( onMeshEdit   () ) );
  connect( m_widgets.pAnalysis,   SIGNAL( clicked() ), SLOT( onAnalysis   () ) );
  connect( m_widgets.pCAFBrowser, SIGNAL( clicked() ), SLOT( onCAFBrowser () ) );
  //
  connect( m_widgets.pSkinning,        SIGNAL( clicked() ), SLOT( onSkinning        () ) );
  connect( m_widgets.pUBend,           SIGNAL( clicked() ), SLOT( onUBend           () ) );
  connect( m_widgets.pHullReconstruct, SIGNAL( clicked() ), SLOT( onHullReconstruct () ) );
}

//-----------------------------------------------------------------------------

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

//! Reaction to clicking "CAF Browser" button.
void gui_welcome_window::onCAFBrowser()
{
  this->hide();

  //---------------------------------------------------------------------------

  char BUFF[1024];
  GetEnvironmentVariable("THIS_DATA", BUFF, 1024);
  TCollection_AsciiString fn = BUFF;

  const Handle(ActData_Application)& anApp = ActData_Application::Instance();

  Handle(TDocStd_Document) aDoc;
  ActData_Application::Instance()->NewDocument("BinXCAF", aDoc);

  PCDM_ReaderStatus aStatus = PCDM_RS_OpenError;
  try
  {
    aStatus = anApp->Open(fn, aDoc);
  }
  catch ( Standard_Failure exc )
  {
    cout << "OCCT exception:" << endl;
    cout << exc.DynamicType()->Name() << endl;
    cout << exc.GetMessageString() << endl;
    return;
  }

  if ( aStatus != PCDM_RS_OK )
  {
    std::cout << "Error: unrecognized format" << std::endl;
    getchar();
    return;
  }

  //
  DFBrowser::Factory(aDoc);
}

//-----------------------------------------------------------------------------

//! Reaction to clicking "Skinning" button.
void gui_welcome_window::onSkinning()
{
  this->hide();
  //
  gui_main_window_skinning* pMainWindow = new gui_main_window_skinning();
  pMainWindow->show();
}

//! Reaction to clicking "U-bend" button.
void gui_welcome_window::onUBend()
{
  this->hide();
  //
  gui_main_window_ubend* pMainWindow = new gui_main_window_ubend();
  pMainWindow->show();
}

//! Reaction to clicking "Hull Reconstruction" button.
void gui_welcome_window::onHullReconstruct()
{
  this->hide();
  //
  //gui_main_window_hull* pMainWindow = new gui_main_window_hull();
  //pMainWindow->show();
}
