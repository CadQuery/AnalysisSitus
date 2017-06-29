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

#ifndef asiAlgo_DesignLaw_h
#define asiAlgo_DesignLaw_h

// A-Situs (calculus) includes
#include <asiAlgo_FuncUnivariate.h>

// OCCT includes
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_DesignLaw, asiAlgo_FuncUnivariate)

//! Evolution law for a scalar design variable.
class asiAlgo_DesignLaw : public asiAlgo_FuncUnivariate
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DesignLaw, asiAlgo_FuncUnivariate)

public:

  asiAlgo_EXPORT
    asiAlgo_DesignLaw(const Handle(TColStd_HArray1OfReal)& poles,
                      const double                         last);

public:

  asiAlgo_EXPORT virtual double
    operator() (const double x);

  asiAlgo_EXPORT virtual double
    Min_X() const;

  asiAlgo_EXPORT virtual double
    Max_X() const;

public:

  Handle(TColStd_HArray1OfReal)&    ChangePoles() { return m_poles; }
  Handle(TColStd_HArray1OfReal)&    ChangeKnots() { return m_knots; }
  Handle(TColStd_HArray1OfInteger)& ChangeMults() { return m_mults; }

protected:

  Handle(TColStd_HArray1OfReal)    m_poles; //!< Poles.
  Handle(TColStd_HArray1OfReal)    m_knots; //!< Knots.
  Handle(TColStd_HArray1OfInteger) m_mults; //!< Multiplicities.

};

#endif
