//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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
