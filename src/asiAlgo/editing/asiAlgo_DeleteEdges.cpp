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
#include <asiAlgo_DeleteEdges.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <TopExp_Explorer.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
asiAlgo_DeleteEdges::asiAlgo_DeleteEdges(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Removes the given edges from the master model.
//! \param edges    [in] edges to delete.
//! \param edgeOnly [in] indicates whether to delete edge only.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_DeleteEdges::Perform(const TopTools_IndexedMapOfShape& edges,
                                  const bool                        edgeOnly)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;
  //
  for ( int e = 1; e <= edges.Extent(); ++e )
  {
    ReShape->Remove( edges(e) );

    if ( !edgeOnly )
    {
      // Get rid of vertices
      for ( TopExp_Explorer exp(edges(e), TopAbs_VERTEX); exp.More(); exp.Next() )
      {
        ReShape->Remove( exp.Current() );
      }
    }
  }
  //
  m_result = ReShape->Apply(m_master);
  return true; // Success
}
