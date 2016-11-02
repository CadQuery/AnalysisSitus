//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiEngine_Domain_h
#define asiEngine_Domain_h

// A-Situs includes
#include <asiEngine.h>

// A-Situs (geometry) includes
#include <asiData_PartNode.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//! Data Model API for face domain.
namespace asiEngine_Domain
{
  asiEngine_EXPORT void
    GetHighlightedEdges(const Handle(asiData_PartNode)&            partNode,
                        const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                        TopTools_IndexedMapOfShape&                edges);

  asiEngine_EXPORT void
    GetHighlightedEdges(const Handle(asiData_PartNode)&            partNode,
                        const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                        TopTools_IndexedMapOfShape&                edges,
                        TopoDS_Face&                               face);
};

#endif
