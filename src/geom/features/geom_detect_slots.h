//-----------------------------------------------------------------------------
// Created on: 23 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_detect_slots_h
#define geom_detect_slots_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_aag.h>

//-----------------------------------------------------------------------------

//! Utility to detect planar slots.
class geom_detect_slots
{
public:

  ASitus_EXPORT
    geom_detect_slots(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform();

public:

  //! \return detected holes.
  inline const TopTools_IndexedMapOfShape& Result() const
  {
    return m_result;
  }

protected:

  Handle(geom_aag)           m_aag;    //!< Master AAG.
  TopTools_IndexedMapOfShape m_result; //!< Detected slot faces.

};

#endif
