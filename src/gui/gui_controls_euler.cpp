//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_euler.h>

// GUI includes
#include <gui_common.h>
#include <gui_dialog_euler.h>

// Common includes
#include <common_facilities.h>

// Qt includes
#include <QGroupBox>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_euler::gui_controls_euler(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pCheckEulerPoincare = new QPushButton("Check Euler-Poincare");
  //
  m_widgets.pCheckEulerPoincare -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for controls
  QGroupBox*   pEulerGroup = new QGroupBox("Euler operations");
  QVBoxLayout* pEulerLay   = new QVBoxLayout(pEulerGroup);
  //
  pEulerLay->addWidget(m_widgets.pCheckEulerPoincare);

  // Set layout
  m_pMainLayout->addWidget(pEulerGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pCheckEulerPoincare, SIGNAL( clicked() ), SLOT( onCheckEulerPoincare() ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
gui_controls_euler::~gui_controls_euler()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Checks Euler-Poincare equation.
void gui_controls_euler::onCheckEulerPoincare()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Run dialog
  gui_dialog_euler* wEuler = new gui_dialog_euler(this);
  wEuler->show();
}
