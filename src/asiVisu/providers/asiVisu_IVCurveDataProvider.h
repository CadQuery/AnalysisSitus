//-----------------------------------------------------------------------------
// Created on: 11 April 2016
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

#ifndef asiVisu_IVCurveDataProvider_h
#define asiVisu_IVCurveDataProvider_h

// A-Situs visualization includes
#include <asiVisu_CurveDataProvider.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVCurveDataProvider, asiVisu_CurveDataProvider)

//! Data provider for a single curve in IV.
class asiVisu_IVCurveDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVCurveDataProvider, asiVisu_CurveDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_IVCurveDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual Handle(Standard_Type)
    GetCurveType() const;

  asiVisu_EXPORT virtual Handle(Geom2d_Curve)
    GetCurve2d(double& f, double& l) const;

  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT Handle(asiVisu_IVCurveDataProvider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
