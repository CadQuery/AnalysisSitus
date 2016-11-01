//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiEngine_Part.h>

// A-Situs (common) includes
#include <common_facilities.h>
#include <asiData_Model.h>

// A-Situs (GUI) includes
#include <asiUI_Common.h>

// A-Situs (visualization) includes
#include <asiVisu_GeomPrs.h>

// VTK includes
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

//! \return newly created Part Node.
Handle(asiData_PartNode) asiEngine_Part::Create_Part()
{
  //---------------------------------------------------------------------------
  // Create persistent object
  //---------------------------------------------------------------------------

  // Access Model
  Handle(asiData_Model) M = common_facilities::Instance()->Model;

  // Add Part Node to Partition
  Handle(asiData_PartNode) geom_n = Handle(asiData_PartNode)::DownCast( asiData_PartNode::Instance() );
  M->GetPartPartition()->AddNode(geom_n);

  // Initialize geometry
  geom_n->Init();
  geom_n->SetName("Part");

  // Create underlying face representation Node
  {
    Handle(ActAPI_INode) geom_face_base = asiData_FaceNode::Instance();
    M->GetGeomFacePartition()->AddNode(geom_face_base);

    // Initialize
    Handle(asiData_FaceNode) geom_face_n = Handle(asiData_FaceNode)::DownCast(geom_face_base);
    geom_face_n->Init();
    geom_face_n->SetName("Face domain");

    // Set as child
    geom_n->AddChildNode(geom_face_n);
  }

  // Create underlying surface representation Node
  {
    Handle(ActAPI_INode) geom_surf_base = asiData_SurfNode::Instance();
    M->GetGeomSurfacePartition()->AddNode(geom_surf_base);

    // Initialize
    Handle(asiData_SurfNode) geom_surf_n = Handle(asiData_SurfNode)::DownCast(geom_surf_base);
    geom_surf_n->Init();
    geom_surf_n->SetName("Host surface");

    // Set as child
    geom_n->AddChildNode(geom_surf_n);
  }

  // Create underlying edge representation Node
  {
    Handle(ActAPI_INode) geom_edge_base = asiData_EdgeNode::Instance();
    M->GetGeomEdgePartition()->AddNode(geom_edge_base);

    // Initialize
    Handle(asiData_EdgeNode) geom_edge_n = Handle(asiData_EdgeNode)::DownCast(geom_edge_base);
    geom_edge_n->Init();
    geom_edge_n->SetName("Edge domain");

    // Set as child
    geom_n->AddChildNode(geom_edge_n);
  }

  // Create underlying curve representation Node
  {
    Handle(ActAPI_INode) geom_curve_base = geom_curve_node::Instance();
    M->GetGeomCurvePartition()->AddNode(geom_curve_base);

    // Initialize
    Handle(geom_curve_node) geom_curve_n = Handle(geom_curve_node)::DownCast(geom_curve_base);
    geom_curve_n->Init();
    geom_curve_n->SetName("Host curve");

    // Set as child
    geom_n->AddChildNode(geom_curve_n);
  }

  // Create underlying boundary edges representation Node
  {
    Handle(ActAPI_INode) geom_edges_base = asiData_BoundaryEdgesNode::Instance();
    M->GetGeomBoundaryEdgesPartition()->AddNode(geom_edges_base);

    // Initialize
    Handle(asiData_BoundaryEdgesNode) geom_edges_n = Handle(asiData_BoundaryEdgesNode)::DownCast(geom_edges_base);
    geom_edges_n->Init();
    geom_edges_n->SetName("Boundary");

    // Set as child
    geom_n->AddChildNode(geom_edges_n);
  }

  // Create underlying contour Node
  {
    Handle(ActAPI_INode) geom_contour_base = asiData_ContourNode::Instance();
    M->GetGeomContourPartition()->AddNode(geom_contour_base);

    // Initialize
    Handle(asiData_ContourNode) geom_contour_n = Handle(asiData_ContourNode)::DownCast(geom_contour_base);
    geom_contour_n->Init();
    geom_contour_n->SetName("Contour");

    // Set as child
    geom_n->AddChildNode(geom_contour_n);
  }

  // Return the just created Node
  return geom_n;
}

