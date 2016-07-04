//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_curve_data_provider_h
#define visu_curve_data_provider_h

// A-Situs (visualization) includes
#include <visu_data_provider.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

DEFINE_STANDARD_HANDLE(visu_curve_data_provider, visu_data_provider)

//! Data provider for parametric curves.
class visu_curve_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_curve_data_provider, visu_data_provider)

public:

  virtual Handle(Standard_Type) GetCurveType ()                     const = 0;
  virtual Handle(Geom2d_Curve)  GetCurve2d   (double& f, double& l) const = 0;
  virtual Handle(Geom_Curve)    GetCurve     (double& f, double& l) const = 0;

};

#endif
