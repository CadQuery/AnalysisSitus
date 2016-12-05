//-----------------------------------------------------------------------------
// Created on: 30 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapeRobustTessellator.h>

// VTK includes
#include <asiVisu_CurveSource.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <IVtkOCC_Shape.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_ShapeRobustTessellator::asiVisu_ShapeRobustTessellator() : IVtk_IShapeMesher()
{}

//-----------------------------------------------------------------------------

//! \return CAD model being tessellated.
TopoDS_Shape asiVisu_ShapeRobustTessellator::GetShape() const
{
  return IVtkOCC_Shape::Handle::DownCast(myShapeObj)->GetShape();
}

//-----------------------------------------------------------------------------

//! Builds the polygonal data.
void asiVisu_ShapeRobustTessellator::internalBuild()
{
  TopoDS_Shape master = this->GetShape();
  //
  if ( master.IsNull() )
    return;

  // Build map of all topological elements
  TopExp::MapShapes(master, m_all);

  // Build maps of topological entities
  TopExp::MapShapes(master, TopAbs_VERTEX, m_vertices);
  TopExp::MapShapes(master, TopAbs_EDGE,   m_edges);
  TopExp::MapShapes(master, TopAbs_FACE,   m_faces);

  // Build map of shapes and their parents
  TopTools_IndexedDataMapOfShapeListOfShape verticesOnEdges;
  TopExp::MapShapesAndAncestors(master, TopAbs_VERTEX, TopAbs_EDGE, verticesOnEdges);
  //
  TopTools_IndexedDataMapOfShapeListOfShape edgesOnFaces;
  TopExp::MapShapesAndAncestors(master, TopAbs_EDGE, TopAbs_FACE, edgesOnFaces);

  /* =========================================
   *  STAGE 1: fill data source with vertices
   * ========================================= */

  // Add vertices
  for ( int vidx = 1; vidx <= m_vertices.Extent(); ++vidx )
  {
    const TopoDS_Vertex&           v = TopoDS::Vertex( m_vertices(vidx) );
    const TopTools_ListOfShape edges = verticesOnEdges.FindFromKey(v);

    IVtk_MeshType type;
    if ( edges.IsEmpty() )
      type = MT_FreeVertex;
    else
      type = MT_SharedVertex;

    // Add vertex to the data source
    this->addVertex(v, vidx, type);
  }

  /* ======================================
   *  STAGE 2: fill data source with edges
   * ====================================== */

  // Add edges
  for ( int eidx = 1; eidx <= m_edges.Extent(); ++eidx )
  {
    const TopoDS_Edge&             e = TopoDS::Edge( m_edges(eidx) );
    const TopTools_ListOfShape faces = edgesOnFaces.FindFromKey(e);

    IVtk_MeshType type;
    if ( faces.IsEmpty() )
      type = MT_FreeEdge;
    else if ( faces.Extent() == 1 )
      type = MT_BoundaryEdge;
    else
      type = MT_SharedEdge;

    // Add edge to the data source
    this->addEdge(e, eidx, type);
  }

}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addVertex(const TopoDS_Vertex& vertex,
                                               const IVtk_IdType    shapeId,
                                               const IVtk_MeshType  meshType)
{
  if ( vertex.IsNull() )
    return;

  // Access host Cartesian point
  gp_Pnt pnt = BRep_Tool::Pnt(vertex);

  // Fill data structure
  IVtk_PointId pid = myShapeData->InsertCoordinate( pnt.X(), pnt.Y(), pnt.Z() );
  myShapeData->InsertVertex(shapeId, pid, meshType);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addEdge(const TopoDS_Edge&  edge,
                                             const IVtk_IdType   shapeId,
                                             const IVtk_MeshType meshType)
{
  if ( edge.IsNull() || BRep_Tool::Degenerated(edge) )
    return;

  // Use curve data source to provide the required tessellation
  vtkSmartPointer<asiVisu_CurveSource>
    curveSrc = vtkSmartPointer<asiVisu_CurveSource>::New();
  //
  if ( !curveSrc->SetInputEdge(edge) )
  {
#if defined COUT_DEBUG
    std::cout << "Error: cannot discretize edge" << std::endl;
#endif
    return;
  }

  // Get tessellation
  Handle(HRealArray) xCoords, yCoords, zCoords;
  asiVisu_Orientation ori;
  //
  curveSrc->GetInputArrays(xCoords, yCoords, zCoords, ori);

  if ( xCoords->Length() < 2 )
  {
#if defined COUT_DEBUG
    std::cout << "Error: poor edge discretization" << std::endl;
#endif
    return;
  }

  // Prepare a discrete representation for edge
  IVtk_PointIdList* pids = new IVtk_PointIdList();
  //
  for ( int j = xCoords->Lower(); j <= xCoords->Upper(); ++j )
  {
    IVtk_PointId pid = myShapeData->InsertCoordinate( xCoords->Value(j), yCoords->Value(j), zCoords->Value(j) );
    pids->Append(pid);
  }
  myShapeData->InsertLine(shapeId, pids, meshType);
  delete[] pids; pids = NULL;
}
