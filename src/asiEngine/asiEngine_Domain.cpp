//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiEngine_Domain.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_SequenceOfShape.hxx>

//! Retrieves highlighted edges from the Face Domain viewer.
//! \param edges [out] result collection.
void asiEngine_Domain::GetHighlightedEdges(TopTools_IndexedMapOfShape& edges)
{
  TopoDS_Face dummyFace;
  GetHighlightedEdges(edges, dummyFace);
}

//! Retrieves highlighted edges from the Face Domain viewer.
//! \param partNode [in]  part Node playing as a source of geometry.
//! \param edges    [out] result collection.
//! \param face     [out] base face.
void asiEngine_Domain::GetHighlightedEdges(const Handle(asiData_PartNode)& partNode,
                                           TopTools_IndexedMapOfShape&     edges,
                                           TopoDS_Face&                    face)
{
  // Get Part shape
  TopoDS_Shape part = partNode->GetShape();

  // Build a map of shapes
  TopTools_IndexedMapOfShape FacesMap;
  TopExp::MapShapes(part, FacesMap);

  // Get face
  const int face_idx = partNode->GetFaceRepresentation()->GetSelectedFace();
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
  const asiVisu_PickResult&      pick_res = sel.PickResult(SelectionNature_Pick);
  const asiVisu_ActorElemMap&   elem_map = pick_res.GetPickMap();
  //
  // Prepare cumulative set of all picked element IDs
  for ( asiVisu_ActorElemMap::Iterator it(elem_map); it.More(); it.Next() )
  {
    const TColStd_PackedMapOfInteger& face_mask = it.Value();
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger mit(face_mask); mit.More(); mit.Next() )
    {
      edges.Add( faceEdges( mit.Key() ) );
    }
  }
}
