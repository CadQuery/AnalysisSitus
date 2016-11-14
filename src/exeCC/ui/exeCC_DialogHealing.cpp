//-----------------------------------------------------------------------------
// Created on: 26 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_DialogHealing.h>

// exeCC includes
#include <exeCC_CommonFacilities.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_Common.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// OCCT includes
#include <TopExp_Explorer.hxx>

// SPE includes
#include <Common_ReapproxContour.h>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
exeCC_DialogHealing::exeCC_DialogHealing(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pPrecision = new asiUI_LineEdit();

  // Sizing
  m_widgets.pPrecision->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pPrecision->setText("0.1");

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
  pGrid->addWidget(new QLabel("Precision:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pPrecision, 0, 1);
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
  this->setWindowTitle("Contour Healing");
}

//! Destructor.
exeCC_DialogHealing::~exeCC_DialogHealing()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void exeCC_DialogHealing::onPerform()
{
  // Read user inputs
  const double precision = m_widgets.pPrecision->text().toDouble();

  // Get part Node
  Handle(asiData_PartNode)
    part_n = exeCC_CommonFacilities::Instance()->Model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    std::cout << "Error: part is not defined" << std::endl;
    return;
  }

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  /* =================
   *  Perform healing
   * ================= */

  // Get contour wire
  TopoDS_Wire contourShape = contour_n->AsShape();

  // Count edges
  {
    int numEdges = 0;
    for ( TopExp_Explorer exp(contourShape, TopAbs_EDGE); exp.More(); exp.Next() )
      numEdges++;
    //
    std::cout << "Input contour contains " << numEdges << " edge(s)" << std::endl;
  }

  // Prepare journal
  Handle(SpeCore_Journal) Journal = SpeCore_Journal::Instance();

  TIMER_NEW
  TIMER_GO

  // Run healing
  Common_ReapproxContour Heal(contourShape, precision);
  TopoDS_Wire res;
  if ( !Heal(res, true, true, Journal) )
  {
    std::cout << "Error: cannot heal contour" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Contour Healing")

  Journal->Dump(std::cout);

  //---------------------------------------------------------------------------

  ActAPI_PlotterEntry IV(exeCC_CommonFacilities::Instance()->Plotter);

  const gp_Pnt& Center = Heal.Center();
  const gp_Vec& Ori    = Heal.Orientation();

  if ( Ori.Magnitude() < RealEpsilon() )
  {
    std::cout << "Error: cannot compute orientation vector" << std::endl;
    return;
  }

  IV.DRAW_POINT(Center, Color_Violet);
  IV.DRAW_VECTOR_AT(Center, Ori*2000, Color_Violet);

  //---------------------------------------------------------------------------

  // Count edges
  {
    int numEdges = 0;
    for ( TopExp_Explorer exp(res, TopAbs_EDGE); exp.More(); exp.Next() )
      numEdges++;
    //
    std::cout << "Output contour contains " << numEdges << " edge(s)" << std::endl;
  }

  /* ==========
   *  Finalize
   * ========== */

  exeCC_CommonFacilities::Instance()->Model->OpenCommand();
  {
    contour_n->SetGeometry(res);
  }
  exeCC_CommonFacilities::Instance()->Model->CommitCommand();

  // Actualize
  exeCC_CommonFacilities::Instance()->Prs.Part->DeletePresentation( contour_n.get() );
  exeCC_CommonFacilities::Instance()->Prs.Part->Actualize( contour_n.get() );

  // Close
  this->close();
}
