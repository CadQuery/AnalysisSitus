//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_shape_data_provider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Constructor accepting the set of source data structures.
//! \param theNodeId    [in] ID of the target Data Node.
//! \param theParamList [in] source Parameters.
visu_shape_data_provider::visu_shape_data_provider(const ActAPI_DataObjectId&           theNodeId,
                                                   const Handle(ActAPI_HParameterList)& theParamList)
: visu_data_provider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;
}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_shape_data_provider::GetNodeID() const
{
  return m_nodeID;
}

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape visu_shape_data_provider::GetShape() const
{
  return ActParamTool::AsShape( m_params->Value(1) )->GetShape();
}

//! Returns mask of sub-shape IDs to keep in the initial source.
//! \return mask of sub-shape IDs.
Handle(TColStd_HPackedMapOfInteger) visu_shape_data_provider::GetSubShapes() const
{
  return m_subShapes;
}

//! Setter for transient collection of sub-shape IDs.
//! \param theSubShapes [in] collection to set.
void visu_shape_data_provider::SetSubShapes(const Handle(TColStd_HPackedMapOfInteger)& theSubShapes)
{
  m_subShapes = theSubShapes;
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(visu_shape_data_provider) visu_shape_data_provider::Clone() const
{
  return new visu_shape_data_provider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Returns TRUE if position data is sourced.
//! \return true/false.
bool visu_shape_data_provider::HasPosition() const
{
  return false;
}

//! Returns TRUE if position & rotation data is sourced.
//! \return true/false.
bool visu_shape_data_provider::HasPositionAndRotation() const
{
  return false;
}

//! Accessor for position data.
//! \param thePosX [out] position X.
//! \param thePosY [out] position Y.
//! \param thePosZ [out] position Z.
void visu_shape_data_provider::GetPosition(double& thePosX,
                                           double& thePosY,
                                           double& thePosZ) const
{
  thePosX = 0.0;
  thePosY = 0.0;
  thePosZ = 0.0;
}

//! Accessor for rotation data.
//! \param theAngleAroundX [out] rotation angle around OX axis.
//! \param theAngleAroundY [out] rotation angle around OY axis.
//! \param theAngleAroundZ [out] rotation angle around OZ axis.
void visu_shape_data_provider::GetRotation(double& theAngleAroundX,
                                           double& theAngleAroundY,
                                           double& theAngleAroundZ) const
{
  theAngleAroundX = 0.0;
  theAngleAroundY = 0.0;
  theAngleAroundZ = 0.0;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_shape_data_provider::translationSources() const
{
  return ActAPI_ParameterStream() << m_params->Value(1); // Shape Parameter
}
