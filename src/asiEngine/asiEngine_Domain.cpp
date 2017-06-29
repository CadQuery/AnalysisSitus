//-----------------------------------------------------------------------------
// Created on: 30 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
  //const asiVisu_PickResult&      pick_res = sel.PickResult(SelectionNature_Pick);
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
