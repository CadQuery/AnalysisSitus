//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogEuler.h>

// GUI includes
#include <asiUI_Common.h>

// Geometry includes
#include <asiAlgo_EulerPoincare.h>

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
asiUI_DialogEuler::asiUI_DialogEuler(const Handle(asiEngine_Model)& model,
                                     QWidget*                       parent)
//
: QDialog(parent), m_model(model)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Give me a hint on genus");

  // Editors
  m_widgets.pGenus = new asiUI_LineEdit();

  // Sizing
  m_widgets.pGenus->setMinimumWidth(CONTROL_EDIT_WIDTH);
  //
  m_widgets.pGenus->setText( QString::number(0) );

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pCheck = new QPushButton("Check");

  // Sizing
  m_widgets.pCheck->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pCheck, SIGNAL( clicked() ), this, SLOT( onCheck() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Genus:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pGenus,  0, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pCheck);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Euler-Poincare formula");
}

//! Destructor.
asiUI_DialogEuler::~asiUI_DialogEuler()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on check.
void asiUI_DialogEuler::onCheck()
{
  // Convert user input to integer
  const int genus = QVariant( m_widgets.pGenus->text() ).toInt();

  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // TODO
  if ( !asiAlgo_EulerPoincare::Check(part, genus) )
    std::cout << "Euler-Poincare is FALSE" << std::endl;
  else
    std::cout << "Euler-Poincare is OK" << std::endl;

  // Close
  this->close();
}
