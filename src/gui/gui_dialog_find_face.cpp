//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_dialog_find_face.h>

// Common includes
#include <common_facilities.h>

// GUI includes
#include <gui_common.h>

// Engine includes
#include <engine_part.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// OCCT includes
#include <TopExp.hxx>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_dialog_find_face::gui_dialog_find_face(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Face of interest");

  // Editors
  m_widgets.pIndex = new gui_line_edit();

  // Sizing
  m_widgets.pIndex->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pIndex->setText("");

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pFind = new QPushButton("Find");

  // Sizing
  m_widgets.pFind->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pFind, SIGNAL( clicked() ), this, SLOT( onFind() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Face index (1-based):"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pIndex, 0, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pFind);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Find face by index");
}

//! Destructor.
gui_dialog_find_face::~gui_dialog_find_face()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Find" button.
void gui_dialog_find_face::onFind()
{
  // Read user inputs
  const int face_id = m_widgets.pIndex->text().toInt();

  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TopTools_IndexedMapOfShape faces, found;
  TopExp::MapShapes(part, TopAbs_FACE, faces);
  //
  const TopoDS_Shape& face = faces.FindKey(face_id);
  found.Add(face);

  // Highlight
  engine_part::HighlightSubShapes(found);
  //
  common_facilities::Instance()->ViewerPart->onSubShapesPicked();

  // Close
  this->close();
}
