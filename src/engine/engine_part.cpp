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
  M->PartPartition()->AddNode(geom_n);

  // Initialize geometry
  geom_n->Init();
  geom_n->SetName("Part");

  // Create underlying face representation Node
  {
    Handle(ActAPI_INode) geom_face_base = geom_face_node::Instance();
    M->GeomFacePartition()->AddNode(geom_face_base);

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
    M->GeomSurfacePartition()->AddNode(geom_surf_base);

    // Initialize
    Handle(geom_surf_node) geom_surf_n = Handle(geom_surf_node)::DownCast(geom_surf_base);
    geom_surf_n->Init();
    geom_surf_n->SetName("Host surface");

    // Set as child
    geom_n->AddChildNode(geom_surf_n);
  }

  // Create underlying boundary edges representation Node
  {
    Handle(ActAPI_INode) geom_edges_base = geom_boundary_edges_node::Instance();
    M->GeomBoundaryEdgesPartition()->AddNode(geom_edges_base);

    // Initialize
    Handle(geom_boundary_edges_node) geom_edges_n = Handle(geom_boundary_edges_node)::DownCast(geom_edges_base);
    geom_edges_n->Init();
    geom_edges_n->SetName("Boundary");

    // Set as child
    geom_n->AddChildNode(geom_edges_n);
  }

  // Return the just created Node
  return geom_n;
}

//! Extracts sub-shape indices for the given collection of sub-shapes.
//! \param subShapes [in]  sub-shapes of interest.
//! \param indices   [out] their corresponding IDs.
void engine_part::GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                                     TColStd_PackedMapOfInteger&       indices)
{
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(common_facilities::Instance()->Model->PartNode()->GetShape(), M);

  for ( int i = 1; i <= subShapes.Extent(); ++i )
    indices.Add( M.FindIndex( subShapes.FindKey(i) ) );
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

  // Get Part Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();

  // Get Presentation for the Part Node
  Handle(visu_geom_prs)
    prs = Handle(visu_geom_prs)::DownCast( common_facilities::Instance()->Prs.Part->GetPresentation(N) );

  // Prepare list to satisfy the API of Presentation Manager
  Handle(ActAPI_HNodeList) dummyList = new ActAPI_HNodeList;
  dummyList->Append(N);

  // Prepare selection object
  visu_actor_elem_map selection;
  selection.Bind( prs->MainActor(), selected );

  // Set color
  double prevColor[3];
  QColor qcolor = gui_common::IntToColor(color);
  prs->GetPickPipeline()->Actor()->GetProperty()->GetColor( prevColor[0], prevColor[1], prevColor[2] );
  prs->GetPickPipeline()->Actor()->GetProperty()->SetColor( qcolor.redF(), qcolor.greenF(), qcolor.blueF() );

  // Highlight
  common_facilities::Instance()->Prs.Part->Highlight(dummyList, selection, SelectionMode_Face);
}

//! Retrieves highlighted sub-shapes from the viewer.
//! \param subShapes [out] result collection.
void engine_part::GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes)
{
  // Get Part Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->PartNode();

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
    const TColStd_PackedMapOfInteger& face_mask = it.Value();
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger mit(face_mask); mit.More(); mit.Next() )
    {
      const int face_idx = mit.Key();
      const TopoDS_Face& F = TopoDS::Face( M.FindKey(face_idx) );
      subShapes.Add(F);
    }
  }
}