//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_shape_data_provider_h
#define visu_shape_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

DEFINE_STANDARD_HANDLE(visu_shape_data_provider, visu_data_provider)

//! Data provider from geometry Node to Presentation.
class visu_shape_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_shape_data_provider, visu_data_provider)

public:

  visu_shape_data_provider(const ActAPI_DataObjectId&           theNodeId,
                           const Handle(ActAPI_HParameterList)& theParamList);

protected:

  visu_shape_data_provider();

public:

  virtual ActAPI_DataObjectId
    GetNodeID() const;

  virtual TopoDS_Shape
    GetShape() const;

  virtual Handle(TColStd_HPackedMapOfInteger)
    GetSubShapes() const;

  virtual void
    SetSubShapes(const Handle(TColStd_HPackedMapOfInteger)& theSubShapes);

  virtual bool
    HasPosition() const;

  virtual bool
    HasPositionAndRotation() const;

  virtual void
    GetPosition(double& thePosX,
                double& thePosY,
                double& thePosZ) const;

  virtual void
    GetRotation(double& theAngleAroundX,
                double& theAngleAroundY,
                double& theAngleAroundZ) const;

public:

  Handle(visu_shape_data_provider)
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
