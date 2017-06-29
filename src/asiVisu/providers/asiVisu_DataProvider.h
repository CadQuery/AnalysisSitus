//-----------------------------------------------------------------------------
// Created on: 28 September 2015
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

#ifndef asiVisu_DataProvider_h
#define asiVisu_DataProvider_h

// A-Situs includes
#include <asiVisu.h>

// Active Data (API) includes
#include <ActAPI_INode.h>
#include <ActAPI_IParameter.h>

// Active Data (auxiliary) includes
#include <ActAux_TimeStamp.h>

DEFINE_STANDARD_HANDLE(asiVisu_DataProvider, Standard_Transient)

//! The instances of this class are normally used to supply data to the VTK
//! visualization pipelines. The main idea behind this class is to provide
//! a universal way of feeding the pipelines with domain-specific data
//! structures for their subsequent translation to VTK data sets. Such
//! DOMAIN -> VTK translation process is usually quite heavy, so it should
//! perform only in cases when the DOMAIN data is actually changed. The latter
//! fact is normally checked by comparing modification time (MTime) of the
//! data source with the modification time of the pipeline.
class asiVisu_DataProvider : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_DataProvider, Standard_Transient)

public:

  asiVisu_EXPORT virtual bool
    MustExecute(const Handle(ActAux_TimeStamp)& Against) const;

public:

  //! Returns associated Node ID.
  //! \return Node ID.
  virtual ActAPI_DataObjectId
    GetNodeID() const = 0;

protected:

  //! Enumerates all Active Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process. If any Parameter listed by this method is changed
  //! (more precisely, if its MTime record is updated), the translation must
  //! be repeated.
  //! \return list of source Parameters.
  virtual Handle(ActAPI_HParameterList)
    translationSources() const = 0;

};

#endif
