//-----------------------------------------------------------------------------
// Created on: 18 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_find_nonmanifold_h
#define geom_find_nonmanifold_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//! Utilities for detection of non-manifold geometry in CAD.
namespace geom_find_nonmanifold
{

  //! Finds non-manifold edges.
  //! \param shape   [in]     input CAD model.
  //! \param nmEdges [out]    non-manifold edges.
  //! \param Journal [in/out] journal.
  void FindEdges(const TopoDS_Shape&         shape,
                 TopTools_IndexedMapOfShape& nmEdges,
                 ActAPI_ProgressEntry        Journal);
};

#endif
