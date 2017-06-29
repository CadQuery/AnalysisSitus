//-----------------------------------------------------------------------------
// Created on: 17 May 2017
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
#include <asiAlgo_FindFree.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//-----------------------------------------------------------------------------

void asiAlgo_FindFree::FindEdges(const TopoDS_Shape&         shape,
                                 TopTools_IndexedMapOfShape& freeEdges,
                                 ActAPI_ProgressEntry        Journal)
{
  Journal.SendLogMessage( LogInfo(Normal) << "Checking free edges..." );

  // Get child-parent relationships
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, M);

  // Check for every edge the number of its owning faces
  const int nEdges = M.Extent();
  for ( int e = 1; e <= nEdges; ++e )
  {
    const TopoDS_Edge& E = TopoDS::Edge( M.FindKey(e) );
    //
    if ( BRep_Tool::Degenerated(E) )
      continue;

    const int nOwningFaces = M.FindFromIndex(e).Extent();
    if ( nOwningFaces == 1 )
      freeEdges.Add(E);
  }
}

//-----------------------------------------------------------------------------
