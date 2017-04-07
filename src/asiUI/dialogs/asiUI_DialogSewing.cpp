//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogSewing.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
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
//! \param model  [in] Data Model instance.
//! \param parent [in] parent widget.
asiUI_DialogSewing::asiUI_DialogSewing(const Handle(asiEngine_Model)& model,
                                       QWidget*                       parent)
//
: QDialog(parent), m_model(model)
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

  if ( m_model.IsNull() )
    return;

  // Working shape
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  TopoDS_Shape             part   = part_n->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return;
  }

  // Sew part
  m_model->OpenCommand();
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
    asiEngine_Part(m_model).Update(part);
  }
  m_model->CommitCommand();

  // Close
  this->close();
}
