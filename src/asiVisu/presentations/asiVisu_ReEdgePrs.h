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

#ifndef asiVisu_ReEdgePrs_h
#define asiVisu_ReEdgePrs_h

// asiVisu includes
#include <asiVisu_DefaultPrs.h>
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_ReEdgeNode.h>

//-----------------------------------------------------------------------------

//! Presentation class for a wireframe edge in the reverse engineering
//! workflow.
class asiVisu_ReEdgePrs : public asiVisu_DefaultPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ReEdgePrs, asiVisu_DefaultPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_ReEdgeNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Polyline = 1,
    Pipeline_Curve,
    Pipeline_CurvePoles,
    Pipeline_CurveKnots
  };

public:

  //! Factory method for Presentation.
  //! \param[in] N Node to create a Presentation for.
  //! \return new Presentation instance.
  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& N);

private:

  //! \brief Creates a Presentation object for the passed Node.
  //! Allocation is allowed only via Instance() method.
  //! \param[in] N Data Node to create a Presentation for.
  asiVisu_ReEdgePrs(const Handle(ActAPI_INode)& N);

};

#endif
