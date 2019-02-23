//-----------------------------------------------------------------------------
// Created on: 06 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiVisu_ShapeDataProvider.h>

// asiData includes
#include <asiData_ReEdgeNode.h>

//! Data provider for contour shape.
class asiVisu_ReEdgeCurveDataProvider : public asiVisu_ShapeDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ReEdgeCurveDataProvider, asiVisu_ShapeDataProvider)

public:

  //! Ctor.
  //! \param[in] N Edge Node to access the geometric data from.
  asiVisu_EXPORT
    asiVisu_ReEdgeCurveDataProvider(const Handle(asiData_ReEdgeNode)& N);

public:

  //! Returns the BREP shape to be visualized.
  //! \return BREP shape being a container for the parametric curve.
  asiVisu_EXPORT virtual TopoDS_Shape
    GetShape() const;

protected:

  Handle(asiData_ReEdgeNode) m_node; //!< Edge Node.

};

#endif
