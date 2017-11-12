//-----------------------------------------------------------------------------
// Created on: 29 March 2016
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
#include <asiAlgo_DetachFaces.h>

// OCCT includes
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
asiAlgo_DetachFaces::asiAlgo_DetachFaces(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Detaches the given faces from the master model.
//! \param faces [in] faces to detach.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_DetachFaces::Perform(const TopTools_IndexedMapOfShape& faces)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;

  // Detach each face one by one
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    const TopoDS_Shape& current_face = faces(f);

    // Copy face
    BRepBuilderAPI_Copy copier(current_face);
    const TopoDS_Shape& copy = copier.Shape();

    ReShape->Replace(current_face, copy);
  }

  m_result = ReShape->Apply(m_master);

  return true; // Success
}
