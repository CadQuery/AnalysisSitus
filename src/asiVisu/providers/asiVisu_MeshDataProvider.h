//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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

#ifndef asiVisu_MeshDataProvider_h
#define asiVisu_MeshDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// Mesh (Active Data) includes
#include <Mesh.h>
#include <Mesh_Group.h>

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
