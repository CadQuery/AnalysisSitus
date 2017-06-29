//-----------------------------------------------------------------------------
// Created on: 02 March 2016
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
#include <asiAlgo_SuppressFaces.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#undef COUT_DEBUG

//! Constructor.
//! \param[in] masterCAD full CAD model.
asiAlgo_SuppressFaces::asiAlgo_SuppressFaces(const TopoDS_Shape& masterCAD)
{
  m_master  = masterCAD;
  m_reShape = new BRepTools_ReShape;
  m_reShape->ModeConsiderLocation() = 1; // We do not care of location today
}

//! Removes the given faces from the master model.
//! \param[in] faceIndices indices faces to suppress.
//! \param[in] facesOnly   indicates whether to delete faces only.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_SuppressFaces::Perform(const TColStd_PackedMapOfInteger& faceIndices,
                                    const bool                        facesOnly)
{
  // Get indices of faces
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(m_master, TopAbs_FACE, faces);

  // Remove requested faces
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(faceIndices); fit.More(); fit.Next() )
  {
    const int face_idx = fit.Key();
    //
    if ( face_idx < 1 || face_idx > faces.Extent() )
      continue;

    const TopoDS_Shape& face = faces(face_idx);

    m_reShape->Remove(face);

    if ( !facesOnly )
    {
      // Get rid of wires
      for ( TopExp_Explorer exp(face, TopAbs_WIRE); exp.More(); exp.Next() )
      {
        m_reShape->Remove( exp.Current() );
      }

      // Get rid of edges
      for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
      {
        m_reShape->Remove( exp.Current() );
      }

      // Get rid of vertices
      for ( TopExp_Explorer exp(face, TopAbs_VERTEX); exp.More(); exp.Next() )
      {
        m_reShape->Remove( exp.Current() );
      }
    }
  }
  m_result = m_reShape->Apply(m_master);

  return true; // Success
}
