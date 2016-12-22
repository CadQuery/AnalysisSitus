//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_CurveDataProvider_h
#define asiVisu_CurveDataProvider_h

// A-Situs (visualization) includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_CurveDataProvider, asiVisu_DataProvider)

//! Data provider for parametric curves.
class asiVisu_CurveDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_CurveDataProvider, asiVisu_DataProvider)

public:

  virtual Handle(Standard_Type) GetCurveType ()                     const = 0;
  virtual Handle(Geom2d_Curve)  GetCurve2d   (double& f, double& l) const = 0;
  virtual Handle(Geom_Curve)    GetCurve     (double& f, double& l) const = 0;

};

#endif
