//-----------------------------------------------------------------------------
// Created on: 25 May 2017
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

#ifndef asiVisu_FuncUnivariateDataProvider_h
#define asiVisu_FuncUnivariateDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_FuncUnivariate.h>

//! Data provider for explicitly defined univariate function.
class asiVisu_FuncUnivariateDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncUnivariateDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_FuncUnivariate)
    GetFunc() const = 0;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const = 0;

};

#endif
