//-----------------------------------------------------------------------------
// Created on: 18 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FindNonmanifold_h
#define asiAlgo_FindNonmanifold_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//! Utilities for detection of non-manifold geometry in CAD.
namespace asiAlgo_FindNonmanifold
{
  //! Finds non-manifold edges.
  //! \param shape   [in]     input CAD model.
  //! \param nmEdges [out]    non-manifold edges.
  //! \param Journal [in/out] journal.
  asiAlgo_EXPORT
    void FindEdges(const TopoDS_Shape&         shape,
                   TopTools_IndexedMapOfShape& nmEdges,
                   ActAPI_ProgressEntry        Journal);
};

#endif
