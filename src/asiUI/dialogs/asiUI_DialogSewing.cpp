//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogSewing.h>

// Geometry includes
#include <asiAlgo_Utils.h>

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
asiUI_DialogSewing::asiUI_DialogSewing(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pTolerance = new asiUI_LineEdit();

  // Sizing
  m_widgets.pTolerance->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pTolerance->setText("0.00001");

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
  pGrid->addWidget(new QLabel("Tolerance:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pTolerance,  0, 1);
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
  this->setWindowTitle("Sewing parameters");
}

//! Destructor.
asiUI_DialogSewing::~asiUI_DialogSewing()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on perform.
void asiUI_DialogSewing::onPerform()
{
  // Convert user input to double
  const double toler = QVariant( m_widgets.pTolerance->text() ).toDouble();

  //---------------------------------------------------------------------------
  // Sewing
  //---------------------------------------------------------------------------

  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Working shape
  TopoDS_Shape part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Sew part
  common_facilities::Instance()->Model->OpenCommand();
  {
    // Perform sewing
    //
    std::cout << "Sewing tolerance = " << toler << std::endl;
    //
    if ( !asiAlgo_Utils::Sew(part, toler) )
    {
      std::cout << "Error: sewing failed" << std::endl;
      this->close();
    }
    //
    std::cout << "Sewing done. Visualizing..." << std::endl;
    //
    N->SetShape(part);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Update viewer
  common_facilities::Instance()->Model->Clear();
  //
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->InitializePickers();
  common_facilities::Instance()->Prs.Part->Actualize( N.get() );

  // Close
  this->close();
}
