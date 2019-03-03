//-----------------------------------------------------------------------------
// Created on: 25 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
#include <asiUI_ControlsPart.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_CommonFacilities.h>
#include <asiUI_DialogSTEP.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Builder.hxx>

// Qt include
#pragma warning(push, 0)
#include <QGroupBox>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_ControlsPart::asiUI_ControlsPart(const Handle(asiEngine_Model)& model,
                                       asiUI_ViewerPart*              pPartViewer,
                                       ActAPI_ProgressEntry           notifier,
                                       ActAPI_PlotterEntry            plotter,
                                       QWidget*                       parent)
//
: QScrollArea    (parent),
  m_model        (model),
  m_partViewer   (pPartViewer),
  m_notifier     (notifier),
  m_plotter      (plotter),
  m_iPrevSelMode (0)
{
  // Main layout.
  m_pMainLayout = new QVBoxLayout();

  // Buttons.
  m_widgets.Load.pSTEP       = new QPushButton("STEP");
  m_widgets.Load.pBRep       = new QPushButton("BREP");
  m_widgets.Load.pBRepAppend = new QPushButton("BREP (+)");
  //
  m_widgets.Save.pSTEP       = new QPushButton("STEP");
  m_widgets.Save.pBRep       = new QPushButton("BREP");
  //
  m_widgets.Select.pFaces    = new QPushButton("Faces");
  m_widgets.Select.pEdges    = new QPushButton("Edges");
  m_widgets.Select.pVertices = new QPushButton("Vertices");

  // Group box for loading CAD files.
  QGroupBox*   pLoadGroup = new QGroupBox("Load from");
  QVBoxLayout* pLoadLay   = new QVBoxLayout(pLoadGroup);
  //
  pLoadLay->addWidget(m_widgets.Load.pSTEP);
  pLoadLay->addWidget(m_widgets.Load.pBRep);
  pLoadLay->addWidget(m_widgets.Load.pBRepAppend);

  // Group box for saving CAD files.
  QGroupBox*   pSaveGroup = new QGroupBox("Save to");
  QVBoxLayout* pSaveLay   = new QVBoxLayout(pSaveGroup);
  //
  pSaveLay->addWidget(m_widgets.Save.pSTEP);
  pSaveLay->addWidget(m_widgets.Save.pBRep);

  // Group box for tuning interactive selection.
  QGroupBox*   pSelectGroup = new QGroupBox("Select");
  QVBoxLayout* pSelectLay   = new QVBoxLayout(pSelectGroup);
  //
  pSelectLay->addWidget(m_widgets.Select.pFaces);
  pSelectLay->addWidget(m_widgets.Select.pEdges);
  pSelectLay->addWidget(m_widgets.Select.pVertices);

  // Set main layout.
  m_pMainLayout->addWidget(pLoadGroup);
  m_pMainLayout->addWidget(pSaveGroup);
  m_pMainLayout->addWidget(pSelectGroup);
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
  connect( m_widgets.Load.pSTEP,       SIGNAL( clicked() ), SLOT( onLoadFromSTEP       () ) );
  connect( m_widgets.Load.pBRep,       SIGNAL( clicked() ), SLOT( onLoadFromBRep       () ) );
  connect( m_widgets.Load.pBRepAppend, SIGNAL( clicked() ), SLOT( onLoadFromBRepAppend () ) );
  //
  connect( m_widgets.Save.pSTEP,       SIGNAL( clicked() ), SLOT( onSaveToSTEP () ) );
  connect( m_widgets.Save.pBRep,       SIGNAL( clicked() ), SLOT( onSaveToBRep () ) );
  //
  connect( m_widgets.Select.pFaces,    SIGNAL( clicked() ), SLOT( onSelectFaces    () ) );
  connect( m_widgets.Select.pEdges,    SIGNAL( clicked() ), SLOT( onSelectEdges    () ) );
  connect( m_widgets.Select.pVertices, SIGNAL( clicked() ), SLOT( onSelectVertices () ) );
}

//-----------------------------------------------------------------------------

//! On BREP loading.
void asiUI_ControlsPart::onLoadFromBRep()
{
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Open);
  //
  if ( filename.isEmpty() )
    return;

  m_notifier.SetMessageKey("Load BREP");
  m_notifier.Init(2);

  // Read BREP
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot read BREP file.");
    return;
  }
  m_notifier.SendLogMessage( LogNotice(Normal) << "Part loaded from BREP file %1." << QStr2AsciiStr(filename) );
  m_notifier.StepProgress(1);
  m_notifier.SetMessageKey("Update accelerating structures");

  // Clean up the Model
  asiUI_CommonFacilities::ClearViewers(m_partViewer, NULL, NULL, true);
  //
  m_model->Clear();

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part(m_model).Update(shape, true);
  }
  m_model->CommitCommand(); // tx commit
  //
  m_notifier.StepProgress(1);
  m_notifier.SetProgressStatus(Progress_Succeeded);

  // Notify
  emit partLoaded(filename);
}

