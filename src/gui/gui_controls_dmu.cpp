//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_dmu.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (mesh) includes
#include <mesh_convert.h>
#include <mesh_ply.h>

// VTK includes
#include <vtkDecimatePro.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

// Qt includes
#pragma warning(push, 0)
#include <QFileDialog>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_dmu::gui_controls_dmu(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadPly  = new QPushButton("Load &ply");
  m_widgets.pDecimate = new QPushButton("Decimate [VTK]");
  //
  m_widgets.pLoadPly  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pDecimate -> setMinimumWidth(BTN_MIN_WIDTH);

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(m_widgets.pLoadPly);
  m_pMainLayout->addWidget(m_widgets.pDecimate);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadPly,  SIGNAL( clicked() ), SLOT( onLoadPly() ) );
  connect( m_widgets.pDecimate, SIGNAL( clicked() ), SLOT( onDecimate() ) );
}

//! Destructor.
gui_controls_dmu::~gui_controls_dmu()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! On ply loading.
void gui_controls_dmu::onLoadPly()
{
  //
}

//-----------------------------------------------------------------------------

//! On decimation.
void gui_controls_dmu::onDecimate()
{
  //
}

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Allows to select filename for import.
//! \return selected filename.
QString gui_controls_dmu::selectPlyFile() const
{
  QStringList aFilter;
  aFilter << "PLY (*.ply)";

  QString dir;
  QString
    aFileName = QFileDialog::getOpenFileName(NULL, "Select ply file with mesh",
                                             dir, aFilter.join(";;"), NULL);

  return aFileName;
}
