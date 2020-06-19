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
#include <asiUI_ViewerPartListener.h>

// asiUI includes
#include <asiUI_BgColorDialog.h>
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_CheckDihedralAngle.h>
#include <asiAlgo_InvertFaces.h>
#include <asiAlgo_JSON.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Features.h>
#include <asiEngine_IV.h>
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_PartNodeInfo.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// VTK includes
#pragma warning(push, 0)
#include <vtkCamera.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QClipboard>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_ViewerPartListener::asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                                                   asiUI_ViewerDomain*            wViewerDomain,
                                                   asiUI_ViewerHost*              wViewerHost,
                                                   const Handle(asiEngine_Model)& model,
                                                   ActAPI_ProgressEntry           progress,
                                                   ActAPI_PlotterEntry            plotter)
//
: asiUI_Viewer3dListener  (wViewerPart, model, progress, plotter),
  m_wViewerDomain         (wViewerDomain),
  m_wViewerHost           (wViewerHost),
  m_pSaveBREPAction       (nullptr),
  m_pShowNormsAction      (nullptr),
  m_pInvertFacesAction    (nullptr),
  m_pShowOriContourAction (nullptr),
  m_pCopyAsStringAction   (nullptr),
  m_pSetAsVariableAction  (nullptr),
  m_pFindIsolated         (nullptr)
{}

//-----------------------------------------------------------------------------

asiUI_ViewerPartListener::~asiUI_ViewerPartListener()
{}

//-----------------------------------------------------------------------------

void asiUI_ViewerPartListener::Connect()
{
  asiUI_Viewer3dListener::Connect(); // Connect basic reactions.

  connect( m_pViewer, SIGNAL ( facePicked(asiVisu_PickerResult*) ),
           this,      SLOT   ( onFacePicked(asiVisu_PickerResult*) ) );
  //
  connect( m_pViewer, SIGNAL ( edgePicked(asiVisu_PickerResult*) ),
           this,      SLOT   ( onEdgePicked(asiVisu_PickerResult*) ) );
  //
  connect( m_pViewer, SIGNAL ( vertexPicked(asiVisu_PickerResult*) ),
           this,      SLOT   ( onVertexPicked(asiVisu_PickerResult*) ) );
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPartListener::onFacePicked(asiVisu_PickerResult* pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes->GetPickedActor() );
  //
  if ( pickRes->GetPickedActor() && !nodeInfo )
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
    allSubShapes = geom_n->GetAAG()->RequestMapOfSubShapes();
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

void asiUI_ViewerPartListener::onEdgePicked(asiVisu_PickerResult* pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes->GetPickedActor() );
  //
  if ( pickRes->GetPickedActor() && !nodeInfo )
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
    allSubShapes = geom_n->GetAAG()->RequestMapOfSubShapes();
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

void asiUI_ViewerPartListener::onVertexPicked(asiVisu_PickerResult* pickRes)
{
  // Check if part is picked
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pickRes->GetPickedActor() );
  //
  if ( pickRes->GetPickedActor() && !nodeInfo )
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
    allSubShapes = geom_n->GetAAG()->RequestMapOfSubShapes();
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

void asiUI_ViewerPartListener::populateMenu(QMenu& menu)
{
  // Get highlighted faces and edges.
  TColStd_PackedMapOfInteger faceIndices, edgeIndices;
  //
  asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedFaces(faceIndices);
  asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedEdges(edgeIndices);

  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Part Node is null or bad-formed" );
    return;
  }

  // Prepare the context menu items.
  if ( faceIndices.Extent() || edgeIndices.Extent() )
  {
    // Add items specific to faces.
    if ( faceIndices.Extent() )
    {
      menu.addSeparator();
      //
      if ( m_pViewer->PrsMgr()->IsPresentable( STANDARD_TYPE(asiData_FaceNormsNode) ) )
        m_pShowNormsAction = menu.addAction("Show face normals");
      if ( m_pViewer->PrsMgr()->IsPresentable( STANDARD_TYPE(asiData_FaceContourNode) ) )
        m_pShowOriContourAction = menu.addAction("Show face oriented contour");
      //
      m_pInvertFacesAction = menu.addAction("Invert faces");
      m_pFindIsolated      = menu.addAction("Find isolated");
      m_pCheckDihAngle     = menu.addAction("Check dihedral angle");
    }

    menu.addSeparator();
    //
    m_pSaveBREPAction      = menu.addAction("Save to BREP...");
    m_pSetAsVariableAction = menu.addAction("Set as variable");

    // Add items which work for single-element selection.
    if ( faceIndices.Extent() == 1 || edgeIndices.Extent() == 1 )
    {
      m_pCopyAsStringAction = menu.addAction("Copy as JSON");
    }
  }
}

