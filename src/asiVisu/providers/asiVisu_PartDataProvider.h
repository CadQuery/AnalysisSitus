//-----------------------------------------------------------------------------
// Created on: 30 March 2017
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

#ifndef asiVisu_PartDataProvider_h
#define asiVisu_PartDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_PartDataProvider, asiVisu_DataProvider)

//! Data provider from Part Node to Presentation.
class asiVisu_PartDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_PartDataProvider(const Handle(asiData_PartNode)& partNode);

protected:

  asiVisu_EXPORT
    asiVisu_PartDataProvider();

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT virtual Handle(asiData_PartNode)
    GetPartNode() const;

  asiVisu_EXPORT virtual TopoDS_Shape
    GetShape() const;

  asiVisu_EXPORT virtual Handle(asiAlgo_AAG)
    GetAAG() const;

  asiVisu_EXPORT double
    GetLinearDeflection() const;

  asiVisu_EXPORT double
    GetAngularDeflection() const;

public:

  asiVisu_EXPORT Handle(asiVisu_PartDataProvider)
    Clone() const;

protected:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  //! Source Node.
  Handle(asiData_PartNode) m_node;

};

#endif
