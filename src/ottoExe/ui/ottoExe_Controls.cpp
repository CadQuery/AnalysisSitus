//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoExe_Controls.h>

// ottoExe includes
#include <ottoExe_CommonFacilities.h>
#include <ottoExe_Graph.h>

// asiUI includes
#include <asiUI_Common.h>

// Qt include
#include <QGroupBox>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
ottoExe_Controls::ottoExe_Controls(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pShowGraph = new QPushButton("Show dependency graph");

  // Group for tools
  QGroupBox*   pToolsGroup = new QGroupBox("Tools");
  QVBoxLayout* pToolsLay   = new QVBoxLayout(pToolsGroup);
  //
  pToolsLay->addWidget(m_widgets.pShowGraph);

  // Set layout
  m_pMainLayout->addWidget(pToolsGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pShowGraph, SIGNAL( clicked() ), SLOT( onShowGraph() ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
ottoExe_Controls::~ottoExe_Controls()
{
}

//-----------------------------------------------------------------------------

//! Shows dependency graph.
void ottoExe_Controls::onShowGraph()
{
  Handle(ottoExe_CommonFacilities) cf = ottoExe_CommonFacilities::Instance();

  // Render graph
  ottoExe_Graph* pGraphView = new ottoExe_Graph(cf->Model);
  //
  pGraphView->Render();
}

