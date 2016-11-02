//-----------------------------------------------------------------------------
// Created on: 19 June 2016 (nice airport in Munich)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogCloudify.h>

// GUI includes
#include <asiUI_Common.h>

// Geometry includes
#include <asiAlgo_Cloudify.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param model    [in] Data Model instance.
//! \param notifier [in] progress notifier.
//! \param plotter  [in] imperative plotter.
//! \param parent   [in] parent widget.
asiUI_DialogCloudify::asiUI_DialogCloudify(const Handle(asiEngine_Model)& model,
                                           ActAPI_ProgressEntry           notifier,
                                           ActAPI_PlotterEntry            plotter,
                                           QWidget*                       parent)
//
: asiUI_Dialog(notifier, plotter, parent), m_model(model)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Options");

  // Editors
  m_widgets.pLinearStep = new asiUI_LineEdit();

  // Sizing
  m_widgets.pLinearStep->setMinimumWidth(CONTROL_EDIT_WIDTH);

  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
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
asiUI_DialogCloudify::~asiUI_DialogCloudify()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on perform.
void asiUI_DialogCloudify::onPerform()
{
  // Convert user input to double
  const double linStep = QVariant( m_widgets.pLinearStep->text() ).toDouble();

  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  TIMER_NEW
  TIMER_GO

  // Cloud builder
  asiAlgo_Cloudify cloud_builder(linStep, m_notifier, m_plotter);
  //
  Handle(asiAlgo_PointCloud) cloud;
  if ( !cloud_builder.Sample_Faces(part, cloud) )
  {
    std::cout << "Error: cannot build point cloud" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Cloudification")

  ActAPI_PlotterEntry IV(m_plotter);
  IV.DRAW_POINTS( cloud->GetPoints(), Color_Red, "Scanner imitation" );

  // Select filename for the output file
  QString
    qfilename = asiUI_Common::selectFile(QStringList() << "*.xyz",
                                         "", "Choose file to store the point cloud",
                                         asiUI_Common::OpenSaveAction_Save);
  //
  TCollection_AsciiString filename = QStr2AsciiStr(qfilename);

  if ( !cloud->SaveAs(filename) )
  {
    std::cout << "Error: cannot save file" << std::endl;
  }

  // Close
  this->close();
}
