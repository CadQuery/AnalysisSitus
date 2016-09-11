//-----------------------------------------------------------------------------
// Created on: 26 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_build_stock_h
#define geom_build_stock_h

// A-Situs includes
#include <analysis_situs.h>

// Feature includes
#include <feature_angle.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_build_stock, ActAPI_IAlgorithm)

//! Utility to build stock.
class geom_build_stock : public ActAPI_IAlgorithm
{
public:

  ASitus_EXPORT
    geom_build_stock(ActAPI_ProgressEntry progress,
                     ActAPI_PlotterEntry  plotter);

public:

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape& part);

  ASitus_EXPORT bool
    operator()(const TopoDS_Shape&               part,
               const TopTools_IndexedMapOfShape& outlineFaces);

public:

  //! \return result stock.
  const TopoDS_Solid& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Solid m_result; //!< Stock shape.

};

#endif
