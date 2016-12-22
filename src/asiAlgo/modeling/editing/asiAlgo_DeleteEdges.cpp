//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_DeleteEdges.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <TopExp_Explorer.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
asiAlgo_DeleteEdges::asiAlgo_DeleteEdges(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Removes the given edges from the master model.
//! \param edges    [in] edges to delete.
//! \param edgeOnly [in] indicates whether to delete edge only.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_DeleteEdges::Perform(const TopTools_IndexedMapOfShape& edges,
                                  const bool                        edgeOnly)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;
  //
  for ( int e = 1; e <= edges.Extent(); ++e )
  {
    ReShape->Remove( edges(e) );

    if ( !edgeOnly )
    {
      // Get rid of vertices
      for ( TopExp_Explorer exp(edges(e), TopAbs_VERTEX); exp.More(); exp.Next() )
      {
        ReShape->Remove( exp.Current() );
      }
    }
  }
  //
  m_result = ReShape->Apply(m_master);
  return true; // Success
}
