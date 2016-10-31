//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_contour_data_provider_h
#define visu_contour_data_provider_h

// A-Situs visualization includes
#include <visu_shape_data_provider.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(visu_contour_data_provider, visu_shape_data_provider)

//! Data provider for contour shape.
class visu_contour_data_provider : public visu_shape_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_contour_data_provider, visu_shape_data_provider)

public:

  visu_contour_data_provider();

public:

  virtual TopoDS_Shape
    GetShape() const;

};

#endif
