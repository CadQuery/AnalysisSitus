//-----------------------------------------------------------------------------
// Created on: 30 November 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapeRobustTessellator.h>

// asiVisu includes
#include <asiVisu_CurveSource.h>

// asiAlgo includes
#include <asiAlgo_MeshGen.h>
#include <asiAlgo_MeshMerge.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_ShapeRobustTessellator);

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_ShapeRobustTessellator::asiVisu_ShapeRobustTessellator()
: vtkObject()
{
  m_data = new asiVisu_ShapeData; // Allocate data for visualization primitives
                                  // (i.e. VTK points, cells, arrays)
}

//-----------------------------------------------------------------------------

//! Initializes tessellation tool.
//! \param aag                   [in] AAG.
//! \param linearDeflection      [in] linear deflection.
//! \param angularDeflection_deg [in] angular deflection in degrees.
//! \param progress              [in] progress notifier.
//! \param plotter               [in] imperative plotter.
void asiVisu_ShapeRobustTessellator::Initialize(const Handle(asiAlgo_AAG)& aag,
                                                const double               linearDeflection,
                                                const double               angularDeflection_deg,
                                                ActAPI_ProgressEntry       progress,
                                                ActAPI_PlotterEntry        plotter)
{
  m_aag   = aag;
  m_shape = m_aag->GetMasterCAD();
  //
  this->internalInit(linearDeflection, angularDeflection_deg, progress, plotter);
}

//-----------------------------------------------------------------------------

//! Initializes tessellation tool.
//! \param shape                 [in] shape.
//! \param linearDeflection      [in] linear deflection.
//! \param angularDeflection_deg [in] angular deflection in degrees.
//! \param progress              [in] progress notifier.
//! \param plotter               [in] imperative plotter.
void asiVisu_ShapeRobustTessellator::Initialize(const TopoDS_Shape&  shape,
                                                const double         linearDeflection,
                                                const double         angularDeflection_deg,
                                                ActAPI_ProgressEntry progress,
                                                ActAPI_PlotterEntry  plotter)
{
  m_aag.Nullify();
  m_shape = shape;
  //
  this->internalInit(linearDeflection, angularDeflection_deg, progress, plotter);
}

//-----------------------------------------------------------------------------

//! Builds the polygonal data.
void asiVisu_ShapeRobustTessellator::Build()
{
  this->internalBuild();
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::internalInit(const double         linearDeflection,
                                                  const double         angularDeflection_deg,
                                                  ActAPI_ProgressEntry progress,
                                                  ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;

  // Set linear deflection
  if ( Abs(linearDeflection) < asiAlgo_TooSmallValue )
    m_fLinDeflection = asiAlgo_Utils::AutoSelectLinearDeflection(m_shape);
  else
    m_fLinDeflection = linearDeflection;
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "Faceter linear deflection is %1" << m_fLinDeflection);

  // Set angular deflection
  if ( Abs(angularDeflection_deg) < asiAlgo_TooSmallValue )
    m_fAngDeflectionDeg = asiAlgo_Utils::AutoSelectAngularDeflection(m_shape);
  else
    m_fAngDeflectionDeg = angularDeflection_deg;
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "Faceter angular deflection is %1 deg." << m_fAngDeflectionDeg);
}

//-----------------------------------------------------------------------------

