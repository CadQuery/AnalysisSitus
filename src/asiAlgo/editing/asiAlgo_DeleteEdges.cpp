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
//    * Neither the name of Sergey Slyadnev nor the
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
