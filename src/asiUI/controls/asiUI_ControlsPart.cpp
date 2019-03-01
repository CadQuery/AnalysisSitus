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
#include <asiUI_DialogGapFilling.h>
#include <asiUI_DialogSewing.h>
#include <asiUI_DialogSTEP.h>
#include <asiUI_PickCallback.h>
#include <asiUI_PickEdgeCallback.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
#include <asiAlgo_CheckValidity.h>
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PLY.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// VTK includes
#pragma warning(push, 0)
#include <vtkActor.h>
#include <vtkProperty.h>
#pragma warning(pop)

// Qt include
#pragma warning(push, 0)
#include <QGroupBox>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

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
  ////
  //m_widgets.pCheckShape     = new QPushButton("Check shape");
  //m_widgets.pTolerance      = new QPushButton("Tolerance");
  ////
  //m_widgets.pSewing         = new QPushButton("Sewing");
  //m_widgets.pMaximizeFaces  = new QPushButton("Maximize faces");
  //m_widgets.pFillGap        = new QPushButton("Fill gap");
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

  //// Group for analysis
  //QGroupBox*   pAnalysisGroup = new QGroupBox("Analysis");
  //QVBoxLayout* pAnalysisLay   = new QVBoxLayout(pAnalysisGroup);
  ////
  //pAnalysisLay->addWidget(m_widgets.pCheckShape);
  //pAnalysisLay->addWidget(m_widgets.pTolerance);

  //// Group for processing
  //QGroupBox*   pProcessingGroup = new QGroupBox("Processing");
  //QVBoxLayout* pProcessingLay   = new QVBoxLayout(pProcessingGroup);
  ////
  //pProcessingLay->addWidget(m_widgets.pSewing);
  //pProcessingLay->addWidget(m_widgets.pMaximizeFaces);
  //pProcessingLay->addWidget(m_widgets.pFillGap);

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
  //connect( m_widgets.pCheckShape,     SIGNAL( clicked() ), SLOT( onCheckShape     () ) );
  //connect( m_widgets.pTolerance,      SIGNAL( clicked() ), SLOT( onTolerance      () ) );
  ////
  //connect( m_widgets.pSewing,         SIGNAL( clicked() ), SLOT( onSewing         () ) );
  //connect( m_widgets.pMaximizeFaces,  SIGNAL( clicked() ), SLOT( onMaximizeFaces  () ) );
  //connect( m_widgets.pFillGap,        SIGNAL( clicked() ), SLOT( onFillGap        () ) );
  //
  connect( m_widgets.Select.pFaces,    SIGNAL( clicked() ), SLOT( onSelectFaces    () ) );
  connect( m_widgets.Select.pEdges,    SIGNAL( clicked() ), SLOT( onSelectEdges    () ) );
  connect( m_widgets.Select.pVertices, SIGNAL( clicked() ), SLOT( onSelectVertices () ) );
}

//-----------------------------------------------------------------------------

asiUI_ControlsPart::~asiUI_ControlsPart()
{}

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

