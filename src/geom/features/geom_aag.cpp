//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_aag.h>

// Visualization includes
#include <visu_topo_graph_item.h>

// Geometry includes
#include <geom_utils.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

// VTK includes
#include <vtkNew.h>
#include <vtkStringArray.h>

//! Initializes AAG from the given master model and selected faces.
//! \param masterCAD     [in] master model (full CAD).
//! \param selectedFaces [in] selected faces.
geom_aag::geom_aag(const TopoDS_Shape&         masterCAD,
                   const TopTools_ListOfShape& selectedFaces)
{
  this->init( masterCAD, selectedFaces );
}

//! Constructor accepting master CAD only.
//! \param masterCAD [in] master CAD.
geom_aag::geom_aag(const TopoDS_Shape& masterCAD)
{
  this->init( masterCAD, TopTools_ListOfShape() );
}

//! Destructor.
geom_aag::~geom_aag()
{}

//! Converts AAG to VTK presentable form.
//! \return VTK graph.
vtkSmartPointer<vtkMutableUndirectedGraph> geom_aag::ToVTK() const
{
  vtkSmartPointer<vtkMutableUndirectedGraph>
    result = vtkSmartPointer<vtkMutableUndirectedGraph>::New();

  // Array for vertex groups
  vtkNew<vtkStringArray> groupArr;
  groupArr->SetNumberOfComponents(1);
  groupArr->SetName(ARRNAME_GROUP);

  // Array for vertex labels
  vtkNew<vtkStringArray> labelArr;
  labelArr->SetNumberOfComponents(1);
  labelArr->SetName(ARRNAME_LABELS);

  // Add vertices for faces
  NCollection_DataMap<int, vtkIdType> FaceVertexMap;
  for ( int f = 1; f <= m_faces.Extent(); ++f )
  {
    const vtkIdType vertex_id = result->AddVertex();
    FaceVertexMap.Bind(f, vertex_id);

    // Fill property arrays
    labelArr->InsertNextValue( geom_utils::FaceGeometryName( TopoDS::Face( m_faces(f) ) ).c_str() );
    //
    if ( m_selected.Contains(f) )
      groupArr->InsertNextValue(ARRNAME_GROUP_ADJACENT);
    else
      groupArr->InsertNextValue(ARRNAME_GROUP_ORDINARY);
  }

  // Set property arrays
  result->GetVertexData()->AddArray( labelArr.GetPointer() );
  result->GetVertexData()->AddArray( groupArr.GetPointer() );

  // Add links for adjacency relations
  for ( NCollection_DataMap<int, TColStd_PackedMapOfInteger>::Iterator it(m_neighbors); it.More(); it.Next() )
  {
    const int       f_idx = it.Key();
    const vtkIdType v_idx = FaceVertexMap(f_idx);

    // Get face neighbors
    const TColStd_PackedMapOfInteger& neighbors = it.Value();

    // Add links for each neighbor
    for ( TColStd_MapIteratorOfPackedMapOfInteger mit(neighbors); mit.More(); mit.Next() )
    {
      const int       neighbor_f_idx = mit.Key();
      const vtkIdType neighbor_v_idx = FaceVertexMap(neighbor_f_idx);

      // Add link
      result->AddEdge(v_idx, neighbor_v_idx);
    }
  }

  return result;
}

//! Returns topological face by its internal index (e.g. coming from iterator).
//! \param face_idx [in] face index.
//! \return topological face.
const TopoDS_Face& geom_aag::GetFace(const int face_idx) const
{
  return TopoDS::Face( m_faces.FindKey(face_idx) );
}

//! Returns neighbors for the face having the given its internal index.
//! \param face_idx [in] face index.
//! \return indices of the neighbor faces.
const TColStd_PackedMapOfInteger& geom_aag::GetNeighbors(const int face_idx) const
{
  return m_neighbors.Find(face_idx);
}

//-----------------------------------------------------------------------------

//! Initializes graph tool with master CAD and selected faces.
//! \param masterCAD     [in] master model (full CAD).
//! \param selectedFaces [in] selected faces.
void geom_aag::init(const TopoDS_Shape&         masterCAD,
                    const TopTools_ListOfShape& selectedFaces)
{
  //---------------------------------------------------------------------------

  // Extract all faces with unique indexes from the master CAD.
  TopExp::MapShapes(masterCAD, TopAbs_FACE, m_faces);

  // Fill adjacency map with empty buckets
  for ( int f = 1; f <= m_faces.Extent(); ++f )
  {
    m_neighbors.Bind( f, TColStd_PackedMapOfInteger() );
  }

  //---------------------------------------------------------------------------

  TopTools_IndexedDataMapOfShapeListOfShape ChildParentMap;
  TopExp::MapShapesAndAncestors(masterCAD, TopAbs_EDGE, TopAbs_FACE, ChildParentMap);

  // Build adjacency graph
  for ( TopExp_Explorer exp(masterCAD, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopTools_ListOfShape& adjacentFaces = ChildParentMap.FindFromKey( exp.Current() );
    this->addMates(adjacentFaces);
  }

  //---------------------------------------------------------------------------

  // Save selected faces for future filtering
  for ( TopTools_ListIteratorOfListOfShape lit(selectedFaces); lit.More(); lit.Next() )
  {
    m_selected.Add( m_faces.FindIndex( lit.Value() ) );
  }
}

//! Fills graph with nodes for mate faces.
//! \param mateFaces [in] faces to add (if not yet added).
void geom_aag::addMates(const TopTools_ListOfShape& mateFaces)
{
  for ( TopTools_ListIteratorOfListOfShape lit(mateFaces); lit.More(); lit.Next() )
  {
    const int                   face_idx   = m_faces.FindIndex( lit.Value() );
    TColStd_PackedMapOfInteger& face_links = m_neighbors.ChangeFind(face_idx);

    // Add all the rest faces as neighbors
    for ( TopTools_ListIteratorOfListOfShape lit2(mateFaces); lit2.More(); lit2.Next() )
    {
      const int linked_face_idx = m_faces.FindIndex( lit2.Value() );
      if ( linked_face_idx == face_idx )
        continue; // Skip the same index to avoid loop arcs in the graph

      face_links.Add(linked_face_idx);
    }
  }
}
