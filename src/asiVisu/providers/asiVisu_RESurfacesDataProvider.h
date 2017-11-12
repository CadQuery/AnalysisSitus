//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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

#ifndef asiVisu_RESurfacesDataProvider_h
#define asiVisu_RESurfacesDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_RESurfacesNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

//! Data provider for re-engineering surfaces.
class asiVisu_RESurfacesDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RESurfacesDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_RESurfacesDataProvider(const Handle(asiData_RESurfacesNode)& surfaces_n);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT int
    GetNumOfSurfaces() const;

  asiVisu_EXPORT Handle(Geom_Surface)
    GetSurface(const int oneBased_idx,
               double&   uLimit,
               double&   vLimit) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_RESurfacesNode) m_surfaces; //!< Surfaces Node.

};

#endif
