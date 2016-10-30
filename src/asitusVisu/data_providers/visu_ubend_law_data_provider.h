//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_ubend_law_data_provider_h
#define visu_ubend_law_data_provider_h

// A-Situs (visualization) includes
#include <visu_curve_data_provider.h>

// A-Situs (geometry) includes
#include <geom_ubend_law_node.h>

// OCCT includes
#include <Geom2d_BSplineCurve.hxx>

DEFINE_STANDARD_HANDLE(visu_ubend_law_data_provider, visu_curve_data_provider)

//! Data provider for a U-bend evolution laws.
class visu_ubend_law_data_provider : public visu_curve_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_ubend_law_data_provider, visu_curve_data_provider)

public:

  visu_ubend_law_data_provider(const Handle(geom_ubend_law_node)& N);

public:

  virtual Handle(Standard_Type) GetCurveType ()                     const;
  virtual Handle(Geom2d_Curve)  GetCurve2d   (double& f, double& l) const;
  virtual Handle(Geom_Curve)    GetCurve     (double& f, double& l) const;
  virtual ActAPI_DataObjectId   GetNodeID    ()                     const;

protected:

  virtual Handle(ActAPI_HParameterList) translationSources() const;

protected:

  Handle(geom_ubend_law_node) m_node; //!< Law Node.

};

#endif
