//-----------------------------------------------------------------------------
// Created on: 01 July 2019
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

#ifndef asiVisu_ReEdgeCurveDataProvider_h
#define asiVisu_ReEdgeCurveDataProvider_h

// asiVisu includes
#include <asiVisu_CurveDataProvider.h>

// asiData includes
#include <asiData_ReEdgeNode.h>

//! Data provider for RE edge as a parametric curve.
class asiVisu_ReEdgeCurveDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ReEdgeCurveDataProvider, asiVisu_CurveDataProvider)

public:

  //! Ctor.
  //! \param[in] N Edge Node to access the geometric data from.
  asiVisu_EXPORT
    asiVisu_ReEdgeCurveDataProvider(const Handle(asiData_ReEdgeNode)& N);

public:

  //! Returns associated Node ID.
  //! \return Node ID.
  virtual ActAPI_DataObjectId GetNodeID() const
  {
    return m_node->GetId();
  }

  //! Unused.
  virtual Handle(Geom2d_Curve) GetCurve2d(double& asiVisu_NotUsed(f),
                                          double& asiVisu_NotUsed(l)) const
  {
    return nullptr;
  }

public:

  //! Returns curve type.
  //! \return dynamic type.
  asiVisu_EXPORT virtual Handle(Standard_Type)
    GetCurveType() const;

  //! Returns parametric curve representing the edge.
  //! \param[out] f first parameter.
  //! \param[out] l last parameter.
  //! \return parametric curve.
  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

protected:

  //! Enumerates all Active Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process. If any Parameter listed by this method is changed
  //! (more precisely, if its MTime record is updated), the translation must
  //! be repeated.
  //! \return list of source Parameters.
  asiVisu_EXPORT virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_ReEdgeNode) m_node; //!< Edge Node.

};

#endif
