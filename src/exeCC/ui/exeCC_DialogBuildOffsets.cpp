//-----------------------------------------------------------------------------
// Created on: 25 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_DialogBuildOffsets.h>

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
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

// SPE includes
#include <SpeCore_Hull.h>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param plate  [in] plate base to offset.
//! \param parent [in] parent widget.
exeCC_DialogBuildOffsets::exeCC_DialogBuildOffsets(const Handle(SpeCore_Plate)& plate,
                                                   QWidget*                     parent)
//
: QDialog(parent), m_plate(plate)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pOffset    = new asiUI_LineEdit();
  m_widgets.pThickness = new asiUI_LineEdit();
  m_widgets.pShift     = new asiUI_LineEdit();

  // Sizing
  m_widgets.pOffset    -> setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pThickness -> setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pShift     -> setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pOffset    -> setText("100");
  m_widgets.pThickness -> setText("10");
  m_widgets.pShift     -> setText("0.5");

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
  pGrid->addWidget(new QLabel("Offset:"),    0, 0);
  pGrid->addWidget(new QLabel("Thickness:"), 1, 0);
  pGrid->addWidget(new QLabel("Shift:"),     2, 0);
  //
  pGrid->addWidget(m_widgets.pOffset,    0, 1);
  pGrid->addWidget(m_widgets.pThickness, 1, 1);
  pGrid->addWidget(m_widgets.pShift,     2, 1);
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
  this->setWindowTitle("Build Offsets");
}

//! Destructor.
exeCC_DialogBuildOffsets::~exeCC_DialogBuildOffsets()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void exeCC_DialogBuildOffsets::onPerform()
{
  if ( m_plate.IsNull() )
  {
    std::cout << "Error: plate base is undefined" << std::endl;
  }

  // Read user inputs
  const double offset    = m_widgets.pOffset->text().toDouble();
  const double thickness = m_widgets.pThickness->text().toDouble();
  const double shift     = m_widgets.pShift->text().toDouble();

  // Get part Node
  Handle(asiData_PartNode)
    part_n = exeCC_CommonFacilities::Instance()->Model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    std::cout << "Error: part is not defined" << std::endl;
    return;
  }

  Handle(SpeCore_Journal) Journal = SpeCore_Journal::Instance();
  
  /* ===============
   *  Build offsets
   * =============== */

  TIMER_NEW
  TIMER_GO

  if ( !m_plate->DefineOffsets(offset, thickness, shift, Journal) )
    std::cout << "Cannot define offsets" << std::endl;

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Contour Capture")

  SpeCore_Surface PlateOffset;
  if ( !m_plate->GetOffsetSurface(SpeCore_Plate::Surface_Plate, PlateOffset, Journal) )
  {
    std::cout << "Cannot access to a plate offset" << std::endl;
    Journal->Dump(std::cout);
    return;
  }

  SpeCore_Surface LowerOffset;
  if ( !m_plate->GetOffsetSurface(SpeCore_Plate::Surface_Lower, LowerOffset, Journal) )
  {
    std::cout << "Cannot access to a lower offset" << std::endl;
    Journal->Dump(std::cout);
    return;
  }

  SpeCore_Surface UpperOffset;
  if ( !m_plate->GetOffsetSurface(SpeCore_Plate::Surface_Upper, UpperOffset, Journal) )
  {
    std::cout << "Cannot access to a upper offset" << std::endl;
    Journal->Dump(std::cout);
    return;
  }

  TopoDS_Compound comp;
  BRep_Builder().MakeCompound(comp);
  BRep_Builder().Add(comp, m_plate->GetPlateBase().Geometry);
  BRep_Builder().Add(comp, PlateOffset.Geometry);
  BRep_Builder().Add(comp, LowerOffset.Geometry);
  BRep_Builder().Add(comp, UpperOffset.Geometry);

  Journal->Dump(std::cout);

  /* ==========
   *  Finalize
   * ========== */

  exeCC_CommonFacilities::Instance()->Model->OpenCommand();
  {
    part_n->SetShape(comp);
  }
  exeCC_CommonFacilities::Instance()->Model->CommitCommand();

  // Replace part with the extracted piece of shell
  exeCC_CommonFacilities::Instance()->Prs.DeleteAll();
  exeCC_CommonFacilities::Instance()->Prs.Part->InitializePickers();
  exeCC_CommonFacilities::Instance()->Prs.Part->Actualize(part_n.get(), false, false);

  // Close
  this->close();
}
