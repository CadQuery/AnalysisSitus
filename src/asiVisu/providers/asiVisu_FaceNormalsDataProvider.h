//-----------------------------------------------------------------------------
// Created on: 02 March 2017
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
//    * Neither the name of the copyright holder(s) nor the
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

#ifndef asiVisu_FaceNormalsDataProvider_h
#define asiVisu_FaceNormalsDataProvider_h

// asiVisu includes
#include <asiVisu_VectorsDataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

//! Data provider for b-rep face normals.
class asiVisu_FaceNormalsDataProvider : public asiVisu_VectorsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FaceNormalsDataProvider, asiVisu_VectorsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_FaceNormalsDataProvider(const Handle(asiData_FaceNormsNode)& faceNode);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<float>)
    GetPointsf();

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf();

  asiVisu_EXPORT virtual double
    GetMaxVectorModulus() const;

public:

  asiVisu_EXPORT TopoDS_Face
    GetFace() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_PartNode)         m_partNode; //!< Owning part Node.
  Handle(asiAlgo_BaseCloud<float>) m_points;   //!< Cached points.
  Handle(asiAlgo_BaseCloud<float>) m_vectors;  //!< Cached vectors.

};

#endif
