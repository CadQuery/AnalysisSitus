//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_dialog_detect_holes.h>

// Common includes
#include <common_facilities.h>

// Engine includes
#include <engine_part.h>

// Feature includes
#include <feature_detect_choles.h>

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
gui_dialog_detect_holes::gui_dialog_detect_holes(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pRadius            = new gui_line_edit();
  m_widgets.pIsHardFeatureMode = new QCheckBox();

  // Sizing
  m_widgets.pRadius->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pRadius->setText("10");
  m_widgets.pIsHardFeatureMode->setChecked(false);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pIdentify = new QPushButton("Identify");

  // Sizing
  m_widgets.pIdentify->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pIdentify, SIGNAL( clicked() ), this, SLOT( onIdentify() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Radius:"),            0, 0);
  pGrid->addWidget(new QLabel("Hard feature mode:"), 1, 0);
  //
  pGrid->addWidget(m_widgets.pRadius,            0, 1);
  pGrid->addWidget(m_widgets.pIsHardFeatureMode, 1, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pIdentify);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Holes identification");
}

//! Destructor.
gui_dialog_detect_holes::~gui_dialog_detect_holes()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Identify" button.
void gui_dialog_detect_holes::onIdentify()
{
  // Read user inputs
  const double radius     = m_widgets.pRadius->text().toDouble();
  const bool   isHardMode = m_widgets.pIsHardFeatureMode->isChecked();

  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Identify holes
  feature_detect_choles detector(part, radius, NULL,
                                 common_facilities::Instance()->Notifier,
                                 common_facilities::Instance()->Plotter);
  //
  detector.SetHardFeatureMode(isHardMode);
  //
  if ( !detector.Perform() )
  {
    std::cout << "Error: cannot identify holes" << std::endl;
    return;
  }

  // Get detected holes
  const TopTools_IndexedMapOfShape& holes = detector.GetResultFaces();
  if ( holes.IsEmpty() )
  {
    std::cout << "No holes detected with radius <= " << radius << std::endl;
    return;
  }
  else
    std::cout << holes.Extent() << " hole(s) detected with radius <= " << radius << std::endl;

  // Highlight
  engine_part::HighlightSubShapes(holes);

  // Close
  this->close();
}
