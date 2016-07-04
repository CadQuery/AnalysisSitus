//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_divide_by_continuity_h
#define geom_divide_by_continuity_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <Precision.hxx>
#include <TopoDS_Shell.hxx>

//! Utilities for dividing shapes by continuity criteria.
namespace geom_divide_by_continuity
{
  //! Divides a shape so as to satisfy "at least C1" criterion on its host
  //! surfaces.
  //! \param shape     [in/out] shape to convert.
  //! \param precision [in]     precision of operation.
  //! \param Journal   [in]     progress notifier.
  //! \return true in case of full success, false -- in case of at least
  //!         partial failure.
  bool Divide_C0(TopoDS_Shape&        shape,
                 const double         precision,
                 ActAPI_ProgressEntry Journal);
};

#endif
