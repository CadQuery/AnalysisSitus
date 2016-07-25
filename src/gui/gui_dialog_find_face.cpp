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

// Geometry includes
#include <geom_utils.h>

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
  m_widgets.pUseAddress = new QCheckBox();
  m_widgets.pIndex      = new gui_line_edit();
  m_widgets.pAddress    = new gui_line_edit();

  // Sizing
  m_widgets.pIndex  ->setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pAddress->setMinimumWidth(CONTROL_EDIT_WIDTH);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pFind = new QPushButton("Find");

  // Sizing
  m_widgets.pFind->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pUseAddress, SIGNAL( clicked() ), this, SLOT( onUseAddress () ) );
  connect( m_widgets.pFind,       SIGNAL( clicked() ), this, SLOT( onFind       () ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Use face address:"),     0, 0);
  pGrid->addWidget(new QLabel("Face index (1-based):"), 1, 0);
  pGrid->addWidget(new QLabel("Face address:"),         2, 0);
  //
  pGrid->addWidget(m_widgets.pUseAddress, 0, 1);
  pGrid->addWidget(m_widgets.pIndex,      1, 1);
  pGrid->addWidget(m_widgets.pAddress,    2, 1);
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

  // Adjust initial state
  this->onUseAddress();
}

//! Destructor.
gui_dialog_find_face::~gui_dialog_find_face()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Use address" checkbox.
void gui_dialog_find_face::onUseAddress()
{
  m_widgets.pIndex->setEnabled( !m_widgets.pUseAddress->isChecked() );
  m_widgets.pAddress->setEnabled( m_widgets.pUseAddress->isChecked() );
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Find" button.
void gui_dialog_find_face::onFind()
{
  const bool useAddress = m_widgets.pUseAddress->isChecked();
  //
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TopTools_IndexedMapOfShape faces, found;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  if ( useAddress )
  {
    TCollection_AsciiString face_addr = QStr2AsciiStr( m_widgets.pAddress->text() );
    face_addr.LowerCase();

    for ( int f = 1; f <= faces.Extent(); ++f )
    {
      TCollection_AsciiString next_addr = geom_utils::ShapeAddr( faces(f) ).c_str();
      next_addr.LowerCase();

      if ( next_addr.IsEqual(face_addr) )
      {
        found.Add( faces(f) );
        break;
      }
    }
  }
  else
  {
    // Read user inputs
    const int face_id = m_widgets.pIndex->text().toInt();

    if ( face_id <= faces.Extent() )
    {
      const TopoDS_Shape& face = faces.FindKey(face_id);
      found.Add(face);
    }
    else
    {
      std::cout << "Error: input index is out of range" << std::endl;
    }
  }

  if ( !found.IsEmpty() )
  {
    // Highlight
    engine_part::HighlightSubShapes(found);
    //
    common_facilities::Instance()->ViewerPart->onSubShapesPicked();
  }
  else
  {
    std::cout << "No face was found" << std::endl;
  }

  // Close
  this->close();
}
