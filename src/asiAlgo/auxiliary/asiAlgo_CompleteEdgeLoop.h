//-----------------------------------------------------------------------------
// Created on: 09 March 2017
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

  //! Ctor.
  //! \param[in] aag      Attributed Adjacency Graph (AAG).
  //! \param[in] progress Progress Notifier.
  //! \param[in] plotter  Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_CompleteEdgeLoop(const Handle(asiAlgo_AAG)& aag,
                             ActAPI_ProgressEntry       progress,
                             ActAPI_PlotterEntry        plotter);

public:

  //! Attempts to construct a loop of edges starting from the given seed edge.
  //! \param[in]  seedEdgeIndex 1-based index of a seed edge.
  //! \param[out] loopIndices   all edges forming a loop.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    operator()(const int                   seedEdgeIndex,
               TColStd_PackedMapOfInteger& loopIndices);

public:

  //! Sets map of vertex-edges.
  //! \param[in] veMap map to set.
  void SetVertexEdgeMap(const TopTools_IndexedDataMapOfShapeListOfShape& veMap)
  {
    m_vertexEdgeMap = veMap;
  }

  //! Sets map of edge-face.
  //! \param[in] efMap map to set.
  void SetEdgeFaceMap(const TopTools_IndexedDataMapOfShapeListOfShape& efMap)
  {
    m_edgeFaceMap = efMap;
  }

protected:

  //! Traverses next/previous edges for the given seed.
  //! \param[in] seedEdgeIndex 1-based index of the seed edge.
  //! \param[out] loopIndices  accumulated indices of loop edges.
  asiAlgo_EXPORT void
    traverse(const int                   seedEdgeIndex,
             TColStd_PackedMapOfInteger& loopIndices) const;

  //! From the given collection of neighbor edges, this method selects those
  //! which were not iterated, and adds them to the output collection.
  //! \param[in] neighborEdges edges to traverse.
  //! \param[out] result       accumulated indices of loop edges.
  asiAlgo_EXPORT void
    addEdges(const TopTools_ListOfShape& neighborEdges,
             TColStd_PackedMapOfInteger& loopIndices) const;

protected:

  Handle(asiAlgo_AAG)                       m_aag;           //!< AAG.
  TopTools_IndexedDataMapOfShapeListOfShape m_vertexEdgeMap; //!< Vertices and their edges.
  TopTools_IndexedDataMapOfShapeListOfShape m_edgeFaceMap;   //!< Edges and their faces.

};

#endif
