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

#ifndef asiVisu_LawDataProvider_h
#define asiVisu_LawDataProvider_h

// asiVisu includes
#include <asiVisu_FuncUnivariateDataProvider.h>

// asiData includes
#include <asiData_DesignLawNode.h>

//! Data provider for explicitly defined design law.
class asiVisu_LawDataProvider : public asiVisu_FuncUnivariateDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_LawDataProvider, asiVisu_FuncUnivariateDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_LawDataProvider(const Handle(asiData_DesignLawNode)& N);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_FuncUnivariate)
    GetFunc() const;

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

protected:

  virtual Handle(ActAPI_HParameterList) translationSources() const;

protected:

  Handle(asiData_DesignLawNode) m_node; //!< Source Node.

};

#endif
