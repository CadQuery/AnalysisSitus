//-----------------------------------------------------------------------------
// Created on: 21 July 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiVisu_ReCoedgeDataProvider_h
#define asiVisu_ReCoedgeDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_ReCoEdgeNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

//! Data provider for RE coedge.
class asiVisu_ReCoedgeDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ReCoedgeDataProvider, asiVisu_DataProvider)

public:

  //! Ctor.
  //! \param[in] N Edge Node to access the geometric data from.
  asiVisu_EXPORT
    asiVisu_ReCoedgeDataProvider(const Handle(asiData_ReCoedgeNode)& N);

public:

  //! Returns associated Node ID.
  //! \return Node ID.
  virtual ActAPI_DataObjectId GetNodeID() const
  {
    return m_node->GetId();
  }

public:

  //! Returns parametric curve representing the edge.
  //! \return parametric curve.
  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve() const;

  //! Returns "same sense" flag for coedge.
  //! \return orientation flag.
  asiVisu_EXPORT virtual bool
    GetSameSense() const;

  //! Returns host surface.
  //! \return parametric surface.
  asiVisu_EXPORT virtual Handle(Geom_Surface)
    GetSurface() const;

protected:

  //! Enumerates all Active Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process. If any Parameter listed by this method is changed
  //! (more precisely, if its MTime record is updated), the translation must
  //! be repeated.
  //! \return list of source Parameters.
  asiVisu_EXPORT virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_ReCoedgeNode) m_node; //!< Coedge Node.

};

#endif
