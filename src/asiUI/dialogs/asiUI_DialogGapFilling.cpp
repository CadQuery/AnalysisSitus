//-----------------------------------------------------------------------------
// Created on: 10 March 2017
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiUI_DialogGapFilling.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_PlateOnEdges.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// OCCT includes
#include <BRep_Builder.hxx>

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
//! \param model    [in] Data Model instance.
//! \param prsMgr   [in] presentation manager.
//! \param pViewer  [in] connected viewer.
//! \param progress [in] progress notifier.
//! \param plotter  [in] imperative plotter.
//! \param parent   [in] parent widget.
asiUI_DialogGapFilling::asiUI_DialogGapFilling(const Handle(asiEngine_Model)&             model,
                                               const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                               asiUI_ViewerPart*                          pViewer,
                                               ActAPI_ProgressEntry                       progress,
                                               ActAPI_PlotterEntry                        plotter,
                                               QWidget*                                   parent)
//
: QDialog    (parent),
  m_model    (model),
  m_prsMgr   (prsMgr),
  m_pViewer  (pViewer),
  m_notifier (progress),
  m_plotter  (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Filling parameters");

  // Editors
  m_widgets.pOrder = new QComboBox();
  //
  m_widgets.pOrder->addItem("0", 0);
  m_widgets.pOrder->addItem("1", 1);
  m_widgets.pOrder->addItem("2", 2);
  
  // Sizing
  m_widgets.pOrder->setMinimumWidth(CONTROL_EDIT_WIDTH);

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
  pGrid->addWidget(new QLabel("Order of continuity:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pOrder, 0, 1);
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
  this->setWindowTitle("Gap filling");
}

//! Destructor.
asiUI_DialogGapFilling::~asiUI_DialogGapFilling()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void asiUI_DialogGapFilling::onPerform()
{
  const int continuity = m_widgets.pOrder->currentIndex();
  m_notifier.SendLogMessage(LogInfo(Normal) << "Smoothness C%1 requested" << continuity);
  //
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  TColStd_PackedMapOfInteger edgeIndices;
  asiEngine_Part(m_model, m_prsMgr).GetHighlightedEdges(edgeIndices);
  //
  if ( !edgeIndices.Extent() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "No seed edges selected");
    return;
  }

  /* =====================
   *  Perform gap filling
   * ===================== */

  Handle(Geom_BSplineSurface) supportSurf;
  TopoDS_Face patchFace;
  BRep_Builder BB;

  // Perform gap filling
  asiAlgo_PlateOnEdges PlateOnEdges(m_model->GetPartNode()->GetAAG(), m_notifier, m_plotter);
  //
  if ( !PlateOnEdges.Build(edgeIndices, continuity, supportSurf, patchFace) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Gap filling failed");
    return;
  }

  // Build a compound of the original geometry and a patch
  TopoDS_Compound comp;
  BB.MakeCompound(comp);
  BB.Add(comp, part);
  BB.Add(comp, patchFace);

  // Update Data Model
  m_model->OpenCommand();
  {
    // Set new geometry
    asiEngine_Part(m_model, m_prsMgr).Update(comp);
  }
  m_model->CommitCommand();

  // Close
  this->close();
}
