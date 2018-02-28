//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
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
#include <asiUI_ViewerPartListener.h>

// asiUI includes
#include <asiUI_BgColorDialog.h>
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_InvertFaces.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_PartNodeInfo.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// VTK includes
#include <vtkCamera.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param[in] wViewerPart   part viewer.
//! \param[in] wViewerDomain domain viewer.
//! \param[in] wViewerHost   host viewer.
//! \param[in] model         Data Model instance.
//! \param[in] progress      progress notifier.
//! \param[in] plotter       imperative plotter.
asiUI_ViewerPartListener::asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                                                   asiUI_ViewerDomain*            wViewerDomain,
                                                   asiUI_ViewerHost*              wViewerHost,
                                                   const Handle(asiEngine_Model)& model,
                                                   ActAPI_ProgressEntry           progress,
                                                   ActAPI_PlotterEntry            plotter)
//
: asiUI_Viewer3dListener (wViewerPart, model, progress, plotter),
  m_wViewerDomain        (wViewerDomain),
  m_wViewerHost          (wViewerHost),
  m_pSaveBREPAction      (NULL),
  m_pShowNormsAction     (NULL),
  m_pInvertFacesAction   (NULL),
  m_pShowOriContour      (NULL)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerPartListener::~asiUI_ViewerPartListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_ViewerPartListener::Connect()
{
  asiUI_Viewer3dListener::Connect(); // Connect basic reactions.

  connect( m_pViewer, SIGNAL ( facePicked(const asiVisu_PickResult&) ),
           this,      SLOT   ( onFacePicked(const asiVisu_PickResult&) ) );
  //
  connect( m_pViewer, SIGNAL ( edgePicked(const asiVisu_PickResult&) ),
           this,      SLOT   ( onEdgePicked(const asiVisu_PickResult&) ) );
  //
  connect( m_pViewer, SIGNAL ( vertexPicked(const asiVisu_PickResult&) ),
           this,      SLOT   ( onVertexPicked(const asiVisu_PickResult&) ) );
}

//-----------------------------------------------------------------------------

//! Reaction on face picking.
//! \param[in] pickRes pick result.
void asiUI_ViewerPartListener::onFacePicked(const asiVisu_PickResult& pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes.GetPickedActor() );
  //
  if ( pickRes.GetPickedActor() && !nodeInfo )
    return;

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->Actualize(geom_n->GetFaceRepresentation().get(), false, true);
  //
  if ( m_wViewerHost )
    m_wViewerHost->PrsMgr()->Actualize(geom_n->GetSurfaceRepresentation().get(), false, true);

  /* =============================
   *  Dump textual info to logger
   * ============================= */

  // Get index of the active sub-shape.
  const int
    globalId = geom_n->GetFaceRepresentation()->GetSelectedFace();
  //
  if ( globalId == 0 )
    return;

  // Get sub-shapes map.
  const TopTools_IndexedMapOfShape&
    allSubShapes = geom_n->GetAAG()->GetMapOfSubShapes();
  //
  if ( globalId < 1 || globalId > allSubShapes.Extent() )
    return;

  // Get sub-shape.
  const TopoDS_Shape& subShape = allSubShapes(globalId);

  // Send message to logger.
  TCollection_AsciiString
    msg = asiAlgo_Utils::NamedShapeToString( subShape,
                                             globalId,
                                             geom_n->GetNaming() );
  //
  m_progress.SendLogMessage( LogInfo(Normal) << msg.ToCString() );
}

//-----------------------------------------------------------------------------

//! Reaction on edge picking.
//! \param[in] pickRes pick result.
void asiUI_ViewerPartListener::onEdgePicked(const asiVisu_PickResult& pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes.GetPickedActor() );
  //
  if ( pickRes.GetPickedActor() && !nodeInfo )
    return;

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->Actualize(geom_n->GetEdgeRepresentation().get(), false, true);
  //
  if ( m_wViewerHost )
    m_wViewerHost->PrsMgr()->Actualize(geom_n->GetCurveRepresentation().get(), false, true);

  /* =============================
   *  Dump textual info to logger
   * ============================= */

  // Get index of the active sub-shape.
  const int
    globalId = geom_n->GetEdgeRepresentation()->GetSelectedEdge();
  //
  if ( globalId == 0 )
    return;

  // Get sub-shapes map.
  const TopTools_IndexedMapOfShape&
    allSubShapes = geom_n->GetAAG()->GetMapOfSubShapes();
  //
  if ( globalId < 1 || globalId > allSubShapes.Extent() )
    return;

  // Get sub-shape.
  const TopoDS_Shape& subShape = allSubShapes(globalId);

  // Send message to logger.
  TCollection_AsciiString
    msg = asiAlgo_Utils::NamedShapeToString( subShape,
                                             globalId,
                                             geom_n->GetNaming() );
  //
  m_progress.SendLogMessage( LogInfo(Normal) << msg.ToCString() );
}

//-----------------------------------------------------------------------------

