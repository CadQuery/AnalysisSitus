//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_part.h>

// A-Situs (common) includes
#include <common_facilities.h>
#include <common_model.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (visualization) includes
#include <visu_geom_prs.h>

// VTK includes
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

//! \return newly created Part Node.
Handle(geom_part_node) engine_part::Create_Part()
{
  //---------------------------------------------------------------------------
  // Create persistent object
  //---------------------------------------------------------------------------

  // Access Model
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Add Part Node to Partition
  Handle(geom_part_node) geom_n = Handle(geom_part_node)::DownCast( geom_part_node::Instance() );
  M->GetPartPartition()->AddNode(geom_n);

  // Initialize geometry
  geom_n->Init();
  geom_n->SetName("Part");

  // Create underlying face representation Node
  {
    Handle(ActAPI_INode) geom_face_base = geom_face_node::Instance();
    M->GetGeomFacePartition()->AddNode(geom_face_base);

    // Initialize
    Handle(geom_face_node) geom_face_n = Handle(geom_face_node)::DownCast(geom_face_base);
    geom_face_n->Init();
    geom_face_n->SetName("Face domain");

    // Set as child
    geom_n->AddChildNode(geom_face_n);
  }

  // Create underlying surface representation Node
  {
    Handle(ActAPI_INode) geom_surf_base = geom_surf_node::Instance();
    M->GetGeomSurfacePartition()->AddNode(geom_surf_base);

    // Initialize
    Handle(geom_surf_node) geom_surf_n = Handle(geom_surf_node)::DownCast(geom_surf_base);
    geom_surf_n->Init();
    geom_surf_n->SetName("Host surface");

    // Set as child
    geom_n->AddChildNode(geom_surf_n);
  }

  // Create underlying edge representation Node
  {
    Handle(ActAPI_INode) geom_edge_base = geom_edge_node::Instance();
    M->GetGeomEdgePartition()->AddNode(geom_edge_base);

    // Initialize
    Handle(geom_edge_node) geom_edge_n = Handle(geom_edge_node)::DownCast(geom_edge_base);
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
    Handle(ActAPI_INode) geom_edges_base = geom_boundary_edges_node::Instance();
    M->GetGeomBoundaryEdgesPartition()->AddNode(geom_edges_base);

    // Initialize
    Handle(geom_boundary_edges_node) geom_edges_n = Handle(geom_boundary_edges_node)::DownCast(geom_edges_base);
    geom_edges_n->Init();
    geom_edges_n->SetName("Boundary");

    // Set as child
    geom_n->AddChildNode(geom_edges_n);
  }

  // Create underlying contour Node
  {
    Handle(ActAPI_INode) geom_contour_base = geom_contour_node::Instance();
    M->GetGeomContourPartition()->AddNode(geom_contour_base);

    // Initialize
    Handle(geom_contour_node) geom_contour_n = Handle(geom_contour_node)::DownCast(geom_contour_base);
    geom_contour_n->Init();
    geom_contour_n->SetName("Contour");

    // Set as child
    geom_n->AddChildNode(geom_contour_n);
  }

  // Return the just created Node
  return geom_n;
}

//! Cleans up Data Model structure related to the Part Node.
void engine_part::Clean_Part()
{
  // Access Model
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Get Part Node
  Handle(geom_part_node) part_n = M->GetPartNode();
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
void engine_part::GetSubShapeIndicesByFaceIndices(const TColStd_PackedMapOfInteger& faceIndices,
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
void engine_part::GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                                     TColStd_PackedMapOfInteger&       indices)
{
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(common_facilities::Instance()->Model->GetPartNode()->GetShape(), M);

  for ( int i = 1; i <= subShapes.Extent(); ++i )
    indices.Add( M.FindIndex( subShapes.FindKey(i) ) );
}

//! Highlights the passed sub-shapes identified by their indices.
//! \param subShapeIndices [in] indices of the sub-shapes to highlight.
void engine_part::HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices)
{
  double pick_color[3];
  visu_utils::DefaultPickingColor(pick_color[0], pick_color[1], pick_color[2]);
  QColor color;
  color.setRedF   (pick_color[0]);
  color.setGreenF (pick_color[1]);
  color.setBlueF  (pick_color[2]);
  //
  HighlightSubShapes( subShapeIndices, gui_common::ColorToInt(color) );
}

