//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_domain.h>

// A-Situs (common) includes
#include <common_facilities.h>
#include <common_model.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_SequenceOfShape.hxx>

//! Retrieves highlighted edges from the Face Domain viewer.
//! \param edges [out] result collection.
void engine_domain::GetHighlightedEdges(TopTools_IndexedMapOfShape& edges)
{
  TopoDS_Face dummyFace;
  GetHighlightedEdges(edges, dummyFace);
}

//! Retrieves highlighted edges from the Face Domain viewer.
//! \param edges [out] result collection.
//! \param face  [out] base face.
void engine_domain::GetHighlightedEdges(TopTools_IndexedMapOfShape& edges,
                                        TopoDS_Face&                face)
{
  // Get Part Node
  Handle(geom_part_node) N = common_facilities::Instance()->Model->GetPartNode();

  // Get Part shape
  TopoDS_Shape part = N->GetShape();

  // Build a map of shapes
  TopTools_IndexedMapOfShape FacesMap;
  TopExp::MapShapes(N->GetShape(), FacesMap);

  // Get face
  const int face_idx = N->GetFaceRepresentation()->GetSelectedFace();
  //
  if ( face_idx > 0 )
    face = TopoDS::Face( FacesMap.FindKey(face_idx) );
  //
  if ( face.IsNull() )
    return;

  TopTools_SequenceOfShape faceEdges;
  for ( TopExp_Explorer eexp(face, TopAbs_EDGE); eexp.More(); eexp.Next() )
  {
    faceEdges.Append( eexp.Current() );
  }

  // Get actual selection
  const visu_actual_selection& sel      = common_facilities::Instance()->Prs.Domain->GetCurrentSelection();
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
      edges.Add( faceEdges( mit.Key() ) );
    }
  }
}