//-----------------------------------------------------------------------------

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
  // ACTION: copy as string
  //---------------------------------------------------------------------------
  else if ( pAction == m_pCopyAsStringAction )
  {
    // Get highlighted sub-shapes.
    TopTools_IndexedMapOfShape selected;
    asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

    // Dump to JSON string.
    TCollection_AsciiString jsonStr;
    //
    for ( int k = 1; k <= selected.Extent(); ++k )
    {
      std::ostringstream jsonStream;

      // Get selected shape.
      const TopoDS_Shape& selectedSh = selected(k);

      if ( selectedSh.ShapeType() == TopAbs_EDGE )
      {
        const TopoDS_Edge& selectedEdge = TopoDS::Edge(selectedSh);

        // Get curve.
        double f, l;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(selectedEdge, f, l);

        // Dump.
        asiAlgo_JSON::DumpCurve(curve, jsonStream);
      }
      else if ( selectedSh.ShapeType() == TopAbs_FACE )
      {
        const TopoDS_Face& selectedFace = TopoDS::Face(selectedSh);

        // Get surface.
        Handle(Geom_Surface) surface = BRep_Tool::Surface(selectedFace);

        // Dump.
        asiAlgo_JSON::DumpSurface(surface, jsonStream);
      }

      jsonStr += jsonStream.str().c_str();
    }

    // Set to clipboard.
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText( AsciiStr2QStr(jsonStr) );

    // Notify.
    m_progress.SendLogMessage( LogInfo(Normal) << "JSON was copied to clipboard." );
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
  else if ( pAction == m_pShowOriContourAction )
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

  //---------------------------------------------------------------------------
  // ACTION: set as variable
  //---------------------------------------------------------------------------
  if ( pAction == m_pSetAsVariableAction )
  {
    // Get highlighted sub-shapes
    TopTools_IndexedMapOfShape selected;
    asiEngine_Part( m_model, m_pViewer->PrsMgr() ).GetHighlightedSubShapes(selected);

    // Prepare a shape to set as a variable
    TopoDS_Shape shape2Var;
    //
    if ( selected.Extent() == 1 )
      shape2Var = selected(1);
    else
    {
      // Put selected shapes to a compound
      TopoDS_Compound comp;
      BRep_Builder().MakeCompound(comp);
      //
      for ( int k = 1; k <= selected.Extent(); ++k )
        BRep_Builder().Add( comp, selected(k) );
      //
      shape2Var = comp;
    }

    // Add variable via the imperative plotter
    m_plotter.DRAW_SHAPE(shape2Var, Color_Yellow, "var");
  }

  //---------------------------------------------------------------------------
  // ACTION: find isolated
  //---------------------------------------------------------------------------
  else if ( pAction == m_pFindIsolated )
  {
    asiEngine_Part partApi( m_model, m_pViewer->PrsMgr() );

    // Get highlighted faces
    asiAlgo_Feature faceIndices;
    partApi.GetHighlightedFaces(faceIndices);

    // Find features.
    asiAlgo_Feature
      isolated = asiEngine_Features(m_model,
                                    m_progress,
                                    m_plotter).FindIsolated(faceIndices);

    if ( !isolated.IsEmpty() )
      partApi.HighlightFaces(isolated);
    else
      m_progress.SendLogMessage(LogInfo(Normal) << "No isolated features found.");
  }

  //---------------------------------------------------------------------------
  // ACTION: check dihedral angle
  //---------------------------------------------------------------------------
  else if ( pAction == m_pCheckDihAngle )
  {
    asiEngine_Part partApi( m_model, m_pViewer->PrsMgr() );

    // Get highlighted faces.
    asiAlgo_Feature faceIndices;
    partApi.GetHighlightedFaces(faceIndices);

    if ( faceIndices.Extent() != 2 )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "There should be two faces to measure an angle.");
      return;
    }

    // Get AAG to access faces by indices.
    Handle(asiAlgo_AAG) aag = partApi.GetAAG();
    //
    if ( aag.IsNull() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "AAG is null.");
      return;
    }

    // Get faces.
    const TopoDS_Face& F = aag->GetFace( faceIndices.GetMinimalMapped() );
    const TopoDS_Face& G = aag->GetFace( faceIndices.GetMaximalMapped() );

    double xMin[2], yMin[2], zMin[2], xMax[2], yMax[2], zMax[2];
    asiAlgo_Utils::Bounds(F, xMin[0], yMin[0], zMin[0], xMax[0], yMax[0], zMax[0]);
    asiAlgo_Utils::Bounds(G, xMin[1], yMin[1], zMin[1], xMax[1], yMax[1], zMax[1]);
    //
    const double
      faceSize[2] = { gp_Pnt(xMin[0], yMin[0], zMin[0]).Distance( gp_Pnt(xMax[0], yMax[0], zMax[0]) ),
                      gp_Pnt(xMin[1], yMin[1], zMin[1]).Distance( gp_Pnt(xMax[1], yMax[1], zMax[1]) ) };
    //
    const double
      glyphCoeff = Max(faceSize[0], faceSize[1])*0.1;

    // Measure the angle.
    double                     angleRad = 0.;
    TopTools_IndexedMapOfShape commonEdges;
    gp_Pnt                     FP, GP;
    gp_Vec                     FN, GN;
    //
    asiAlgo_CheckDihedralAngle angChecker;
    //
    asiAlgo_FeatureAngleType
      angleType = angChecker.AngleBetweenFaces(F, G, false, 1.0e-3,
                                               commonEdges, angleRad,
                                               FP, GP, FN, GN);

    if ( !commonEdges.Extent() )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "The selected faces are not adjacent.");
      return;
    }

    m_progress.SendLogMessage( LogInfo(Normal) << "Angle is %1 degrees, %2."
                                               << Abs(angleRad)*180./M_PI
                                               << asiAlgo_Utils::FeatureAngleToString(angleType) );

    double colorR, colorG, colorB;
    asiVisu_Utils::ColorForFeatureAngle(angleType, colorR, colorG, colorB);
    //
    for ( int eidx = 1; eidx <= commonEdges.Extent(); ++eidx )
    {
      m_plotter.REDRAW_SHAPE("vexity",
                             commonEdges(eidx),
                             ActAPI_Color(colorR,
                                          colorG,
                                          colorB,
                                          Quantity_TOC_RGB),
                             1.0,
                             true);
    }
    //
    m_plotter.REDRAW_VECTOR_AT("FN", FP, FN*glyphCoeff, Color_Red);
    m_plotter.REDRAW_VECTOR_AT("GN", GP, GN*glyphCoeff, Color_Red);
  }
}
