//-----------------------------------------------------------------------------
// Created on: 30 November 2016
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
#include <asiVisu_ShapeRobustTessellator.h>

// asiVisu includes
#include <asiVisu_CurveSource.h>

// asiAlgo includes
#include <asiAlgo_AttrFaceColor.h>
#include <asiAlgo_CheckValidity.h>
#include <asiAlgo_MeshGen.h>
#include <asiAlgo_MeshMerge.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

#define DEFDEFL 1.0
#define INFDEFL 1e10
#define NUMISOS 20
#define ISOSDEFLECTION 0.0001

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_ShapeRobustTessellator);

//-----------------------------------------------------------------------------

//! Ctor.
asiVisu_ShapeRobustTessellator::asiVisu_ShapeRobustTessellator()
: vtkObject(), m_fGlobalBndDiag(0.0)
{
  m_data = new asiVisu_ShapeData; // Allocate data for visualization primitives
                                  // (i.e. VTK points, cells, arrays).

  // Initialize the scalar generator for faces.
  m_pScalarGen = new t_colorScalarGenerator(ShapePrimitive_LAST);
}

//-----------------------------------------------------------------------------

//! Dtor.
asiVisu_ShapeRobustTessellator::~asiVisu_ShapeRobustTessellator()
{
  delete m_pScalarGen;
}

//-----------------------------------------------------------------------------

//! Initializes tessellation tool.
//! \param[in] aag                   AAG.
//! \param[in] linearDeflection      linear deflection.
//! \param[in] angularDeflection_deg angular deflection in degrees.
//! \param[in] colorSource           color source for sub-shapes.
//! \param[in] progress              progress notifier.
//! \param[in] plotter               imperative plotter.
void asiVisu_ShapeRobustTessellator::Initialize(const Handle(asiAlgo_AAG)&              aag,
                                                const double                            linearDeflection,
                                                const double                            angularDeflection_deg,
                                                const Handle(asiVisu_ShapeColorSource)& colorSource,
                                                ActAPI_ProgressEntry                    progress,
                                                ActAPI_PlotterEntry                     plotter)
{
  m_aag   = aag;
  m_shape = m_aag->GetMasterCAD();
  //
  this->internalInit(linearDeflection,
                     angularDeflection_deg,
                     colorSource,
                     progress,
                     plotter);
}

//-----------------------------------------------------------------------------

//! Initializes tessellation tool.
//! \param[in] shape                 shape.
//! \param[in] linearDeflection      linear deflection.
//! \param[in] angularDeflection_deg angular deflection in degrees.
//! \param[in] colorSource           color source for sub-shapes.
//! \param[in] progress              progress notifier.
//! \param[in] plotter               imperative plotter.
void asiVisu_ShapeRobustTessellator::Initialize(const TopoDS_Shape&                     shape,
                                                const double                            linearDeflection,
                                                const double                            angularDeflection_deg,
                                                const Handle(asiVisu_ShapeColorSource)& colorSource,
                                                ActAPI_ProgressEntry                    progress,
                                                ActAPI_PlotterEntry                     plotter)
{
  m_aag.Nullify();
  m_shape = shape;
  //
  this->internalInit(linearDeflection,
                     angularDeflection_deg,
                     colorSource,
                     progress,
                     plotter);
}

//-----------------------------------------------------------------------------

