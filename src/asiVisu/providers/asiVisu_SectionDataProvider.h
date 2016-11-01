//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_SectionDataProvider_h
#define asiVisu_SectionDataProvider_h

// A-Situs visualization includes
#include <asiVisu_CurveDataProvider.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_SectionDataProvider, asiVisu_CurveDataProvider)

//! Data provider for geometry of a single section.
class asiVisu_SectionDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_SectionDataProvider, asiVisu_CurveDataProvider)

public:

  asiVisu_SectionDataProvider(const ActAPI_DataObjectId&           theNodeId,
                              const Handle(ActAPI_HParameterList)& theParamList);

public:

  virtual Handle(Standard_Type)
    GetCurveType() const;

  virtual Handle(Geom2d_Curve)
    GetCurve2d(double& f, double& l) const;

  virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  virtual ActAPI_DataObjectId
    GetNodeID() const;

  virtual TopoDS_Shape
    GetShape() const;

public:

  Handle(asiVisu_SectionDataProvider)
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