//! Reaction on vertex picking.
//! \param[in] pickRes pick result.
void asiUI_ViewerPartListener::onVertexPicked(const asiVisu_PickResult& pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes.GetPickedActor() );
  //
  if ( pickRes.GetPickedActor() && !nodeInfo )
    return;

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();

  /* =============================
   *  Dump textual info to logger
   * ============================= */

  // Get index of the active sub-shape.
  const int
    globalId = geom_n->GetVertexRepresentation()->GetSelectedVertex();
  //
  if ( globalId == 0 )
    return;

  // Get sub-shapes map.
  const TopTools_IndexedMapOfShape&
    allSubShapes = geom_n->GetAAG()->GetMapOfSubShapes();
  //
  if ( globalId < 1 || globalId > allSubShapes.Extent() )
    return;

  // Get sub-shape.
  const TopoDS_Shape& subShape = allSubShapes(globalId);

  // Send message to logger.
  TCollection_AsciiString
    msg = asiAlgo_Utils::NamedShapeToString( subShape,
                                             globalId,
                                             geom_n->GetNaming() );
  //
  m_progress.SendLogMessage( LogInfo(Normal) << msg.ToCString() );
}

//-----------------------------------------------------------------------------

//! Populates the passed Qt menu with actions specific to Part viewer.
//! \param[in] menu Qt menu to populate.
void asiUI_ViewerPartListener::populateMenu(QMenu& menu)
{
  // Get highlighted faces
  TColStd_PackedMapOfInteger faceIndices;
  asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedFaces(faceIndices);

  // Get Part Node
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Part Node is null or bad-formed" );
    return;
  }

  // Prepare the context menu items
  if ( faceIndices.Extent() )
  {
    menu.addSeparator();
    //
    m_pSaveBREPAction = menu.addAction("Save to BREP...");
    //
    if ( m_pViewer->PrsMgr()->IsPresentable( STANDARD_TYPE(asiData_FaceNormsNode) ) )
      m_pShowNormsAction = menu.addAction("Show face normals");
    if ( m_pViewer->PrsMgr()->IsPresentable( STANDARD_TYPE(asiData_FaceContourNode) ) )
      m_pShowOriContour = menu.addAction("Show face oriented contour");
    //
    m_pInvertFacesAction = menu.addAction("Invert faces");
  }
}

//-----------------------------------------------------------------------------

//! Executes the passed Qt action.
//! \param[in] pAction Qt action to execute.
void asiUI_ViewerPartListener::executeAction(QAction* pAction)
{
  if ( !pAction )
    return;

  //---------------------------------------------------------------------------
  // ACTION: save BREP
  //---------------------------------------------------------------------------
  if ( pAction == m_pSaveBREPAction )
  {
    // Get highlighted sub-shapes
    TopTools_IndexedMapOfShape selected;
    asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

    // Let user choose a filename
    QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);
    //
    if ( filename.isEmpty() )
      return;

    // Prepare a shape to dump
    TopoDS_Shape shape2Save;
    //
    if ( selected.Extent() == 1 )
      shape2Save = selected(1);
    else
    {
      // Put selected shapes to a compound
      TopoDS_Compound comp;
      BRep_Builder().MakeCompound(comp);
      //
      for ( int k = 1; k <= selected.Extent(); ++k )
        BRep_Builder().Add( comp, selected(k) );
      //
      shape2Save = comp;
    }

    // Save shape
    if ( !asiAlgo_Utils::WriteBRep( shape2Save, QStr2AsciiStr(filename) ) )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Cannot save shape" );
      return;
    }
  }

  //---------------------------------------------------------------------------
  // ACTION: show normal field
  //---------------------------------------------------------------------------
  else if ( pAction == m_pShowNormsAction )
  {
    TIMER_NEW
    TIMER_GO

    m_pViewer->PrsMgr()->Actualize( m_model->GetPartNode()->GetNormsRepresentation() );

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Visualization of normals")
  }

  //---------------------------------------------------------------------------
  // ACTION: show oriented contour
  //---------------------------------------------------------------------------
  else if ( pAction == m_pShowOriContour )
  {
    TIMER_NEW
    TIMER_GO

    m_pViewer->PrsMgr()->Actualize( m_model->GetPartNode()->GetContourRepresentation() );

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Visualization of oriented contour")
  }

  //---------------------------------------------------------------------------
  // ACTION: invert faces
  //---------------------------------------------------------------------------
  else if ( pAction == m_pInvertFacesAction )
  {
    // Get highlighted faces
    TColStd_PackedMapOfInteger faceIndices;
    asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedFaces(faceIndices);

    // Get Part Node
    Handle(asiData_PartNode) part_n = m_model->GetPartNode();
    //
    if ( part_n.IsNull() || !part_n->IsWellFormed() )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Part Node is null or bad-formed" );
      return;
    }

    TIMER_NEW
    TIMER_GO

    asiAlgo_InvertFaces InvertFaces( part_n->GetAAG() );
    //
    if ( !InvertFaces.Perform(faceIndices) )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Face inversion failed" );
      return;
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Invert faces")

    // Update Data Model
    m_model->OpenCommand();
    {
      asiEngine_Part(m_model).Update( InvertFaces.GetResult() );
    }
    m_model->CommitCommand();

    // Actualize
    m_pViewer->PrsMgr()->Actualize(part_n);
    m_pViewer->PrsMgr()->Actualize( m_model->GetPartNode()->GetNormsRepresentation() );
  }
}
