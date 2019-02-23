//-----------------------------------------------------------------------------
// Created on: 30 March 2016
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
#include <asiEngine_Domain.h>

// Visualization includes
#include <asiVisu_Prs.h>
#include <asiVisu_Selection.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_SequenceOfShape.hxx>

//! Retrieves highlighted edges from the Face Domain viewer.
//! \param partNode [in]  part Node playing as a source of geometry.
//! \param prsMgr   [in]  presentation manager.
//! \param edges    [out] result collection.
void asiEngine_Domain::GetHighlightedEdges(const Handle(asiData_PartNode)&            partNode,
                                           const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                           TopTools_IndexedMapOfShape&                edges)
{
  TopoDS_Face dummyFace;
  GetHighlightedEdges(partNode, prsMgr, edges, dummyFace);
}

//! Retrieves highlighted edges from the Face Domain viewer.
//! \param partNode [in]  part Node playing as a source of geometry.
//! \param prsMgr   [in]  presentation manager.
//! \param edges    [out] result collection.
//! \param face     [out] base face.
void asiEngine_Domain::GetHighlightedEdges(const Handle(asiData_PartNode)&            partNode,
                                           const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                           TopTools_IndexedMapOfShape&                edges,
                                           TopoDS_Face&                               face)
{
  asiEngine_NotUsed(partNode);
  asiEngine_NotUsed(prsMgr);
  asiEngine_NotUsed(edges);
  asiEngine_NotUsed(face);

  //// Get Part shape
  //TopoDS_Shape part = partNode->GetShape();

  //// Build a map of shapes
  //const TopTools_IndexedMapOfShape& SubShapesMap = partNode->GetAAG()->GetMapOfSubShapes();

  //// Get face
  //const int face_idx = partNode->GetFaceRepresentation()->GetSelectedFace();
  ////
  //if ( face_idx > 0 )
  //  face = TopoDS::Face( SubShapesMap.FindKey(face_idx) );
  ////
  //if ( face.IsNull() )
  //  return;

  //TopTools_SequenceOfShape faceEdges;
  //for ( TopExp_Explorer eexp(face, TopAbs_EDGE); eexp.More(); eexp.Next() )
  //{
  //  faceEdges.Append( eexp.Current() );
  //}

  //// Get actual selection
  //const asiVisu_ActualSelection& sel      = prsMgr->GetCurrentSelection();
  //const Handle(asiVisu_PickerResult)&      pick_res = sel.PickResult(SelectionNature_Pick);
  //const asiVisu_ActorElemMap&    elem_map = pick_res.GetPickMap();
  ////
  //// Prepare cumulative set of all picked element IDs
  //for ( asiVisu_ActorElemMap::Iterator it(elem_map); it.More(); it.Next() )
  //{
  //  const TColStd_PackedMapOfInteger& face_mask = it.Value();
  //  //
  //  for ( TColStd_MapIteratorOfPackedMapOfInteger mit(face_mask); mit.More(); mit.Next() )
  //  {
  //    edges.Add( faceEdges( mit.Key() ) );
  //  }
  //}
}
