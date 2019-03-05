//-----------------------------------------------------------------------------
// Created on: 24 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiAlgo_RepatchFaces.h>

// asiAlgo includes
#include <asiAlgo_CheckValidity.h>
#include <asiAlgo_PlateOnEdges.h>
#include <asiAlgo_TopoKill.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeFix_Face.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_RepatchFaces::asiAlgo_RepatchFaces(const TopoDS_Shape&  shape,
                                           ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter), m_input(shape)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_RepatchFaces::Perform(const std::vector<TopoDS_Face>& faces)
{
  // Prepare face killer.
  asiAlgo_TopoKill killer(m_input, m_progress, m_plotter);

  // Prepare a compound of faces in question.
  TopoDS_Compound compFaces;
  BRep_Builder().MakeCompound(compFaces);
  //
  for ( size_t k = 0; k < faces.size(); ++k )
  {
    killer.AskRemove(faces[k]);

    BRep_Builder().Add(compFaces, faces[k]);
  }

  // Build map of edges to extract free ones.
  TopTools_IndexedDataMapOfShapeListOfShape edgesFaces;
  TopExp::MapShapesAndAncestors(compFaces, TopAbs_EDGE, TopAbs_FACE, edgesFaces);

  // Get free edges.
  Handle(TopTools_HSequenceOfShape) freeEdgesSeq = new TopTools_HSequenceOfShape;
  //
  for ( int k = 1; k <= edgesFaces.Extent(); ++k )
  {
    const TopTools_ListOfShape& facesByEdge = edgesFaces(k);
    //
    if ( facesByEdge.Extent() == 1 )
    {
      const TopoDS_Edge& E = TopoDS::Edge( edgesFaces.FindKey(k) );
      //
      if ( BRep_Tool::Degenerated(E) )
        continue;

      freeEdgesSeq->Append(E);

#if defined DRAW_DEBUG
      m_plotter.DRAW_SHAPE(freeEdges[freeEdges.size() - 1], Color_Red, 1.0, true, "freeEdge");
#endif
    }
  }

  // Compose a new wire from the free edges.
  Handle(TopTools_HSequenceOfShape) freeWires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(freeEdgesSeq, 1e-3, 0, freeWires);
  //
  const TopoDS_Wire& repatchW = TopoDS::Wire( freeWires->First() );

#if defined DRAW_DEBUG
  m_plotter.REDRAW_SHAPE("repatchW", repatchW, Color_Red, 1.0, true);
#endif

  // Prepare interpolation tool.
  asiAlgo_PlateOnEdges interpAlgo(m_input, m_progress, m_plotter);

  // Interpolate.
  Handle(Geom_BSplineSurface) repatchSurf;
  //
  if ( !interpAlgo.BuildSurf(freeEdgesSeq, GeomAbs_C0, repatchSurf) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Interpolation failed.");
    return false;
  }

#if defined DRAW_DEBUG
  m_plotter.DRAW_SURFACE(repatchSurf, Color_White, "repatch");
#endif

  // Build new face.
  TopoDS_Face repatchF = BRepBuilderAPI_MakeFace(repatchSurf, repatchW, false);
  //
  ShapeFix_Face ShapeHealer(repatchF);
  ShapeHealer.Perform();
  repatchF = ShapeHealer.Face();

  // Classify point to invert wire if necessary.
  BRepTopAdaptor_FClass2d FClass(repatchF, 0.);
  if ( FClass.PerformInfinitePoint() == TopAbs_IN)
  {
    BRep_Builder B;
    TopoDS_Shape S = repatchF.EmptyCopied();
    TopoDS_Iterator it(repatchF);
    while ( it.More() )
    {
      B.Add( S, it.Value().Reversed() );
      it.Next();
    }
    repatchF = TopoDS::Face(S);
  }

#if defined DRAW_DEBUG
  m_plotter.DRAW_SHAPE(repatchF, Color_White, "repatchF");
#endif

  // Kill old faces.
  if ( !killer.Apply() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Face killer failed.");
    return true;
  }
  //
  const TopoDS_Shape& shape = killer.GetResult();

  // Sew.
  BRepBuilderAPI_Sewing Sewer( asiAlgo_CheckValidity::MaxTolerance(shape), true, false, false, false );
  Sewer.Add(shape);
  Sewer.Add(repatchF);
  //
  Sewer.Perform();
  m_result = Sewer.SewedShape();

  return true;
}
