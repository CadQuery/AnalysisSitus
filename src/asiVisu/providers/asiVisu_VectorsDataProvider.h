//-----------------------------------------------------------------------------
// Created on: 09 December 2016
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

#ifndef asiVisu_VectorsDataProvider_h
#define asiVisu_VectorsDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

DEFINE_STANDARD_HANDLE(asiVisu_VectorsDataProvider, asiVisu_DataProvider)

//! Data provider for a vector field.
class asiVisu_VectorsDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_VectorsDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_VectorsDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  virtual Handle(asiAlgo_BaseCloud<float>)
    GetPointsf() = 0;

  virtual Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf() = 0;

  virtual double
    GetMaxVectorModulus() const = 0;

protected:

  Handle(ActAPI_INode) m_source; //!< Source Node.

};

#endif
