//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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

#ifndef asiVisu_FuncUnivariateSource_h
#define asiVisu_FuncUnivariateSource_h

// asiVisu includes
#include <asiVisu_CurveSource.h>

// asiAlgo includes
#include <asiAlgo_FuncUnivariate.h>

//! Source of polygonal data representing a univariate explicit function.
class asiVisu_FuncUnivariateSource : public asiVisu_CurveSource
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_FuncUnivariateSource, vtkPolyDataAlgorithm);
  static asiVisu_FuncUnivariateSource* New();

// Kernel methods:
public:

  void SetFunc(const Handle(asiAlgo_FuncUnivariate)& func);

protected:

  asiVisu_FuncUnivariateSource();
  ~asiVisu_FuncUnivariateSource();

private:

  asiVisu_FuncUnivariateSource(const asiVisu_FuncUnivariateSource&);
  asiVisu_FuncUnivariateSource& operator=(const asiVisu_FuncUnivariateSource&);

private:

  Handle(asiAlgo_FuncUnivariate) m_func; //!< Function to convert to polygonal form.

};

#endif
