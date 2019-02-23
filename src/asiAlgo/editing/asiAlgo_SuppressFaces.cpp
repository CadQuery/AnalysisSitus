//-----------------------------------------------------------------------------
// Created on: 02 March 2016
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

//-----------------------------------------------------------------------------

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
  TopTools_ListOfShape faceList;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(faceIndices); fit.More(); fit.Next() )
  {
    const int face_idx = fit.Key();
    //
    if ( face_idx < 1 || face_idx > faces.Extent() )
      continue;

    faceList.Append( faces(face_idx) );
  }

  return this->Perform(faceList, facesOnly);
}

//-----------------------------------------------------------------------------

//! Removes the given faces from the master model.
//! \param[in] faces     faces to suppress.
//! \param[in] facesOnly indicates whether to delete faces only.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_SuppressFaces::Perform(const TopTools_ListOfShape& faces,
                                    const bool                  facesOnly)
{
  // Remove requested faces
  for ( TopTools_ListIteratorOfListOfShape fit(faces); fit.More(); fit.Next() )
  {
    const TopoDS_Shape& face = fit.Value();

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
