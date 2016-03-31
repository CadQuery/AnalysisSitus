//-----------------------------------------------------------------------------
// Created on: 02 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_delete_faces_h
#define geom_delete_faces_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to delete faces.
class geom_delete_faces
{
public:

  ASitus_EXPORT
    geom_delete_faces(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& faces,
            const bool                        facesOnly);

public:

  //! \return result shape.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
