//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_control_pane.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (mesh) includes
#include <mesh_ply.h>

// Qt includes
#pragma warning(push, 0)
#include <QFileDialog>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_control_pane::gui_control_pane(QWidget* parent) : QWidget(parent)
{
  // Set common properties
  this->setContentsMargins(0, 0, 0, 0);

  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_buttons.LoadPly = new QPushButton("Load &ply");

  // Set layout
  m_pMainLayout->setSpacing(0);
  m_pMainLayout->addWidget(m_buttons.LoadPly);
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_buttons.LoadPly, SIGNAL( clicked() ), SLOT( onLoadPly() ) );
}

//! Destructor.
gui_control_pane::~gui_control_pane()
{
  delete m_pMainLayout;
  m_buttons.Release();
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! On ply loading.
void gui_control_pane::onLoadPly()
{
  QString filename = this->selectPlyFile();

  Handle(OMFDS_Mesh) mesh;
  NCollection_Sequence<mesh_ply::TNamedArray> NodeArrays;
  NCollection_Sequence<mesh_ply::TNamedArray> ElemArrays;

  if ( !mesh_ply::Read(QStr2AsciiStr(filename), mesh, NodeArrays, ElemArrays ) )
  {
    std::cout << "Error: cannot read ply file" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Allows to select filename for import.
//! \return selected filename.
QString gui_control_pane::selectPlyFile() const
{
  QStringList aFilter;
  aFilter << "PLY (*.ply)";

  QString dir;
  QString
    aFileName = QFileDialog::getOpenFileName(NULL, "Select ply file with mesh",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}
