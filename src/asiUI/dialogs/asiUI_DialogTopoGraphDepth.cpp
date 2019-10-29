//-----------------------------------------------------------------------------
// Created on: 11 December(*) 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogTopoGraphDepth.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_PartGraph.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Qt includes
#pragma warning(push, 0)
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param model      [in] Data Model instance.
//! \param partShape  [in] part as topological shape.
//! \param partViewer [in] part viewer.
//! \param progress   [in] progress notifier.
//! \param parent     [in] parent widget.
asiUI_DialogTopoGraphDepth::asiUI_DialogTopoGraphDepth(const Handle(asiEngine_Model)& model,
                                                       const TopoDS_Shape&            globalShape,
                                                       const TopoDS_Shape&            localShape,
                                                       asiUI_ViewerPart*              partViewer,
                                                       ActAPI_ProgressEntry           progress,
                                                       QWidget*                       parent)
//
: QDialog        (parent),
  m_model        (model),
  m_globalShape  (globalShape),
  m_localShape   (localShape),
  m_bColorizeLoc (false),
  m_pPartViewer  (partViewer),
  m_notifier     (progress)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Depth");

  // Graph traversal depth
  m_widgets.pDepth = new QComboBox();
  //
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_COMPOUND).c_str(),  TopAbs_COMPOUND);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_COMPSOLID).c_str(), TopAbs_COMPSOLID);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_SOLID).c_str(),     TopAbs_SOLID);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_SHELL).c_str(),     TopAbs_SHELL);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_FACE).c_str(),      TopAbs_FACE);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_WIRE).c_str(),      TopAbs_WIRE);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_EDGE).c_str(),      TopAbs_EDGE);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_VERTEX).c_str(),    TopAbs_VERTEX);
  m_widgets.pDepth->addItem(asiAlgo_Utils::ShapeTypeStr(TopAbs_SHAPE).c_str(),     TopAbs_SHAPE);
  //
  m_widgets.pDepth->setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pDepth->setCurrentIndex(TopAbs_SHAPE);

  // Whether to colorize graph according to locations of sub-shapes
  m_widgets.pColorizeLocations = new QCheckBox;

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pOpen = new QPushButton("Open");

  // Sizing
  m_widgets.pOpen->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reactions
  connect( m_widgets.pColorizeLocations, SIGNAL( clicked() ), this, SLOT( onColorizeLocations () ) );
  connect( m_widgets.pOpen,              SIGNAL( clicked() ), this, SLOT( onOpen              () ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  //
  pGrid->addWidget(new QLabel("Depth:"),              0, 0);
  pGrid->addWidget(new QLabel("Colorize locations:"), 1, 0);
  pGrid->addWidget(m_widgets.pDepth,                  0, 1);
  pGrid->addWidget(m_widgets.pColorizeLocations,      1, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pOpen);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Show topology graph");
}

//! Destructor.
asiUI_DialogTopoGraphDepth::~asiUI_DialogTopoGraphDepth()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Open" button.
void asiUI_DialogTopoGraphDepth::onOpen()
{
  const TopAbs_ShapeEnum shapeType = (TopAbs_ShapeEnum) m_widgets.pDepth->currentIndex();
  m_notifier.SendLogMessage( LogInfo(Normal) << "Depth requested: %1."
                                             << asiAlgo_Utils::ShapeTypeStr(shapeType).c_str() );

  // Get naming service
  Handle(asiAlgo_Naming)
    naming = m_model->GetPartNode()->GetNaming();

  // Show graph
  asiUI_PartGraph* pGraphView = new asiUI_PartGraph(m_model, m_pPartViewer);
  pGraphView->SetNaming(naming);
  pGraphView->RenderTopology(m_globalShape, m_localShape, shapeType, m_bColorizeLoc);

  // Close
  this->close();
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "colorize locations" checkbox.
void asiUI_DialogTopoGraphDepth::onColorizeLocations()
{
  m_bColorizeLoc = m_widgets.pColorizeLocations->isChecked();
}
