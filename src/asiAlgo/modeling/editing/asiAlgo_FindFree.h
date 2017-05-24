//-----------------------------------------------------------------------------
// Created on: 17 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
