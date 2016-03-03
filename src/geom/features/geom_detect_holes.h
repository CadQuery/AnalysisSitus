//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_detect_holes_h
#define geom_detect_holes_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_aag.h>

//-----------------------------------------------------------------------------

//! Utility to detect through holes.
class geom_detect_holes
{
public:

  ASitus_EXPORT
    geom_detect_holes(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const double radius);

public:

  //! \return detected holes.
  inline const NCollection_Sequence<TopoDS_Face>& Result() const
  {
    return m_result;
  }

protected:

  template<typename TSurf>
  bool isTypeOf(const TopoDS_Face& face) const
  {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    if ( surf->IsInstance( STANDARD_TYPE(TSurf) ) )
      return true;

    return false;
  }

protected:

  Handle(geom_aag)                  m_aag;    //!< Master AAG.
  NCollection_Sequence<TopoDS_Face> m_result; //!< Detected holes.

};

#endif
