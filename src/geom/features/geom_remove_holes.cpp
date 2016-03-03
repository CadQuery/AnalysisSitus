//-----------------------------------------------------------------------------
// Created on: 02 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_remove_holes.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <TopExp_Explorer.hxx>

//! Constructor.
//! \param masterCAD [in] full CAD model.
geom_remove_holes::geom_remove_holes(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Removes holes assembled from the given feature faces.
//! \param featureFaces [in] feature faces.
//! \return true in case of success, false -- otherwise.
bool geom_remove_holes::Perform(const NCollection_Sequence<TopoDS_Face>& featureFaces)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;

  for ( int f = 1; f <= featureFaces.Length(); ++f )
  {
    ReShape->Remove( featureFaces(f) );

    // Get rid of wires
    for ( TopExp_Explorer exp(featureFaces(f), TopAbs_WIRE); exp.More(); exp.Next() )
    {
      ReShape->Remove( exp.Current() );
    }

    // Get rid of edges
    for ( TopExp_Explorer exp(featureFaces(f), TopAbs_EDGE); exp.More(); exp.Next() )
    {
      ReShape->Remove( exp.Current() );
    }

    // Get rid of vertices
    for ( TopExp_Explorer exp(featureFaces(f), TopAbs_VERTEX); exp.More(); exp.Next() )
    {
      ReShape->Remove( exp.Current() );
    }
  }
  m_result = ReShape->Apply(m_master);

  return true; // Success
}
