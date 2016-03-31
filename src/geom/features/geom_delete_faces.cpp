//-----------------------------------------------------------------------------
// Created on: 02 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_delete_faces.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp_Explorer.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
geom_delete_faces::geom_delete_faces(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Removes the given faces from the master model.
//! \param faces     [in] faces to delete.
//! \param facesOnly [in] indicates whether to delete faces only.
//! \return true in case of success, false -- otherwise.
bool geom_delete_faces::Perform(const TopTools_IndexedMapOfShape& faces,
                                const bool                        facesOnly)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;
  //
  for ( int f = 1; f <= faces.Extent(); ++f )
  {
    ReShape->Remove( faces(f) );

    if ( !facesOnly )
    {
      // Get rid of wires
      for ( TopExp_Explorer exp(faces(f), TopAbs_WIRE); exp.More(); exp.Next() )
      {
        ReShape->Remove( exp.Current() );
      }

      // Get rid of edges
      for ( TopExp_Explorer exp(faces(f), TopAbs_EDGE); exp.More(); exp.Next() )
      {
        ReShape->Remove( exp.Current() );
      }

      // Get rid of vertices
      for ( TopExp_Explorer exp(faces(f), TopAbs_VERTEX); exp.More(); exp.Next() )
      {
        ReShape->Remove( exp.Current() );
      }
    }
  }
  m_result = ReShape->Apply(m_master);

  // TODO: this is a bad practice. Shape should be valid after face deletion
  ShapeFix_Shape ShapeHealer(m_result);
  try
  {
    ShapeHealer.Perform();
  }
  catch ( ... )
  {
    std::cout << "Error: cannot heal shape" << std::endl;
  }
  m_result = ShapeHealer.Shape();

  return true; // Success
}
