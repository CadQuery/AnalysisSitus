//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_section_data_provider_h
#define visu_section_data_provider_h

// A-Situs visualization includes
#include <visu_curve_data_provider.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(visu_section_data_provider, visu_data_provider)

//! Data provider for geometry of a single section.
class visu_section_data_provider : public visu_curve_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_section_data_provider, visu_curve_data_provider)

public:

  visu_section_data_provider(const ActAPI_DataObjectId&           theNodeId,
                             const Handle(ActAPI_HParameterList)& theParamList);

public:

  virtual Handle(Standard_Type)
    GetCurveType() const;

  virtual Handle(Geom2d_Curve)
    GetCurve2d() const;

  virtual Handle(Geom_Curve)
    GetCurve() const;

  virtual ActAPI_DataObjectId
    GetNodeID() const;

  virtual TopoDS_Shape
    GetShape() const;

public:

  Handle(visu_section_data_provider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node ID.
  ActAPI_DataObjectId m_nodeID;

  //! Source Parameters.
  Handle(ActAPI_HParameterList) m_params;

};

#endif