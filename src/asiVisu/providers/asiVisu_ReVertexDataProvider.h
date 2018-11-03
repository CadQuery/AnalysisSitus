//-----------------------------------------------------------------------------
// Created on: 02 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiVisu_ReVertexDataProvider_h
#define asiVisu_ReVertexDataProvider_h

// asiVisu includes
#include <asiVisu_PointsDataProvider.h>

// asiData includes
#include <asiData_ReVertexNode.h>

//! Data provider for a vertex in reverse engineering workflow.
class asiVisu_ReVertexDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ReVertexDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_ReVertexDataProvider(const Handle(asiData_ReVertexNode)& N);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<double>)
    GetPoints() const;

  asiVisu_EXPORT virtual Handle(TColStd_HPackedMapOfInteger)
    GetIndices() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  //! Point cloud structure is stored as a member field to avoid
  //! unnecessary heap allocations. This is possible because the cloud
  //! actually contains always one point, and we need it just to comply
  //! with the pipeline interface.
  Handle(asiAlgo_BaseCloud<double>) m_cloud;

};

#endif
