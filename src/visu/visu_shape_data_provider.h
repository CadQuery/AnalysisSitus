//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_shape_data_provider_h
#define visu_shape_data_provider_h

// A-Situs visualization includes
#include <visu_shape_pipeline.h>

DEFINE_STANDARD_HANDLE(visu_shape_data_provider, visu_data_provider)

//! Data provider from geometry Node to Presentation.
class visu_shape_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_shape_data_provider, visu_data_provider)

public:

  visu_shape_data_provider(const ActAPI_DataObjectId&           theNodeId,
                           const Handle(ActAPI_HParameterList)& theParamList);

public:

  ActAPI_DataObjectId
    GetNodeID() const;

  TopoDS_Shape
    GetShape() const;

  Handle(TColStd_HPackedMapOfInteger)
    GetSubShapes() const;

  void
    SetSubShapes(const Handle(TColStd_HPackedMapOfInteger)& theSubShapes);

  bool
    HasPosition() const;

  bool
    HasPositionAndRotation() const;

  void
    GetPosition(double& thePosX,
                double& thePosY,
                double& thePosZ) const;

  void
    GetRotation(double& theAngleAroundX,
                double& theAngleAroundY,
                double& theAngleAroundZ) const;

public:

  Handle(visu_shape_data_provider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node ID.
  ActAPI_DataObjectId m_nodeID;

  //! Source Parameters.
  Handle(ActAPI_HParameterList) m_params;

  //! Mask of sub-shape IDs.
  Handle(TColStd_HPackedMapOfInteger) m_subShapes;

};

#endif
