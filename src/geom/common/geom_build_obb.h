//-----------------------------------------------------------------------------
// Created on: 30 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_build_obb_h
#define geom_build_obb_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_build_obb, ActAPI_IAlgorithm)

//! Utility to build Oriented Bounding Box.
class geom_build_obb : public ActAPI_IAlgorithm
{
public:

  ASitus_EXPORT
    geom_build_obb(ActAPI_ProgressEntry progress,
                   ActAPI_PlotterEntry  plotter);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape& part);

public:

  //! \return result OBB as shape.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape m_result; //!< OBB shape.

};

#endif
