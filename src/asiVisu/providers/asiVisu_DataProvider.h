//-----------------------------------------------------------------------------
// Created on: 28 September 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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

#ifndef asiVisu_DataProvider_h
#define asiVisu_DataProvider_h

// asiVisu includes
#include <asiVisu.h>

// Active Data (API) includes
#include <ActAPI_INode.h>
#include <ActAPI_IParameter.h>

// Active Data (auxiliary) includes
#include <ActAux_TimeStamp.h>

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
