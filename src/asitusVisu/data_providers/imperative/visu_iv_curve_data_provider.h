//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_curve_data_provider_h
#define visu_iv_curve_data_provider_h

// A-Situs visualization includes
#include <visu_curve_data_provider.h>

DEFINE_STANDARD_HANDLE(visu_iv_curve_data_provider, visu_curve_data_provider)

//! Data provider for a single curve in IV.
class visu_iv_curve_data_provider : public visu_curve_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_curve_data_provider, visu_curve_data_provider)

public:

  visu_iv_curve_data_provider(const Handle(ActAPI_INode)& N);

public:

  virtual Handle(Standard_Type)
    GetCurveType() const;

  virtual Handle(Geom2d_Curve)
    GetCurve2d(double& f, double& l) const;

  virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  Handle(visu_iv_curve_data_provider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
