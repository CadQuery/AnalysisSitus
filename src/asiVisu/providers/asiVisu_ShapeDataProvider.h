//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeDataProvider_h
#define asiVisu_ShapeDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_ShapeDataProvider, asiVisu_DataProvider)

//! Data provider from geometry Node to Presentation.
class asiVisu_ShapeDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_ShapeDataProvider(const ActAPI_DataObjectId&           theNodeId,
                              const Handle(ActAPI_HParameterList)& theParamList);

protected:

  asiVisu_EXPORT
    asiVisu_ShapeDataProvider();

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT virtual TopoDS_Shape
    GetShape() const;

  asiVisu_EXPORT virtual Handle(TColStd_HPackedMapOfInteger)
    GetSubShapes() const;

  asiVisu_EXPORT virtual void
    SetSubShapes(const Handle(TColStd_HPackedMapOfInteger)& theSubShapes);

  asiVisu_EXPORT virtual bool
    HasPosition() const;

  asiVisu_EXPORT virtual bool
    HasPositionAndRotation() const;

  asiVisu_EXPORT virtual void
    GetPosition(double& thePosX,
                double& thePosY,
                double& thePosZ) const;

  asiVisu_EXPORT virtual void
    GetRotation(double& theAngleAroundX,
                double& theAngleAroundY,
                double& theAngleAroundZ) const;

public:

  asiVisu_EXPORT Handle(asiVisu_ShapeDataProvider)
    Clone() const;

protected:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  //! Source Node ID.
  ActAPI_DataObjectId m_nodeID;

  //! Source Parameters.
  Handle(ActAPI_HParameterList) m_params;

  //! Mask of sub-shape IDs.
  Handle(TColStd_HPackedMapOfInteger) m_subShapes;

};

#endif
