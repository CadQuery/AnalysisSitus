//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_data_provider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//! Default constructor.
visu_mesh_data_provider::visu_mesh_data_provider() : visu_data_provider() {}

//! Constructor accepting the set of source data structures.
//! \param theNodeId    [in] ID of the target Data Node.
//! \param theParamList [in] source Parameters.
visu_mesh_data_provider::visu_mesh_data_provider(const ActAPI_DataObjectId&           theNodeId,
                                                 const Handle(ActAPI_HParameterList)& theParamList)
: visu_data_provider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;
}

//! Returns ID of the Data Node represented by VTK actor. This ID is bound to
//! the pipeline's actor in order to have a back-reference from Presentation
//! to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_mesh_data_provider::GetNodeID() const
{
  return m_nodeID;
}

//! Returns Mesh Data Structures used as the main source for pipelining.
//! \return tessellation DS.
Handle(OMFDS_Mesh) visu_mesh_data_provider::GetMeshDS() const
{
  return ActParamTool::AsMesh( m_params->Value(1) )->GetMesh();
}

//! Accessor for the source Data Parameters.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_mesh_data_provider::SourceParameters() const
{
  return this->translationSources();
}

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_mesh_data_provider::translationSources() const
{
  ActAPI_ParameterStream aResStream;
  aResStream << m_params->Value(1); // Mesh Parameter [entire mesh]
  return aResStream;
}
