//-----------------------------------------------------------------------------
// Created on: 28 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_rehost_faces_h
#define geom_rehost_faces_h

// A-Situs includes
#include <asitusGeom.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to re-host faces so that they share the same underlying surface.
class geom_rehost_faces
{
public:

  ASitus_EXPORT
    geom_rehost_faces(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& faces);

public:

  //! \return result.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  void moveWires(const TopoDS_Face& source,
                 TopoDS_Face&       target) const;

protected:

  TopoDS_Shape m_master; //!< Master CAD.
  TopoDS_Shape m_result; //!< Result.

};

#endif
