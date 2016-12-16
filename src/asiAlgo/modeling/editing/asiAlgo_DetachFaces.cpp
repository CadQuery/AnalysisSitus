//-----------------------------------------------------------------------------
// Created on: 29 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_DetachFaces.h>

// OCCT includes
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
asiAlgo_DetachFaces::asiAlgo_DetachFaces(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
  //
  //TopExp::MapShapesAndAncestors(m_master, TopAbs_EDGE, TopAbs_FACE, m_M);
}

//! Detaches the given faces from the master model.
//! \param faces [in] faces to detach.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_DetachFaces::Perform(const TopTools_IndexedMapOfShape& faces)
{
  //// Fill data map of faces with neighbors
  //NCollection_DataMap<TopoDS_Shape, TopTools_IndexedMapOfShape> faceNeighbors;
  //for ( int f = 1; f <= faces.Extent(); ++f )
  //{
  //  const TopoDS_Shape& current_face = faces.FindKey(f);

  //  // Fill the collection of neighbor faces. In order to do that, we loop
  //  // over the full collection of parents of the given edge and accumulate
  //  // those not equal to the current face
  //  TopTools_IndexedMapOfShape current_face_neighbors;
  //  for ( TopExp_Explorer fexp(current_face, TopAbs_EDGE); fexp.More(); fexp.Next() )
  //  {
  //    const TopoDS_Shape&         edge = fexp.Current();
  //    const TopTools_ListOfShape& list = m_M.FindFromKey(edge);
  //    //
  //    for ( TopTools_ListIteratorOfListOfShape lit(list); lit.More(); lit.Next() )
  //    {
  //      if ( lit.Value().IsSame(current_face) )
  //        continue;

  //      // Add next neighbor
  //      current_face_neighbors.Add( lit.Value() );
  //    }
  //  }
  //  //
  //  faceNeighbors.Bind(current_face, current_face_neighbors);
  //}

  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;

  // Detach each face one by one
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    const TopoDS_Shape&               current_face           = faces(f);
    //const TopTools_IndexedMapOfShape& current_face_neighbors = faceNeighbors.Find(current_face);

    // Copy face
    BRepBuilderAPI_Copy copier(current_face);
    const TopoDS_Shape& copy = copier.Shape();

    ReShape->Replace(current_face, copy);
  }

  m_result = ReShape->Apply(m_master);

  return true; // Success
}
