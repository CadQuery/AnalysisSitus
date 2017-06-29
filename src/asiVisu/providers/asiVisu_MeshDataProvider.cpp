//-----------------------------------------------------------------------------
// Created on: 26 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
Handle(ActData_Mesh) asiVisu_MeshDataProvider::GetMeshDS() const
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
