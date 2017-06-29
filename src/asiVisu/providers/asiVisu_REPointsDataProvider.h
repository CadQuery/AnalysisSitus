//-----------------------------------------------------------------------------
// Created on: 06 April 2016
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

#ifndef asiVisu_REPointsDataProvider_h
#define asiVisu_REPointsDataProvider_h

// A-Situs visualization includes
#include <asiVisu_PointsDataProvider.h>

// asiData includes
#include <asiData_REPointsNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_REPointsDataProvider, asiVisu_PointsDataProvider)

//! Data provider for re-engineering point cloud.
class asiVisu_REPointsDataProvider : public asiVisu_PointsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REPointsDataProvider, asiVisu_PointsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_REPointsDataProvider(const Handle(asiData_REPointsNode)& points_n);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<double>)
    GetPoints() const;

  asiVisu_EXPORT virtual Handle(TColStd_HPackedMapOfInteger)
    GetIndices() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
