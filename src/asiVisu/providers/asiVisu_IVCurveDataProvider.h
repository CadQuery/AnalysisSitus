//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVCurveDataProvider_h
#define asiVisu_IVCurveDataProvider_h

// A-Situs visualization includes
#include <asiVisu_CurveDataProvider.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVCurveDataProvider, asiVisu_CurveDataProvider)

//! Data provider for a single curve in IV.
class asiVisu_IVCurveDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVCurveDataProvider, asiVisu_CurveDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_IVCurveDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual Handle(Standard_Type)
    GetCurveType() const;

  asiVisu_EXPORT virtual Handle(Geom2d_Curve)
    GetCurve2d(double& f, double& l) const;

  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT Handle(asiVisu_IVCurveDataProvider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
