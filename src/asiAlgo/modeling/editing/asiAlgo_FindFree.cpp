//-----------------------------------------------------------------------------
// Created on: 17 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_FindFree.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//-----------------------------------------------------------------------------

void asiAlgo_FindFree::FindEdges(const TopoDS_Shape&         shape,
                                 TopTools_IndexedMapOfShape& freeEdges,
                                 ActAPI_ProgressEntry        Journal)
{
  Journal.SendLogMessage( LogInfo(Normal) << "Checking free edges..." );

  // Get child-parent relationships
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, M);

  // Check for every edge the number of its owning faces
  const int nEdges = M.Extent();
  for ( int e = 1; e <= nEdges; ++e )
  {
    const TopoDS_Edge& E = TopoDS::Edge( M.FindKey(e) );
    //
    if ( BRep_Tool::Degenerated(E) )
      continue;

    const int nOwningFaces = M.FindFromIndex(e).Extent();
    if ( nOwningFaces == 1 )
      freeEdges.Add(E);
  }
}

//-----------------------------------------------------------------------------