//! Cleans up Data Model structure related to the Part Node.
void asiEngine_Part::Clean_Part()
{
  // Access Model
  Handle(asiData_Model) M = common_facilities::Instance()->Model;

  // Get Part Node
  Handle(asiData_PartNode) part_n = M->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return;

  // Reset data
  part_n->Init();
  part_n->GetFaceRepresentation()->Init();
  part_n->GetSurfaceRepresentation()->Init();
  part_n->GetEdgeRepresentation()->Init();
  part_n->GetCurveRepresentation()->Init();
  part_n->GetBoundaryEdgesRepresentation()->Init();
}

//! Extracts sub-shape indices for the given collection of face indices.
//! \param faceIndices [in]  indices of faces.
//! \param indices     [out] their corresponding indices among all sub-shapes.
void asiEngine_Part::GetSubShapeIndicesByFaceIndices(const TColStd_PackedMapOfInteger& faceIndices,
                                                  TColStd_PackedMapOfInteger&       indices)
{
  TopTools_IndexedMapOfShape AllFaces, SelectedFaces;
  TopExp::MapShapes(common_facilities::Instance()->Model->GetPartNode()->GetShape(),
                    TopAbs_FACE, AllFaces);

  // Get selected faces in topological form
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(faceIndices); fit.More(); fit.Next() )
  {
    const int input_face_idx = fit.Key();
    SelectedFaces.Add( AllFaces.FindKey(input_face_idx) );
  }

  // Get indices of the faces among all sub-shapes
  GetSubShapeIndices(SelectedFaces, indices);
}

//! Extracts sub-shape indices for the given collection of sub-shapes.
//! \param subShapes [in]  sub-shapes of interest.
//! \param indices   [out] their corresponding IDs.
void asiEngine_Part::GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                                     TColStd_PackedMapOfInteger&       indices)
{
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(common_facilities::Instance()->Model->GetPartNode()->GetShape(), M);

  for ( int i = 1; i <= subShapes.Extent(); ++i )
    indices.Add( M.FindIndex( subShapes.FindKey(i) ) );
}

//! Highlights the passed sub-shapes identified by their indices.
//! \param subShapeIndices [in] indices of the sub-shapes to highlight.
void asiEngine_Part::HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices)
{
  double pick_color[3];
  asiVisu_Utils::DefaultPickingColor(pick_color[0], pick_color[1], pick_color[2]);
  QColor color;
  color.setRedF   (pick_color[0]);
  color.setGreenF (pick_color[1]);
  color.setBlueF  (pick_color[2]);
  //
  HighlightSubShapes( subShapeIndices, asiUI_Common::ColorToInt(color) );
}

//! Highlights the passed sub-shapes identified by their indices.
//! \param subShapeIndices [in] indices of the sub-shapes to highlight.
//! \param color           [in] highlighting color.
void asiEngine_Part::HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices,
                                     const int                         color)
{
  // Get Part Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();

  // Get Presentation for the Part Node
  Handle(asiVisu_GeomPrs)
    prs = Handle(asiVisu_GeomPrs)::DownCast( common_facilities::Instance()->Prs.Part->GetPresentation(N) );

  // Prepare list to satisfy the API of Presentation Manager
  Handle(ActAPI_HNodeList) dummyList = new ActAPI_HNodeList;
  dummyList->Append(N);

  // Prepare selection object
  asiVisu_ActorElemMap selection;
  selection.Bind( prs->MainActor(), subShapeIndices );

  // Set color
  double prevColor[3];
  QColor qcolor = asiUI_Common::IntToColor(color);
  prs->GetPickPipeline()->Actor()->GetProperty()->GetColor( prevColor[0], prevColor[1], prevColor[2] );
  prs->GetPickPipeline()->Actor()->GetProperty()->SetColor( qcolor.redF(), qcolor.greenF(), qcolor.blueF() );

  // Highlight
  common_facilities::Instance()->Prs.Part->Highlight(dummyList, selection, SelectionMode_Face);
}

