//-----------------------------------------------------------------------------
// Created on: 29 March 2016
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
