//-----------------------------------------------------------------------------
// Created on: 23 September 2016
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

#ifndef asiAlgo_BVHIterator_h
#define asiAlgo_BVHIterator_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <BVH_Types.hxx>
#include <BVH_PrimitiveSet.hxx>
#include <NCollection_Handle.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

//! Depth-first iterator for BVH structure.
//!
//! Stored data of each BVH node has the following meaning:
//! <pre>
//! ========================================================
//!  coord |         leaf        |        sub-volume       |
//! ========================================================
//!    x   |          !0         |             0           |
//! -------+---------------------+-------------------------+
//!    y   | idx of start grain  | idx of left child node  |
//! -------+---------------------+-------------------------+
//!    z   |  idx of end grain   | idx of right child node |
//! ========================================================
//! </pre>
class asiAlgo_BVHIterator
{
  typedef int BVH_Item; // Local jargon (node in BVH).

public:

  asiAlgo_EXPORT
    asiAlgo_BVHIterator(const opencascade::handle<BVH_Tree<double, 4> >& bvh);

public:

  asiAlgo_EXPORT bool
    More() const;

  asiAlgo_EXPORT const BVH_Vec4i&
    Current() const;

  asiAlgo_EXPORT int
    CurrentIndex() const;

  asiAlgo_EXPORT void
    Next();

  asiAlgo_EXPORT bool
    IsLeaf() const;

  asiAlgo_EXPORT void
    BlockLeft();

  asiAlgo_EXPORT void
    BlockRight();

protected:

  opencascade::handle< BVH_Tree<double, 4> > m_bvh; //!< Structure to iterate over.

  // Iteration state variables
  BVH_Item                   m_stack[96];   //!< Non-traversed nodes to return.
  int                        m_iStackHead;  //!< Pointer to the stack head.
  BVH_Item                   m_currentNode; //!< Current node.
  TColStd_PackedMapOfInteger m_blocked;     //!< Nodes to stop traverse (their
                                            //!< children will be skipped).

};

#endif 
