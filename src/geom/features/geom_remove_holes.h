//-----------------------------------------------------------------------------
// Created on: 02 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_remove_holes_h
#define geom_remove_holes_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! Utility to remove through holes.
class geom_remove_holes
{
public:

  ASitus_EXPORT
    geom_remove_holes(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const NCollection_Sequence<TopoDS_Face>& featureFaces);

public:

  //! \return shape without holes.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
