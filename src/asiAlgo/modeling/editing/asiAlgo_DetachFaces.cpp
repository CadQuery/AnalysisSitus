//-----------------------------------------------------------------------------
// Created on: 29 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
}

//! Detaches the given faces from the master model.
//! \param faces [in] faces to detach.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_DetachFaces::Perform(const TopTools_IndexedMapOfShape& faces)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;

  // Detach each face one by one
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    const TopoDS_Shape& current_face = faces(f);

    // Copy face
    BRepBuilderAPI_Copy copier(current_face);
    const TopoDS_Shape& copy = copier.Shape();

    ReShape->Replace(current_face, copy);
  }

  m_result = ReShape->Apply(m_master);

  return true; // Success
}
