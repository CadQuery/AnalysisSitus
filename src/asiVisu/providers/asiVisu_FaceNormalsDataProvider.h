//-----------------------------------------------------------------------------
// Created on: 02 March 2017
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

#ifndef asiVisu_FaceNormalsDataProvider_h
#define asiVisu_FaceNormalsDataProvider_h

// asiVisu includes
#include <asiVisu_VectorsDataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_FaceNormalsDataProvider, asiVisu_VectorsDataProvider)

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
