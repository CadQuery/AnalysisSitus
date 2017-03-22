//-----------------------------------------------------------------------------
// Created on: 30 November 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapeRobustTessellator.h>

// VTK includes
#include <asiVisu_CurveSource.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! ctor.
//! \param aag [in] AAG.
asiVisu_ShapeRobustTessellator::asiVisu_ShapeRobustTessellator(const Handle(asiAlgo_AAG)& aag)
: Standard_Transient(), m_aag(aag)
{
  m_data = new asiVisu_ShapeData; // Allocate data for visualization primitives
                                  // (i.e. VTK points, cells, arrays)
}

//-----------------------------------------------------------------------------

//! Builds the polygonal data.
void asiVisu_ShapeRobustTessellator::Build()
{
  this->internalBuild();
}

//-----------------------------------------------------------------------------

//! Internal method which builds the polygonal data.
void asiVisu_ShapeRobustTessellator::internalBuild()
{
  TopoDS_Shape master = m_aag->GetMasterCAD();
  //
  if ( master.IsNull() )
    return;

  // Build map of shapes and their parents
  TopTools_IndexedDataMapOfShapeListOfShape verticesOnEdges;
  TopExp::MapShapesAndAncestors(master, TopAbs_VERTEX, TopAbs_EDGE, verticesOnEdges);
  //
  TopTools_IndexedDataMapOfShapeListOfShape edgesOnFaces;
  TopExp::MapShapesAndAncestors(master, TopAbs_EDGE, TopAbs_FACE, edgesOnFaces);

  // Cached maps
  const TopTools_IndexedMapOfShape& allVertices = m_aag->GetMapOfVertices();
  const TopTools_IndexedMapOfShape& allEdges    = m_aag->GetMapOfEdges();
  const TopTools_IndexedMapOfShape& allFaces    = m_aag->GetMapOfFaces();

  /* =========================================
   *  STAGE 1: fill data source with vertices
   * ========================================= */

  // Add vertices
  for ( int vidx = 1; vidx <= allVertices.Extent(); ++vidx )
  {
    const TopoDS_Vertex&            v = TopoDS::Vertex( allVertices(vidx) );
    const TopTools_ListOfShape& edges = verticesOnEdges.FindFromKey(v);

    asiVisu_ShapeCellType type;
    if ( edges.IsEmpty() )
      type = ShapeCellType_FreeVertex;
    else
      type = ShapeCellType_SharedVertex;

    // Add vertex to the data source
    this->addVertex(v, vidx, type);
  }

  /* ======================================
   *  STAGE 2: fill data source with edges
   * ====================================== */

  // Add edges
  for ( int eidx = 1; eidx <= allEdges.Extent(); ++eidx )
  {
    const TopoDS_Edge&              e = TopoDS::Edge( allEdges(eidx) );
    const TopTools_ListOfShape& faces = edgesOnFaces.FindFromKey(e);

    asiVisu_ShapeCellType type;
    if ( faces.IsEmpty() )
      type = ShapeCellType_FreeEdge;
    else if ( faces.Extent() == 1 )
      type = ShapeCellType_BorderEdge;
    else
      type = ShapeCellType_SharedEdge;

    // Add edge to the data source
    this->addEdge(e, eidx, type);
  }
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustTessellator::addVertex(const TopoDS_Vertex&        vertex,
                                               const vtkIdType             shapeId,
                                               const asiVisu_ShapeCellType scType)
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

void asiVisu_ShapeRobustTessellator::addEdge(const TopoDS_Edge&          edge,
                                             const vtkIdType             shapeId,
                                             const asiVisu_ShapeCellType scType)
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
  vtkSmartPointer<vtkIdList> pids = vtkSmartPointer<vtkIdList>::New();
  //
  for ( int j = xCoords->Lower(); j <= xCoords->Upper(); ++j )
  {
    IVtk_PointId pid = m_data->InsertCoordinate( xCoords->Value(j),
                                                 yCoords->Value(j),
                                                 zCoords->Value(j) );
    pids->InsertNextId(pid);
  }
  m_data->InsertPolyline(shapeId, pids, scType);
}
