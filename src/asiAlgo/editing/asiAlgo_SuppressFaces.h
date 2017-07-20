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

#ifndef asiAlgo_SuppressFaces_h
#define asiAlgo_SuppressFaces_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopTools_ListOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to delete faces.
class asiAlgo_SuppressFaces
{
public:

  asiAlgo_EXPORT
    asiAlgo_SuppressFaces(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT virtual bool
    Perform(const TColStd_PackedMapOfInteger& faceIndices,
            const bool                        facesOnly);

  asiAlgo_EXPORT virtual bool
    Perform(const TopTools_ListOfShape& faces,
            const bool                  facesOnly);

public:

  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return instance of Re-Shape utility used for topological reduction.
  const Handle(BRepTools_ReShape)& GetReShape() const
  {
    return m_reShape;
  }

protected:

  TopoDS_Shape              m_master;  //!< Master model.
  TopoDS_Shape              m_result;  //!< Result.
  Handle(BRepTools_ReShape) m_reShape; //!< Re-Shape tool.

};

#endif
