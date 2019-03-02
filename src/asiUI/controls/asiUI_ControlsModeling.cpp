//-----------------------------------------------------------------------------
// Created on: 31 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiUI_ControlsModeling.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_DialogGapFilling.h>
#include <asiUI_DialogSewing.h>

// asiAlgo includes
#include <asiAlgo_DetachFaces.h>
#include <asiAlgo_SuppressFaces.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// Qt include
#pragma warning(push, 0)
#include <QGroupBox>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_ControlsModeling::asiUI_ControlsModeling(const Handle(asiEngine_Model)& model,
                                               asiUI_ViewerPart*              pPartViewer,
                                               ActAPI_ProgressEntry           notifier,
                                               ActAPI_PlotterEntry            plotter,
                                               QWidget*                       parent)
: QScrollArea  (parent),
  m_model      (model),
  m_partViewer (pPartViewer),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout.
  m_pMainLayout = new QVBoxLayout;

  // Buttons.
  m_widgets.Edit.pDeleteFaces    = new QPushButton("Delete faces");
  m_widgets.Edit.pSuppressFaces  = new QPushButton("Suppress faces");
  m_widgets.Edit.pDefeatureFaces = new QPushButton("Defeature faces");
  m_widgets.Edit.pDetachFaces    = new QPushButton("Detach faces");
  //
  m_widgets.Healing.pSew            = new QPushButton("Sew");
  m_widgets.Healing.pAutoRepair     = new QPushButton("Auto-repair");
  m_widgets.Healing.pMaximizeFaces  = new QPushButton("Maximize faces");
  m_widgets.Healing.pHealSmallEdges = new QPushButton("Merge small edges");
  m_widgets.Healing.pFillGap        = new QPushButton("Fill gap");

  // Group for editing operators.
  QGroupBox*   pEditGroup = new QGroupBox("Edit");
  QVBoxLayout* pEditLay   = new QVBoxLayout(pEditGroup);
  //
  pEditLay->addWidget(m_widgets.Edit.pDeleteFaces);
  pEditLay->addWidget(m_widgets.Edit.pSuppressFaces);
  pEditLay->addWidget(m_widgets.Edit.pDefeatureFaces);
  pEditLay->addWidget(m_widgets.Edit.pDetachFaces);

  // Group for healing operators.
  QGroupBox*   pHealingGroup = new QGroupBox("Healing");
  QVBoxLayout* pHealingLay   = new QVBoxLayout(pHealingGroup);
  //
  pHealingLay->addWidget(m_widgets.Healing.pSew);
  pHealingLay->addWidget(m_widgets.Healing.pAutoRepair);
  pHealingLay->addWidget(m_widgets.Healing.pMaximizeFaces);
  pHealingLay->addWidget(m_widgets.Healing.pHealSmallEdges);
  pHealingLay->addWidget(m_widgets.Healing.pFillGap);

  // Set layout
  m_pMainLayout->addWidget(pEditGroup);
  m_pMainLayout->addWidget(pHealingGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  QWidget* pMainWidget = new QWidget(this);
  pMainWidget->setLayout(m_pMainLayout);
  //
  this->setWidgetResizable(true);
  this->setFrameStyle(QFrame::NoFrame);
  this->setWidget(pMainWidget);

  // Connect signals to slots.
  connect( m_widgets.Edit.pDeleteFaces,    SIGNAL( clicked() ), SLOT( onDeleteFaces    () ) );
  connect( m_widgets.Edit.pSuppressFaces,  SIGNAL( clicked() ), SLOT( onSuppressFaces  () ) );
  connect( m_widgets.Edit.pDefeatureFaces, SIGNAL( clicked() ), SLOT( onDefeatureFaces () ) );
  connect( m_widgets.Edit.pDetachFaces,    SIGNAL( clicked() ), SLOT( onDetachFaces    () ) );
  //
  connect( m_widgets.Healing.pSew,            SIGNAL( clicked() ), SLOT( onSew            () ) );
  connect( m_widgets.Healing.pAutoRepair,     SIGNAL( clicked() ), SLOT( onAutoRepair     () ) );
  connect( m_widgets.Healing.pMaximizeFaces,  SIGNAL( clicked() ), SLOT( onMaximizeFaces  () ) );
  connect( m_widgets.Healing.pHealSmallEdges, SIGNAL( clicked() ), SLOT( onHealSmallEdges () ) );
  connect( m_widgets.Healing.pFillGap,        SIGNAL( clicked() ), SLOT( onFillGap        () ) );
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onDeleteFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces.
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Delete selected faces.
  asiAlgo_SuppressFaces eraser(part);
  //
  if ( !eraser.Perform(selected, true) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot delete faces.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Faces deletion");

  const TopoDS_Shape& result = eraser.GetResult();

  // Update part.
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onSuppressFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces.
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  TIMER_NEW
  TIMER_GO

  // Suppress selected faces.
  asiAlgo_SuppressFaces eraser(part);
  //
  if ( !eraser.Perform(selected, false) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot suppress faces.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "FULL faces deletion");

  const TopoDS_Shape& result = eraser.GetResult();

  // Update part.
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onDefeatureFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces.
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedFaces(selected);

  // Repack faces.
  TopTools_ListOfShape faces2Kill;
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selected); fit.More(); fit.Next() )
  {
    const TopoDS_Shape& faceSh = part_n->GetAAG()->GetMapOfFaces()( fit.Key() );
    faces2Kill.Append(faceSh);
  }

  TIMER_NEW
  TIMER_GO

  // Perform Boolean operation for face removal.
  TopoDS_Shape result;
  //
  if ( !asiAlgo_Utils::BooleanRemoveFaces( part,
                                           faces2Kill,
                                           false, // Parallel mode.
                                           true, // Whether to track history.
                                           result,
                                           m_notifier ) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Smart face removal failed.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Smart faces deletion");

  // Update part.
  m_model->OpenCommand(); // tx start.
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit.

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onDetachFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get highlighted faces.
  TopTools_IndexedMapOfShape selected;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

  TIMER_NEW
  TIMER_GO

  // Deatch selected faces.
  asiAlgo_DetachFaces detacher(part);
  //
  if ( !detacher.Perform(selected) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot detach faces.");
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Detach faces")

  const TopoDS_Shape& result = detacher.Result();

  // Update part.
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part( m_model, m_partViewer->PrsMgr() ).Update(result);
  }
  m_model->CommitCommand(); // tx commit

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onSew()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Run dialog for sewing properties
  asiUI_DialogSewing* wSewing = new asiUI_DialogSewing(m_model, m_notifier, this);
  wSewing->exec();

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onAutoRepair()
{
  m_notifier.SendLogMessage(LogErr(Normal) << "NYI");
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onMaximizeFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  m_model->OpenCommand();
  {
    TIMER_NEW
    TIMER_GO

    // Perform merge.
    if ( !asiAlgo_Utils::MaximizeFaces(part) )
    {
      m_notifier.SendLogMessage(LogErr(Normal) << "Face maximization failed.");
      m_model->AbortCommand();
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Face maximization")

    asiEngine_Part(m_model).Update(part);
  }
  m_model->CommitCommand();

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onHealSmallEdges()
{
  m_notifier.SendLogMessage(LogErr(Normal) << "NYI");
}

//-----------------------------------------------------------------------------

void asiUI_ControlsModeling::onFillGap()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  /* ======================================================
   *  Gather free edges which should form a closed contour
   * ====================================================== */

  TColStd_PackedMapOfInteger edgeIndices;
  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedEdges(edgeIndices);
  //
  if ( !edgeIndices.Extent() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "No seed edges selected.");
    return;
  }

  /* =====================
   *  Perform gap filling
   * ===================== */

  // Dialog for gap filling.
  asiUI_DialogGapFilling*
    pDlg = new asiUI_DialogGapFilling(m_model,
                                      m_partViewer->PrsMgr(),
                                      m_partViewer,
                                      m_notifier,
                                      m_plotter,
                                      this);
  pDlg->exec();

  // Update viewer.
  m_partViewer->PrsMgr()->Actualize(part_n);
}
