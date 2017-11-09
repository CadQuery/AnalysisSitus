//-----------------------------------------------------------------------------
// Created on: 09 February 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
