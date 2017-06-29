//-----------------------------------------------------------------------------
// Created on: 10 February 2016
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

#ifndef asiVisu_CurveDataProvider_h
#define asiVisu_CurveDataProvider_h

// A-Situs (visualization) includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_CurveDataProvider, asiVisu_DataProvider)

//! Data provider for parametric curves.
class asiVisu_CurveDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_CurveDataProvider, asiVisu_DataProvider)

public:

  virtual Handle(Standard_Type) GetCurveType ()                     const = 0;
  virtual Handle(Geom2d_Curve)  GetCurve2d   (double& f, double& l) const = 0;
  virtual Handle(Geom_Curve)    GetCurve     (double& f, double& l) const = 0;

};

#endif
