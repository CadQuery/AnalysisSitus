//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//! Default constructor.
asiVisu_MeshDataProvider::asiVisu_MeshDataProvider() : asiVisu_DataProvider() {}

//! Constructor accepting the set of source data structures.
//! \param theNodeId    [in] ID of the target Data Node.
//! \param theParamList [in] source Parameters.
asiVisu_MeshDataProvider::asiVisu_MeshDataProvider(const ActAPI_DataObjectId&           theNodeId,
                                                 const Handle(ActAPI_HParameterList)& theParamList)
: asiVisu_DataProvider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;
}

//! Returns ID of the Data Node represented by VTK actor. This ID is bound to
//! the pipeline's actor in order to have a back-reference from Presentation
//! to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_MeshDataProvider::GetNodeID() const
{
  return m_nodeID;
}

//! Returns Mesh Data Structures used as the main source for pipelining.
//! \return tessellation DS.
Handle(Mesh) asiVisu_MeshDataProvider::GetMeshDS() const
{
  return ActParamTool::AsMesh( m_params->Value(1) )->GetMesh();
}

//! Accessor for the source Data Parameters.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_MeshDataProvider::SourceParameters() const
{
  return this->translationSources();
}

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_MeshDataProvider::translationSources() const
{
  ActAPI_ParameterStream aResStream;
  aResStream << m_params->Value(1); // Mesh Parameter [entire mesh]
  return aResStream;
}
