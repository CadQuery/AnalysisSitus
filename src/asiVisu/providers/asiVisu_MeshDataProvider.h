//-----------------------------------------------------------------------------
// Created on: 13 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshDataProvider_h
#define asiVisu_MeshDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// Mesh (Active Data) includes
#include <ActData_Mesh.h>
#include <ActData_Mesh_Group.h>

//! Provides data necessary for visualization of mesh structures.
class asiVisu_MeshDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_MeshDataProvider();

  asiVisu_EXPORT
    asiVisu_MeshDataProvider(const ActAPI_DataObjectId&           theNodeId,
                             const Handle(ActAPI_HParameterList)& theParamList);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT virtual Handle(ActData_Mesh)
    GetMeshDS() const;

  asiVisu_EXPORT virtual Handle(ActAPI_HParameterList)
    SourceParameters() const;

public:

  //! Creates copy of Data Provider.
  //! \return copy.
  Handle(asiVisu_MeshDataProvider) Clone() const
  {
    return new asiVisu_MeshDataProvider(m_nodeID, m_params);
  }

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  ActAPI_DataObjectId           m_nodeID; //!< Source Node ID.
  Handle(ActAPI_HParameterList) m_params; //!< Source Parameters.

};

#endif
