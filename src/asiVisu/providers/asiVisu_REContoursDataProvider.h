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

#ifndef asiVisu_REContoursDataProvider_h
#define asiVisu_REContoursDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_REContoursNode.h>

// OCCT includes
#include <TopoDS_Wire.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_REContoursDataProvider, asiVisu_DataProvider)

//! Data provider for re-engineering contours.
class asiVisu_REContoursDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REContoursDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_REContoursDataProvider(const Handle(asiData_REContoursNode)& contours_n);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT int
    GetNumOfContours() const;

  asiVisu_EXPORT TopoDS_Wire
    GetContour(const int oneBased_idx) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_REContoursNode) m_contours; //!< Contours Node.

};

#endif
