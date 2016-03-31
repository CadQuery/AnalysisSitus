//-----------------------------------------------------------------------------
// Created on: 29 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_detach_faces_h
#define geom_detach_faces_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
//#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to detach faces.
class geom_detach_faces
{
public:

  ASitus_EXPORT
    geom_detach_faces(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& faces);

public:

  //! \return result shape.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape                              m_master; //!< Master model.
  TopoDS_Shape                              m_result; //!< Result.
  //TopTools_IndexedDataMapOfShapeListOfShape m_M;      //!< Sub-shapes with ancestors.

};

#endif
