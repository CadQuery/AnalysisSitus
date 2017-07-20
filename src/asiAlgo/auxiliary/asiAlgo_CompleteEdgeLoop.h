//-----------------------------------------------------------------------------
// Created on: 09 March 2017
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

#ifndef asiAlgo_CompleteEdgeLoop_h
#define asiAlgo_CompleteEdgeLoop_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//-----------------------------------------------------------------------------

//! Constructs edge loop starting from the given seed edges.
class asiAlgo_CompleteEdgeLoop : public ActAPI_IAlgorithm
{
public:

  asiAlgo_EXPORT
    asiAlgo_CompleteEdgeLoop(const Handle(asiAlgo_AAG)& aag,
                             ActAPI_ProgressEntry       progress,
                             ActAPI_PlotterEntry        plotter);

public:

  asiAlgo_EXPORT bool
    operator()(const int                   seedEdgeIndex,
               TColStd_PackedMapOfInteger& loopIndices);

protected:

  asiAlgo_EXPORT void
    traverse(const int                   seedEdgeIndex,
             TColStd_PackedMapOfInteger& loopIndices) const;

  asiAlgo_EXPORT void
    addEdges(const TopTools_ListOfShape& neighborEdges,
             TColStd_PackedMapOfInteger& loopIndices) const;

protected:

  Handle(asiAlgo_AAG)                       m_aag;           //!< AAG.
  TopTools_IndexedDataMapOfShapeListOfShape m_vertexEdgeMap; //!< Vertices and their edges.
  TopTools_IndexedDataMapOfShapeListOfShape m_edgeFaceMap;   //!< Edges and their faces.

};

#endif