//! Builds the polygonal data.
void asiVisu_ShapeRobustTessellator::Build()
{
  this->internalBuild();
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::internalInit(const double                            linearDeflection,
                                                  const double                            angularDeflection_deg,
                                                  const Handle(asiVisu_ShapeColorSource)& colorSource,
                                                  ActAPI_ProgressEntry                    progress,
                                                  ActAPI_PlotterEntry                     plotter)
{
  m_colorSource = colorSource;
  m_progress    = progress;
  m_plotter     = plotter;

  // Set linear deflection
  if ( Abs(linearDeflection) < asiAlgo_TooSmallValue )
    m_fLinDeflection = asiAlgo_MeshGen::AutoSelectLinearDeflection(m_shape);
  else
    m_fLinDeflection = linearDeflection;
  //
  if ( m_fLinDeflection > INFDEFL )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Too high linear deflection (%1). "
                                                 "We will use the default value (%2) instead."
                                              << m_fLinDeflection << DEFDEFL);

    m_fLinDeflection = DEFDEFL;
  }
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "Faceter linear deflection is %1." << m_fLinDeflection);

  // Set angular deflection
  if ( Abs(angularDeflection_deg) < asiAlgo_TooSmallValue )
    m_fAngDeflectionDeg = asiAlgo_MeshGen::AutoSelectAngularDeflection(m_shape);
  else
    m_fAngDeflectionDeg = angularDeflection_deg;
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "Faceter angular deflection is %1 deg." << m_fAngDeflectionDeg);

  // Calculate global bounding box which will be used for calibration of
  // auxiliary visualization primitives
  double xmin, ymin, zmin, xmax, ymax, zmax;
  asiAlgo_Utils::Bounds(m_shape, xmin, ymin, zmin, xmax, ymax, zmax);
  //
  m_fGlobalBndDiag = asiVisu_Utils::TrimInf( gp_Pnt(xmax, ymax, zmax).Distance( gp_Pnt(xmin, ymin, zmin) ) );
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

  // Get maps of sub-shapes (either cached in AAG or not)
  TopTools_IndexedMapOfShape                allVertices;
  TopTools_IndexedMapOfShape                allEdges;
  TopTools_IndexedMapOfShape                allFaces;
  TopTools_IndexedMapOfShape                allSubShapes;
  TopTools_IndexedDataMapOfShapeListOfShape edgesOnFaces;
  //
  if ( m_aag.IsNull() )
  {
    TopExp::MapShapes(m_shape, TopAbs_VERTEX, allVertices);
    TopExp::MapShapes(m_shape, TopAbs_EDGE, allEdges);
    TopExp::MapShapes(m_shape, TopAbs_FACE, allFaces);
    TopExp::MapShapes(m_shape, allSubShapes);

    // Edges and their owner faces.
    TopExp::MapShapesAndAncestors(m_shape, TopAbs_EDGE, TopAbs_FACE, edgesOnFaces);
  }
  else
  {
    allVertices  = m_aag->RequestMapOfVertices();
    allEdges     = m_aag->RequestMapOfEdges();
    allFaces     = m_aag->GetMapOfFaces();
    allSubShapes = m_aag->RequestMapOfSubShapes();
    edgesOnFaces = m_aag->RequestMapOfEdgesFaces();
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
    else if ( edges.Extent() == 1 )
      type = ShapePrimitive_BorderVertex;
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
    {
      // If there are no faces for the edge, it can be either free or dangling.
      // The difference is in the number of parent edges for the vertices
      // in question. If there is only one parent (the edge itself), then
      // we conclude that the edge is free. If there are more than one parents,
      // the edge is said to be dangling.

      TopoDS_Vertex Vf = ShapeAnalysis_Edge().FirstVertex(e);
      TopoDS_Vertex Vl = ShapeAnalysis_Edge().LastVertex(e);

      if ( Vf.IsNull() || Vl.IsNull() )
      {
        m_progress.SendLogMessage(LogWarn(Normal) << "*** First or last vertex is null for edge.");
        type = ShapePrimitive_FreeEdge;
      }
      else if ( verticesOnEdges.FindFromKey(Vf).Extent() > 1 ||
                verticesOnEdges.FindFromKey(Vl).Extent() > 1 )
        type = ShapePrimitive_DanglingEdge;
      else
        type = ShapePrimitive_FreeEdge;
    }
    else if ( faces.Extent() == 1 )
      type = ShapePrimitive_BorderEdge;
    else if ( faces.Extent() > 2 )
      type = ShapePrimitive_NonManifoldEdge;
    else
      type = ShapePrimitive_ManifoldEdge;

    // Get edge ID
    int edgeId = allEdges.FindIndex(e);

    // Get sub-shape ID
    vtkIdType ssId = allSubShapes.FindIndex(e);

    // Add edge to the data source
    this->addEdge(e, ssId, edgeId, type);
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: add edges as VTK cells")
#endif

  /* ==============================================
   *  STAGE 4: fill data source with invalid faces
   * ============================================== */

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
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "*** Iterating NULL face.");
      continue;
    }

    // Get face ID
    int faceId = allFaces.FindIndex(face);

    if ( !this->isValidFace(face) )
    {
      // Get sub-shape ID
      vtkIdType ssId = allSubShapes.FindIndex(face);

      // Add face to the data source
      this->addFace(face, ssId, faceId, false);
    }
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: add invalid faces as VTK cells")
#endif

  /* ======================================
   *  STAGE 5: fill data source with faces
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
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "*** Iterating NULL face.");
      continue;
    }

    if ( this->isValidFace(face) )
    {
      // Get sub-shape ID
      vtkIdType ssId = allSubShapes.FindIndex(face);

      // Get face ID
      int faceId = allFaces.FindIndex(face);

      // Add face to the data source
      this->addFace(face, ssId, faceId, true);
    }
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("asiVisu_ShapeRobustTessellator: add valid faces as VTK cells")
#endif
}

//-----------------------------------------------------------------------------

bool asiVisu_ShapeRobustTessellator::isValidFace(const TopoDS_Face& face) const
{
  // Notice that in the check of wire closeness, we do not enable
  // "check sharing" flag. This is because a faceter usually does not have
  // any troubles with tessellating geometrically but not topologically closed
  // faces.

  asiAlgo_CheckValidity checker;

  // This calibrated value is used to compensate weird tolerances which
  // happen to be insufficient to cover tiny contour gaps.
  const double tol = checker.MaxTolerance(face)*5.0;

  // Location just to access triangulation.
  TopLoc_Location loc;

  return checker.HasAllClosedWires(face, tol) &&
        !checker.HasEdgesWithoutVertices(face) &&
        !BRep_Tool::Triangulation(face, loc).IsNull();
}

//-----------------------------------------------------------------------------

int asiVisu_ShapeRobustTessellator::getFaceScalar(const int                    faceId,
                                                  const asiVisu_ShapePrimitive defaultType) const
{
  if ( m_colorSource.IsNull() ) // Null check.
    return defaultType;

  // Get color.
  const int color = m_colorSource->GetFaceColor(faceId);
  //
  if ( color == -1 )
    return defaultType;

  // Convert color to scalar.
  const int scalar = m_pScalarGen->GetScalar(color);
  return scalar;
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
                                             const int                    edgeId,
                                             const asiVisu_ShapePrimitive scType)
{
  if ( edge.IsNull() || BRep_Tool::Degenerated(edge) )
  {
#if defined COUT_DEBUG
    m_progress.SendLogMessage(LogWarn(Normal) << "*** Skip null or degenerated edge %1."
                                              << edgeId);
#endif
    return;
  }

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
        m_progress.SendLogMessage(LogWarn(Normal) << "*** Different number of nodes in polygons on triangulation for edge %1."
                                                  << edgeId);

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
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "*** No polygons available for edge %1."
                                                << edgeId);

      // When working with a "broken" element, it is a good idea to
      // wrap with try-catch.
      try
      {
        this->addBrokenEdgeCurve(edge, shapeId, edgeId, scType);
      }
      catch ( ... )
      {
        m_progress.SendLogMessage(LogWarn(Normal) << "*** Cannot visualize anything for edge %1."
                                                  << edgeId);
      }
      return;
    }

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

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addFace(const TopoDS_Face& face,
                                             const vtkIdType    shapeId,
                                             const int          faceId,
                                             const bool         isValid)
{
  // Check whether the face to visualize is "correct enough" or not.
  // If not, there is no sense to use its visualization facets: most
  // likely the facets were not constructed...
  if ( !isValid )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Too faulty face %1 detected: "
                                                 "visualization facets skipped."
                                              << faceId);

    // Get bounding box of the face
    double xMin = 0, yMin = 0, zMin = 0, xMax = 0, yMax = 0, zMax = 0;
    const bool
      canComputeBounds = asiAlgo_Utils::Bounds(face, xMin, yMin, zMin, xMax, yMax, zMax);

    // Trim diagonal to avoid infinite here. This can happen for single infinite
    // cylindrical faces for example.
    const double
      boundsDiag = Min( gp_Pnt(xMax, yMax, zMax).Distance( gp_Pnt(xMin, yMin, zMin) ), m_fGlobalBndDiag);

    // Get host surface
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    //
    if ( surf.IsNull() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot process face %1 without surface."
                                               << faceId);
      return;
    }

    // Get natural bounds
    double uMin, uMax, vMin, vMax, uMinSurf, uMaxSurf, vMinSurf, vMaxSurf;
    surf->Bounds(uMinSurf, uMaxSurf, vMinSurf, vMaxSurf);

    // Get parametric bound
    double uMinParam, uMaxParam, vMinParam, vMaxParam;
    BRepTools::UVBounds(face, uMinParam, uMaxParam, vMinParam, vMaxParam);

    // Choose min and max
    uMin = Max(uMinSurf, uMinParam);
    uMax = Min(uMaxSurf, uMaxParam);
    vMin = Max(vMinSurf, vMinParam);
    vMax = Min(vMaxSurf, vMaxParam);

    // Trim natural bounds by the face bounds (if such can be computed)
    if ( canComputeBounds )
    {
      uMin = asiVisu_Utils::Trim(uMin, boundsDiag);
      uMax = asiVisu_Utils::Trim(uMax, boundsDiag);
      vMin = asiVisu_Utils::Trim(vMin, boundsDiag);
      vMax = asiVisu_Utils::Trim(vMax, boundsDiag);
    }

    // Boundary is very ill-defined. If min and max parameter values coincide,
    // we cannot draw even surface isolines, so let's try to expand...
    if ( Abs(uMin - uMax) < RealEpsilon() )
    {
      uMin = asiVisu_Utils::TrimInf(uMinSurf, m_fGlobalBndDiag*0.5);
      uMax = asiVisu_Utils::TrimInf(uMaxSurf, m_fGlobalBndDiag*0.5);
    }
    if ( Abs(vMin - vMax) < RealEpsilon() )
    {
      vMin = asiVisu_Utils::TrimInf(vMinSurf, m_fGlobalBndDiag*0.5);
      vMax = asiVisu_Utils::TrimInf(vMaxSurf, m_fGlobalBndDiag*0.5);
    }

    const double uStep = (uMax - uMin) / NUMISOS;
    const double vStep = (vMax - vMin) / NUMISOS;
    //
    if ( Abs(uStep) < RealEpsilon() || Abs(vStep) < RealEpsilon() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Attempt to use zero parametric step.");
      return;
    }

    // Generate u-isos
    double u     = uMin;
    bool   uStop = false;
    while ( !uStop )
    {
      if ( u > uMax )
      {
        u     = uMax;
        uStop = true;
      }

      // For better visibility of edges, we avoid visualization of isoline
      // extremities as they are likely to overlap with edges.
      if ( Abs(u - uMin) > 1.0e-4 && Abs(u - uMax) > 1.0e-4 )
      {
        Handle(Geom_Curve) uIso = surf->UIso(u);
        //
        if ( uIso.IsNull() )
          continue;

        // Choose parameter range
        double f, l;
        if ( uIso->IsKind( STANDARD_TYPE(Geom_TrimmedCurve) ) )
        {
          f = uIso->FirstParameter();
          l = uIso->LastParameter();
        }
        else
        {
          f = Max( vMin, uIso->FirstParameter() );
          l = Min( vMax, uIso->LastParameter() );
        }

        // Add isoline to data source
        this->addCurve(uIso, f, l, shapeId, ShapePrimitive_Isoline);
      }

      u += uStep;
    }

    // Generate v-isos
    double v     = vMin;
    bool   vStop = false;
    while ( !vStop )
    {
      if ( v > vMax )
      {
        v     = vMax;
        vStop = true;
      }

      // For better visibility of edges, we avoid visualization of isoline
      // extremities as they are likely to overlap with edges.
      if ( Abs(v - vMin) > 1.0e-4 && Abs(v - vMax) > 1.0e-4 )
      {
        Handle(Geom_Curve) vIso = surf->VIso(v);
        //
        if ( vIso.IsNull() )
          continue;

        // Choose parameter range
        double f, l;
        if ( vIso->IsKind( STANDARD_TYPE(Geom_TrimmedCurve) ) )
        {
          f = vIso->FirstParameter();
          l = vIso->LastParameter();
        }
        else
        {
          f = Max( uMin, vIso->FirstParameter() );
          l = Min( uMax, vIso->LastParameter() );
        }

        // Add isoline to data source
        this->addCurve(vIso, f, l, shapeId, ShapePrimitive_Isoline);
      }

      v += vStep;
    }
  }
  else
  {
    const bool isReversed = (face.Orientation() == TopAbs_REVERSED);

    // Get facets associated with the face
    TopLoc_Location loc;
    const Handle(Poly_Triangulation)& triangulation = BRep_Tool::Triangulation(face, loc);
    //
    if ( triangulation.IsNull() )
      return;

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

    // Create triangle cells
    const Poly_Array1OfTriangle& triangles = triangulation->Triangles();
    //
    for ( int t = triangles.Lower(); t <= triangles.Upper(); ++t )
    {
      const Poly_Triangle& triangle = triangles(t);

      int n1, n2, n3;
      triangle.Get(n1, n2, n3);

      // Insert VTK cell
      m_data->InsertTriangle( shapeId,
                              isReversed ? pointIds(n3) : pointIds(n1),
                              pointIds(n2),
                              isReversed ? pointIds(n1) : pointIds(n3),
                              this->getFaceScalar(faceId, ShapePrimitive_Facet) );
    }
  }
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addCurve(const Handle(Geom_Curve)&    curve,
                                              const double                 tMin,
                                              const double                 tMax,
                                              const vtkIdType              shapeId,
                                              const asiVisu_ShapePrimitive scType)
{
  if ( tMin > tMax )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Skip curve with invalid parametric range.");
    return; // Protect from crashes.
  }

  // Discretize
  GeomAdaptor_Curve gac(curve, tMin, tMax);
  GCPnts_QuasiUniformDeflection Defl(gac, ISOSDEFLECTION);
  //
  if ( !Defl.IsDone() )
  {
    vtkErrorMacro( << "Cannot discretize curve" );
    return;
  }
  const int nPts = Defl.NbPoints();
  if ( !nPts )
    return;

  // IDs of the isoline discretization points
  vtkSmartPointer<vtkIdList> pids = vtkSmartPointer<vtkIdList>::New();

  for ( int k = 1; k <= nPts; ++k )
  {
    gp_Pnt P = Defl.Value(k);

    // Insert VTK point
    vtkIdType pid = m_data->InsertCoordinate( P.X(), P.Y(), P.Z() );
    //
    pids->InsertNextId(pid);
  }

  // Create VTK topology
  m_data->InsertPolyline(shapeId, pids, scType);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addBrokenEdgeCurve(const TopoDS_Edge&           edge,
                                                        const vtkIdType              shapeId,
                                                        const int                    edgeId,
                                                        const asiVisu_ShapePrimitive scType)
{
  asiVisu_NotUsed(edgeId);

  double f, l;
  Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, f, l);
  //
  if ( curve.IsNull() )
    return;

  // Choose parameter range
  f = asiVisu_Utils::TrimInf(f);
  l = asiVisu_Utils::TrimInf(l);

  // Add curve
  this->addCurve(curve, f, l, shapeId, scType);
}
