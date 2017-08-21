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

#ifndef asiAlgo_FindFree_h
#define asiAlgo_FindFree_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//! Utilities for detection of free geometry in CAD.
namespace asiAlgo_FindFree
{
  //! Finds free edges.
  //! \param shape   [in]     input CAD model.
  //! \param nmEdges [out]    free edges.
  //! \param Journal [in/out] journal.
  asiAlgo_EXPORT
    void FindEdges(const TopoDS_Shape&         shape,
                   TopTools_IndexedMapOfShape& freeEdges,
                   ActAPI_ProgressEntry        Journal);
};

#endif