//! Highlights the passed sub-shapes identified by their indices.
//! \param subShapeIndices [in] indices of the sub-shapes to highlight.
//! \param color           [in] highlighting color.
void engine_part::HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices,
                                     const int                         color)
{
  // Get Part Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->GetPartNode();

  // Get Presentation for the Part Node
  Handle(visu_geom_prs)
    prs = Handle(visu_geom_prs)::DownCast( common_facilities::Instance()->Prs.Part->GetPresentation(N) );

  // Prepare list to satisfy the API of Presentation Manager
  Handle(ActAPI_HNodeList) dummyList = new ActAPI_HNodeList;
  dummyList->Append(N);

  // Prepare selection object
  visu_actor_elem_map selection;
  selection.Bind( prs->MainActor(), subShapeIndices );

  // Set color
  double prevColor[3];
  QColor qcolor = gui_common::IntToColor(color);
  prs->GetPickPipeline()->Actor()->GetProperty()->GetColor( prevColor[0], prevColor[1], prevColor[2] );
  prs->GetPickPipeline()->Actor()->GetProperty()->SetColor( qcolor.redF(), qcolor.greenF(), qcolor.blueF() );

  // Highlight
  common_facilities::Instance()->Prs.Part->Highlight(dummyList, selection, SelectionMode_Face);
}

//! Highlights the passed sub-shapes in Part Viewer.
//! \param subShapes [in] sub-shapes to highlight.
void engine_part::HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes)
{
  double pick_color[3];
  visu_utils::DefaultPickingColor(pick_color[0], pick_color[1], pick_color[2]);
  QColor color;
  color.setRedF   (pick_color[0]);
  color.setGreenF (pick_color[1]);
  color.setBlueF  (pick_color[2]);
  //
  HighlightSubShapes( subShapes, gui_common::ColorToInt(color) );
}

//! Highlights the passed sub-shapes in Part Viewer.
//! \param subShapes [in] sub-shapes to highlight.
//! \param color     [in] highlighting color.
void engine_part::HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes,
                                     const int                         color)
{
  // Get global relative indices of the sub-shapes in the CAD model
  TColStd_PackedMapOfInteger selected;
  engine_part::GetSubShapeIndices(subShapes, selected);

  // Highlight
  HighlightSubShapes(selected, color);
}

//! Retrieves highlighted sub-shapes from the viewer.
//! \param subShapes [out] result collection.
void engine_part::GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes)
{
  // Get Part Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->GetPartNode();

  // Get Part shape
  TopoDS_Shape part = N->GetShape();

  // Map ALL shapes to extract topology by selected index which is global
  // (related to full accessory graph)
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(part, M);

  // Get actual selection
  const visu_actual_selection& sel      = common_facilities::Instance()->Prs.Part->GetCurrentSelection();
  const visu_pick_result&      pick_res = sel.PickResult(SelectionNature_Pick);
  const visu_actor_elem_map&   elem_map = pick_res.GetPickMap();
  //
  // Prepare cumulative set of all picked element IDs
  for ( visu_actor_elem_map::Iterator it(elem_map); it.More(); it.Next() )
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
void engine_part::GetHighlightedFaces(TColStd_PackedMapOfInteger& faceIndices)
{
  TopTools_IndexedMapOfShape subShapes;
  GetHighlightedSubShapes(subShapes);

  // Take all faces
  Handle(geom_part_node) N = common_facilities::Instance()->Model->GetPartNode();
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
void engine_part::GetHighlightedEdges(TColStd_PackedMapOfInteger& edgeIndices)
{
  TopTools_IndexedMapOfShape subShapes;
  GetHighlightedSubShapes(subShapes);

  // Take all edges
  Handle(geom_part_node) N = common_facilities::Instance()->Model->GetPartNode();
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