////-----------------------------------------------------------------------------
//
////! Saves mesh to PLY file.
//void asiUI_ControlsPart::onSavePly()
//{
//  Handle(asiData_PartNode) part_n;
//  TopoDS_Shape             part;
//  //
//  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
//  //
//  QString filename = asiUI_Common::selectPLYFile(asiUI_Common::OpenSaveAction_Save);
//
//  // Convert shape's inherent mesh to a storable mesh
//  Handle(ActData_Mesh) storedMesh;
//  if ( !asiAlgo_MeshConvert::ToPersistent(part, storedMesh) )
//  {
//    std::cout << "Error: cannot convert mesh to persistent form" << std::endl;
//    return;
//  }
//
//  // Save mesh to ply files
//  if ( !asiAlgo_PLY::Write( storedMesh, QStr2AsciiStr(filename) ) )
//  {
//    std::cout << "Error: cannot save mesh" << std::endl;
//    return;
//  }
//
//  // Notify
//  emit partSaved();
//}

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
    std::cout << "Error: cannot save shape" << std::endl;
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
////-----------------------------------------------------------------------------
//
////! Consults tolerance.
//void asiUI_ControlsPart::onTolerance()
//{
//  Handle(asiData_PartNode) part_n;
//  TopoDS_Shape             part;
//  //
//  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
//
//  // Get highlighted faces
//  TopTools_IndexedMapOfShape selected;
//  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedSubShapes(selected);
//
//  if ( selected.IsEmpty() )
//  {
//    const double maxTol = asiAlgo_CheckValidity::MaxTolerance(part);
//    m_notifier.SendLogMessage(LogInfo(Normal) << "Max tolerance: %1" << maxTol);
//  }
//  else
//  {
//    double maxTol = 0.0;
//    for ( int i = 1; i <= selected.Extent(); ++i )
//    {
//      const double tol = asiAlgo_CheckValidity::MaxTolerance( selected(i) );
//      maxTol = std::max(tol, maxTol);
//    }
//    m_notifier.SendLogMessage(LogInfo(Normal) << "Max tolerance for selected sub-shape: %1" << maxTol);
//  }
//}
//
////-----------------------------------------------------------------------------
//
////! Runs sewing.
//void asiUI_ControlsPart::onSewing()
//{
//  Handle(asiData_PartNode) part_n;
//  TopoDS_Shape             part;
//  //
//  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
//
//  // Run dialog for sewing properties
//  asiUI_DialogSewing* wSewing = new asiUI_DialogSewing(m_model, this);
//  wSewing->exec();
//
//  // Notify
//  emit partModified();
//}
//
////-----------------------------------------------------------------------------
//
////! Runs face maximization.
//void asiUI_ControlsPart::onMaximizeFaces()
//{
//  Handle(asiData_PartNode) part_n;
//  TopoDS_Shape             part;
//  //
//  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
//
//  m_model->OpenCommand();
//  {
//    TIMER_NEW
//    TIMER_GO
//
//    // Perform merge
//    if ( !asiAlgo_Utils::MaximizeFaces(part) )
//    {
//      std::cout << "Error: face merging failed" << std::endl;
//      m_model->AbortCommand();
//      return;
//    }
//
//    TIMER_FINISH
//    TIMER_COUT_RESULT_MSG("Face Maximization")
//
//    //
//    std::cout << "Face merging done. Visualizing..." << std::endl;
//    //
//    asiEngine_Part(m_model).Update(part);
//  }
//  m_model->CommitCommand();
//
//  // Notify
//  emit partModified();
//}
//
////-----------------------------------------------------------------------------
//
////! Performs gap filling.
//void asiUI_ControlsPart::onFillGap()
//{
//  Handle(asiData_PartNode) part_n;
//  TopoDS_Shape             part;
//  //
//  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
//
//  /* ======================================================
//   *  Gather free edges which should form a closed contour
//   * ====================================================== */
//
//  TColStd_PackedMapOfInteger edgeIndices;
//  asiEngine_Part( m_model, m_partViewer->PrsMgr() ).GetHighlightedEdges(edgeIndices);
//  //
//  if ( !edgeIndices.Extent() )
//  {
//    m_notifier.SendLogMessage(LogErr(Normal) << "No seed edges selected");
//    return;
//  }
//
//  /* =====================
//   *  Perform gap filling
//   * ===================== */
//
//  // Dialog for reading STEP
//  asiUI_DialogGapFilling*
//    pDlg = new asiUI_DialogGapFilling(m_model,
//                                      m_partViewer->PrsMgr(),
//                                      m_partViewer,
//                                      m_notifier,
//                                      m_plotter,
//                                      this);
//  pDlg->exec();
//
//  // Notify
//  emit partModified();
//}

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