//! Highlights the passed sub-shapes in Part Viewer.
//! \param subShapes [in] sub-shapes to highlight.
void asiEngine_Part::HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes)
{
  double pick_color[3];
  asiVisu_Utils::DefaultPickingColor(pick_color[0], pick_color[1], pick_color[2]);
  QColor color;
  color.setRedF   (pick_color[0]);
  color.setGreenF (pick_color[1]);
  color.setBlueF  (pick_color[2]);
  //
  HighlightSubShapes( subShapes, asiUI_Common::ColorToInt(color) );
}

//! Highlights the passed sub-shapes in Part Viewer.
//! \param subShapes [in] sub-shapes to highlight.
//! \param color     [in] highlighting color.
void asiEngine_Part::HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes,
                                     const int                         color)
{
  // Get global relative indices of the sub-shapes in the CAD model
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part::GetSubShapeIndices(subShapes, selected);

  // Highlight
  HighlightSubShapes(selected, color);
}

//! Retrieves highlighted sub-shapes from the viewer.
//! \param subShapes [out] result collection.
void asiEngine_Part::GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes)
{
  // Get Part Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();

  // Get Part shape
  TopoDS_Shape part = N->GetShape();

  // Map ALL shapes to extract topology by selected index which is global
  // (related to full accessory graph)
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(part, M);

  // Get actual selection
  const visu_actual_selection& sel      = common_facilities::Instance()->Prs.Part->GetCurrentSelection();
  const asiUI_PickResult&      pick_res = sel.PickResult(SelectionNature_Pick);
  const asiVisu_ActorElemMap&   elem_map = pick_res.GetPickMap();
  //
  // Prepare cumulative set of all picked element IDs
  for ( asiVisu_ActorElemMap::Iterator it(elem_map); it.More(); it.Next() )
  {
    const TColStd_PackedMapOfInteger& subshape_mask = it.Value();
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger mit(subshape_mask); mit.More(); mit.Next() )
    {
      const int           subshape_idx = mit.Key();
      const TopoDS_Shape& F            = M.FindKey(subshape_idx);
      subShapes.Add(F);
    }
  }
}

//! Retrieves indices of the highlighted faces.
//! \param faceIndices [out] indices of the highlighted faces.
void asiEngine_Part::GetHighlightedFaces(TColStd_PackedMapOfInteger& faceIndices)
{
  TopTools_IndexedMapOfShape subShapes;
  GetHighlightedSubShapes(subShapes);

  // Take all faces
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  //
  TopTools_IndexedMapOfShape allFaces;
  TopExp::MapShapes( N->GetShape(), TopAbs_FACE, allFaces);

  // Filter out non-selected faces
  for ( int f = 1; f <= allFaces.Extent(); ++f )
  {
    if ( subShapes.Contains( allFaces(f) ) )
      faceIndices.Add(f);
  }
}

//! Retrieves indices of the highlighted edges.
//! \param edgeIndices [out] indices of the highlighted edges.
void asiEngine_Part::GetHighlightedEdges(TColStd_PackedMapOfInteger& edgeIndices)
{
  TopTools_IndexedMapOfShape subShapes;
  GetHighlightedSubShapes(subShapes);

  // Take all edges
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  //
  TopTools_IndexedMapOfShape allEdges;
  TopExp::MapShapes( N->GetShape(), TopAbs_EDGE, allEdges);

  // Filter out non-selected edges
  for ( int e = 1; e <= allEdges.Extent(); ++e )
  {
    if ( subShapes.Contains( allEdges(e) ) )
      edgeIndices.Add(e);
  }
}