//! Internal method which builds the polygonal data.
void asiVisu_ShapeRobustTessellator::internalBuild()
{
  if ( m_shape.IsNull() )
    return;

#if defined COUT_DEBUG
  TIMER_NEW
  TIMER_GO
#endif

  // Build map of shapes and their parents
  TopTools_IndexedDataMapOfShapeListOfShape verticesOnEdges;
  TopExp::MapShapesAndAncestors(m_shape, TopAbs_VERTEX, TopAbs_EDGE, verticesOnEdges);
  //
  TopTools_IndexedDataMapOfShapeListOfShape edgesOnFaces;
  TopExp::MapShapesAndAncestors(m_shape, TopAbs_EDGE, TopAbs_FACE, edgesOnFaces);

  // Get maps of sub-shapes (either cached in AAG or not)
  TopTools_IndexedMapOfShape allVertices;
  TopTools_IndexedMapOfShape allEdges;
  TopTools_IndexedMapOfShape allSubShapes;
  //
  if ( m_aag.IsNull() )
  {
    TopExp::MapShapes(m_shape, TopAbs_VERTEX, allVertices);
    TopExp::MapShapes(m_shape, TopAbs_EDGE, allEdges);
    TopExp::MapShapes(m_shape, allSubShapes);
  }
  else
  {
    allVertices  = m_aag->GetMapOfVertices();
    allEdges     = m_aag->GetMapOfEdges();
    allSubShapes = m_aag->GetMapOfSubShapes();
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: build topology maps")
#endif

  /* =========================================
   *  STAGE 1: fill data source with vertices
   * ========================================= */

#if defined COUT_DEBUG
  TIMER_RESET
  TIMER_GO
#endif

  // Add vertices
  for ( int vidx = 1; vidx <= allVertices.Extent(); ++vidx )
  {
    const TopoDS_Vertex&            v = TopoDS::Vertex( allVertices(vidx) );
    const TopTools_ListOfShape& edges = verticesOnEdges.FindFromKey(v);

    asiVisu_ShapePrimitive type;
    if ( edges.IsEmpty() )
      type = ShapePrimitive_FreeVertex;
    else
      type = ShapePrimitive_SharedVertex;

    // Get sub-shape ID
    vtkIdType ssId = allSubShapes.FindIndex(v);

    // Add vertex to the data source
    this->addVertex(v, ssId, type);
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: add vertices as VTK cells")
#endif

  /* ===============
   *  STAGE 2: mesh
   * =============== */

#if defined COUT_DEBUG
  TIMER_RESET
  TIMER_GO
#endif

  // Discretize B-Rep model to produce visualization facets
  asiAlgo_MeshInfo meshInfo;
  if ( !asiAlgo_MeshGen::DoNative(m_shape,
                                  m_fLinDeflection,
                                  m_fAngDeflectionDeg,
                                  meshInfo) )
  {
    vtkErrorMacro( << "Cannot tessellate B-Rep model with linear deflection " << m_fLinDeflection
                   << " and angular deflection " << m_fAngDeflectionDeg << " [deg]");
    return;
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: B-Rep mesh")
#endif

  /* ======================================
   *  STAGE 3: fill data source with edges
   * ====================================== */

#if defined COUT_DEBUG
  TIMER_RESET
  TIMER_GO
#endif

  // Add edges
  for ( int eidx = 1; eidx <= allEdges.Extent(); ++eidx )
  {
    const TopoDS_Edge&              e = TopoDS::Edge( allEdges(eidx) );
    const TopTools_ListOfShape& faces = edgesOnFaces.FindFromKey(e);

    asiVisu_ShapePrimitive type;
    if ( !faces.Extent() )
      type = ShapePrimitive_DanglingEdge;
    else if ( faces.Extent() == 1 )
      type = ShapePrimitive_FreeEdge;
    else if ( faces.Extent() > 2 )
      type = ShapePrimitive_NonManifoldEdge;
    else
      type = ShapePrimitive_ManifoldEdge;

    // Get sub-shape ID
    vtkIdType ssId = allSubShapes.FindIndex(e);

    // Add edge to the data source
    this->addEdge(e, ssId, type);
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: add edges as VTK cells")
#endif

  /* ======================================
   *  STAGE 4: fill data source with faces
   * ====================================== */

#if defined COUT_DEBUG
  TIMER_RESET
  TIMER_GO
#endif

  // Loop over the faces
  for ( TopExp_Explorer exp(m_shape, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( exp.Current() );
    //
    if ( face.IsNull() )
      continue;

    // Get facets associated with the face
    TopLoc_Location loc;
    const Handle(Poly_Triangulation)& triangulation = BRep_Tool::Triangulation(face, loc);
    //
    if ( triangulation.IsNull() )
      continue;

    // Populate node
    NCollection_DataMap<int, vtkIdType> pointIds;
    const TColgp_Array1OfPnt& nodes = triangulation->Nodes();
    //
    for ( int pidx = nodes.Lower(); pidx <= nodes.Upper(); ++pidx )
    {
      gp_Pnt P = nodes(pidx);
      //
      if ( !loc.IsIdentity() )
        P.Transform(loc);

      // Insert VTK point
      vtkIdType pid = m_data->InsertCoordinate( P.X(), P.Y(), P.Z() );
      //
      pointIds.Bind(pidx, pid);
    }

    // Get sub-shape ID
    vtkIdType ssId = allSubShapes.FindIndex(face);

    // Create triangle cells
    const Poly_Array1OfTriangle& triangles = triangulation->Triangles();
    //
    for ( int t = triangles.Lower(); t <= triangles.Upper(); ++t )
    {
      const Poly_Triangle& triangle = triangles(t);

      int n1, n2, n3;
      triangle.Get(n1, n2, n3);

      // Insert VTK cell
      m_data->InsertTriangle(ssId,
                             pointIds(n1),
                             pointIds(n2),
                             pointIds(n3),
                             ShapePrimitive_Facet);
    }
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: add faces as VTK cells")
#endif
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addVertex(const TopoDS_Vertex&         vertex,
                                               const vtkIdType              shapeId,
                                               const asiVisu_ShapePrimitive scType)
{
  if ( vertex.IsNull() )
    return;

  // Access host Cartesian point
  gp_Pnt pnt = BRep_Tool::Pnt(vertex);

  // Fill data structure
  vtkIdType pid = m_data->InsertCoordinate( pnt.X(), pnt.Y(), pnt.Z() );
  m_data->InsertVertex(shapeId, pid, scType);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addEdge(const TopoDS_Edge&           edge,
                                             const vtkIdType              shapeId,
                                             const asiVisu_ShapePrimitive scType)
{
  if ( edge.IsNull() || BRep_Tool::Degenerated(edge) )
    return;

  // IDs of the edge points
  vtkSmartPointer<vtkIdList> pids = vtkSmartPointer<vtkIdList>::New();

  // Take polygon on triangulation
  Handle(Poly_PolygonOnTriangulation) polyOn;
  Handle(Poly_Triangulation) poly;
  TopLoc_Location loc;
  //
  {
    Handle(Poly_PolygonOnTriangulation) polyOn1, polyOn2;
    Handle(Poly_Triangulation) poly1, poly2;
    TopLoc_Location loc1, loc2;
    //
    BRep_Tool::PolygonOnTriangulation(edge, polyOn1, poly1, loc1, 1);
    BRep_Tool::PolygonOnTriangulation(edge, polyOn2, poly2, loc2, 2);
    //
    if ( !polyOn1.IsNull() && !polyOn2.IsNull() )
    {
      if ( polyOn2->Nodes().Length() != polyOn1->Nodes().Length() )
        m_progress.SendLogMessage(LogWarn(Normal) << "*** Different number of nodes in polygons on triangulation");

      // Choose more rich polygon. This is weird, but it may happen that even
      // for conformal mesh the number of nodes in polygons are different.
      // It looks like a bug in OCCT faceter...
      if ( polyOn2->Nodes().Length() > polyOn1->Nodes().Length() )
      {
        polyOn = polyOn2;
        poly   = poly2;
        loc    = loc2;
      }
      else
      {
        polyOn = polyOn1;
        poly   = poly1;
        loc    = loc1;
      }
    }
    else
    {
      polyOn = polyOn1; // Take the default polygon on triangulation which is the first one
      poly   = poly1;
      loc    = loc1;
    }
  }

  // If the edge is not associated with any triangulation (e.g. the master
  // shape is a wire), it may still have an explicitly defined 3D polygon
  if ( polyOn.IsNull() || poly.IsNull() )
  {
    const Handle(Poly_Polygon3D)& polygon3d = BRep_Tool::Polygon3D(edge, loc);
    //
    if ( polygon3d.IsNull() )
      return;

    const TColgp_Array1OfPnt& points = polygon3d->Nodes();

    // Add points to VTK geometry set
    for ( int pidx = points.Lower(); pidx <= points.Upper(); ++pidx )
    {
      gp_Pnt P = points(pidx);
      //
      if ( !loc.IsIdentity() )
        P.Transform(loc);

      // Insert VTK point
      vtkIdType pid = m_data->InsertCoordinate( P.X(), P.Y(), P.Z() );
      //
      pids->InsertNextId(pid);
    }
  }
  else
  {
    // Add node indices to the collection of boundary nodes
    const TColStd_Array1OfInteger& polygonOnTriNodes = polyOn->Nodes();
    //
    for ( int k = polygonOnTriNodes.Lower(); k <= polygonOnTriNodes.Upper(); ++k )
    {
      const int pidx = polygonOnTriNodes(k);
      gp_Pnt P = poly->Nodes()(pidx);
      //
      if ( !loc.IsIdentity() )
        P.Transform(loc);

      // Insert VTK point
      vtkIdType pid = m_data->InsertCoordinate( P.X(), P.Y(), P.Z() );
      //
      pids->InsertNextId(pid);
    }
  }

  // Create VTK topology
  m_data->InsertPolyline(shapeId, pids, scType);
}
