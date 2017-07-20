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

#ifndef asiAlgo_JoinEdges_h
#define asiAlgo_JoinEdges_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to join edge chains.
class asiAlgo_JoinEdges
{
public:

  asiAlgo_EXPORT
    asiAlgo_JoinEdges(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& edges,
            const TopoDS_Face&                face);

public:

  //! \return result shape.
  const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  void chooseOrder(const TopTools_IndexedMapOfShape& edges,
                   TopoDS_Edge&                      eFirst,
                   TopoDS_Edge&                      eSecond) const;

  bool joinEdges(const TopoDS_Edge& eFirst,
                 const TopoDS_Edge& eSecond,
                 const TopoDS_Face& face,
                 TopoDS_Edge&       eResult) const;

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
