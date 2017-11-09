//-----------------------------------------------------------------------------
// Created on: 26 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
