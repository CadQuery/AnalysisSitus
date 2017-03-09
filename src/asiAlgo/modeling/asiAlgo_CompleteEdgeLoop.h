//-----------------------------------------------------------------------------
// Created on: 09 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
