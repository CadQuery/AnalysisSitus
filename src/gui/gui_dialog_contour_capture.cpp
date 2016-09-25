//-----------------------------------------------------------------------------
// Created on: 25 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_dialog_contour_capture.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// Engine includes
#include <engine_part.h>

// GUI includes
#include <gui_common.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_dialog_contour_capture::gui_dialog_contour_capture(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pPrecision   = new gui_line_edit();
  m_widgets.pHealContour = new QCheckBox();

  // Sizing
  m_widgets.pPrecision->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pPrecision->setText("0.1");
  m_widgets.pHealContour->setChecked(true);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pPerform = new QPushButton("Perform");

  // Sizing
  m_widgets.pPerform->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pPerform, SIGNAL( clicked() ), this, SLOT( onPerform() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Precision:"),    0, 0);
  pGrid->addWidget(new QLabel("Auto-healing:"), 1, 0);
  //
  pGrid->addWidget(m_widgets.pPrecision,   0, 1);
  pGrid->addWidget(m_widgets.pHealContour, 1, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pPerform);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Contour Capture");
}

//! Destructor.
gui_dialog_contour_capture::~gui_dialog_contour_capture()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void gui_dialog_contour_capture::onPerform()
{
  // Read user inputs
  const double precision     = m_widgets.pPrecision->text().toDouble();
  const bool   isAutoHealing = m_widgets.pHealContour->isChecked();

  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TIMER_NEW
  TIMER_GO

  // TODO: NYI

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Contour Capture")

  // Close
  this->close();
}
