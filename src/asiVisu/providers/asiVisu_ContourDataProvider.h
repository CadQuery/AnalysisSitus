//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ContourDataProvider_h
#define asiVisu_ContourDataProvider_h

// A-Situs visualization includes
#include <asiVisu_ShapeDataProvider.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_ContourDataProvider, asiVisu_ShapeDataProvider)

//! Data provider for contour shape.
class asiVisu_ContourDataProvider : public asiVisu_ShapeDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ContourDataProvider, asiVisu_ShapeDataProvider)

public:

  asiVisu_ContourDataProvider();

public:

  virtual TopoDS_Shape
    GetShape() const;

};

#endif