//-----------------------------------------------------------------------------

//! On BREP adding.
void asiUI_ControlsPart::onLoadFromBRepAppend()
{
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Open);
  //
  if ( filename.isEmpty() )
    return;

  m_notifier.SetMessageKey("Add BREP");
  m_notifier.Init(2);

  // Read BREP
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot read BREP file.");
    return;
  }
  m_notifier.SendLogMessage( LogNotice(Normal) << "Part added from BREP file %1" << QStr2AsciiStr(filename) );
  m_notifier.StepProgress(1);
  m_notifier.SetMessageKey("Update accelerating structures");

  // Get existing CAD part
  TopoDS_Shape partShape = asiEngine_Part(m_model).GetShape();

  // Build a compound of two shapes
  TopoDS_Compound comp;
  BRep_Builder().MakeCompound(comp);
  BRep_Builder().Add(comp, partShape);
  BRep_Builder().Add(comp, shape);

  // Update part
  m_model->OpenCommand(); // tx start
  {
    asiEngine_Part(m_model).Update(comp, true);
  }
  m_model->CommitCommand(); // tx commit
  //
  m_notifier.StepProgress(1);
  m_notifier.SetProgressStatus(Progress_Succeeded);

  // Notify
  emit partAdded(filename);
}

//-----------------------------------------------------------------------------

//! Loads model from STEP file.
void asiUI_ControlsPart::onLoadFromSTEP()
{
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();

  // Dialog for reading STEP
  asiUI_DialogSTEP* pDlg = new asiUI_DialogSTEP(m_model,
                                                geom_n,
                                                asiUI_DialogSTEP::Mode_Read,
                                                m_notifier,
                                                m_plotter,
                                                this);
  pDlg->exec();

  // Notify
  emit partLoaded(pDlg->Filename);
}

//-----------------------------------------------------------------------------

//! Saves model to STEP file.
void asiUI_ControlsPart::onSaveToSTEP()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Dialog for STEP save
  asiUI_DialogSTEP* pDlg = new asiUI_DialogSTEP(m_model,
                                                part_n,
                                                asiUI_DialogSTEP::Mode_Write,
                                                m_notifier,
                                                m_plotter,
                                                this);
  pDlg->show();

  // Notify
  emit partSaved();
}

//-----------------------------------------------------------------------------

//! Saves model to BREP file.
void asiUI_ControlsPart::onSaveToBRep()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
  //
  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);

  // Save shape
  if ( !asiAlgo_Utils::WriteBRep( part, QStr2AsciiStr(filename) ) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot save shape to BREP.");
    return;
  }

  // Notify
  emit partSaved();
}

////-----------------------------------------------------------------------------
//
////! Runs shape checker.
//void asiUI_ControlsPart::onCheckShape()
//{
//  Handle(asiData_PartNode) part_n;
//  TopoDS_Shape             part;
//  //
//  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
//
//  asiAlgo_CheckValidity checker(m_notifier, m_plotter);
//
//  // Get highlighted faces
//  TopTools_IndexedMapOfShape selected;
//  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);
//
//  if ( selected.IsEmpty() )
//  {
//    if ( !checker.CheckBasic(part) )
//    {
//      std::cout << "Error: shape is invalid" << std::endl;
//      return;
//    }
//    std::cout << "Shape is correct" << std::endl;
//  }
//  else
//  {
//    for ( int i = 1; i <= selected.Extent(); ++i )
//    {
//      if ( !checker.CheckBasic( selected(i) ) )
//      {
//        std::cout << "Error: sub-shape is invalid" << std::endl;
//        return;
//      }
//      std::cout << "Sub-shape is correct" << std::endl;
//    }
//  }
//}
//

//-----------------------------------------------------------------------------

//! Switches to selection by faces.
void asiUI_ControlsPart::onSelectFaces()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Notify
  emit selectionFacesOn();
}

//-----------------------------------------------------------------------------

//! Switches to selection by edges.
void asiUI_ControlsPart::onSelectEdges()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Notify
  emit selectionEdgesOn();
}

//-----------------------------------------------------------------------------

//! Switches to selection by vertices.
void asiUI_ControlsPart::onSelectVertices()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Notify
  emit selectionVerticesOn();
}
