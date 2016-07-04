//-----------------------------------------------------------------------------
// Created on: 19 June 2016 (nice airport in Munich)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_dialog_cloudify.h>

// GUI includes
#include <gui_common.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// Geometry includes
#include <geom_cloudify.h>
#include <geom_utils.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// OCCT includes
#include <BRepBndLib.hxx>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_dialog_cloudify::gui_dialog_cloudify(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Options");

  // Editors
  m_widgets.pLinearStep = new gui_line_edit();

  // Sizing
  m_widgets.pLinearStep->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;
  //
  // Depending on the bounding box of the part, we now choose the most
  // suitable parameters for sampling
  Bnd_Box AABB;
  BRepBndLib::Add(part, AABB);
  //
  const double linStep = AABB.CornerMin().Distance( AABB.CornerMax() ) * 0.01;
  //
  m_widgets.pLinearStep->setText( QString::number(linStep) );

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
  pGrid->addWidget(new QLabel("Linear step:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pLinearStep,  0, 1);
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
  this->setWindowTitle("Cloudification parameters");
}

//! Destructor.
gui_dialog_cloudify::~gui_dialog_cloudify()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on perform.
void gui_dialog_cloudify::onPerform()
{
  // Convert user input to double
  const double linStep = QVariant( m_widgets.pLinearStep->text() ).toDouble();

  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  TIMER_NEW
  TIMER_GO

  // Cloud builder
  geom_cloudify cloud_builder(linStep,
                              common_facilities::Instance()->Notifier,
                              common_facilities::Instance()->Plotter);
  //
  Handle(geom_point_cloud) cloud;
  if ( !cloud_builder.Sample_Faces(part, cloud) )
  {
    std::cout << "Error: cannot build point cloud" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Cloudification")

  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);
  IV.DRAW_POINTS( cloud->GetPoints(), Color_Red, "Scanner imitation" );

  // Close
  this->close();
}
