//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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

#ifndef asiAlgo_BlendTopoConditionFF_h
#define asiAlgo_BlendTopoConditionFF_h

// asiAlgo includes
#include <asiAlgo_BlendTopoCondition.h>

// OCCT includes
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Base class for topological conditions on blends between two faces. For this
//! topological condition to realize, the blend face should be enclosed between
//! two support faces connected to the blend face be means of "spring" edges.
class asiAlgo_BlendTopoConditionFF : public asiAlgo_BlendTopoCondition
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoConditionFF, asiAlgo_BlendTopoCondition)

public:

  TopoDS_Face f_s1;   //!< Support face 1.
  TopoDS_Face f_s2;   //!< Support face 2.
  TopoDS_Edge e_b_s1; //!< Spring edge between faces `b` and `s1`.
  TopoDS_Edge e_b_s2; //!< Spring edge between faces `b` and `s2`.

public:

  //! Ctor.
  //! \param[in] aag      attributed adjacency graph of the CAD-model in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_BlendTopoConditionFF(const Handle(asiAlgo_AAG)& aag,
                               ActAPI_ProgressEntry       progress,
                               ActAPI_PlotterEntry        plotter)
  //
  : asiAlgo_BlendTopoCondition (aag, progress, plotter)
  {}

public:

  //! Dumps the topological condition to the passed plotter.
  virtual void Dump(ActAPI_PlotterEntry plotter) const
  {
    asiAlgo_BlendTopoCondition::Dump(plotter); // Dump base entities.

    plotter.REDRAW_SHAPE("f_s1",   this->f_s1);
    plotter.REDRAW_SHAPE("f_s2",   this->f_s2);
    plotter.REDRAW_SHAPE("e_b_s1", this->e_b_s1, Color_Blue, 1.0, true);
    plotter.REDRAW_SHAPE("e_b_s2", this->e_b_s2, Color_Blue, 1.0, true);
  }

  //! Allows to identify a certain topological condition from the passed blend
  //! candidate attribute.
  //! \param[in] bcAttr blend candidate attribute in question.
  //! \return true if the certain topological condition is identified.
  virtual bool Initialize(const Handle(asiAlgo_AttrBlendCandidate)& bcAttr)
  {
    if ( !asiAlgo_BlendTopoCondition::Initialize(bcAttr) )
      return false; // Not identified.

    // Two spring edges are expected.
    const int numSpringEdges = bcAttr->SpringEdgeIndices.Extent();
    //
    if ( numSpringEdges != 2 )
      return false; // Not identified.

    // Get ID of the blend face.
    const int f_b_idx = bcAttr->GetFaceId();

    // Get spring edges.
    const int   e_b_s1_idx = bcAttr->SpringEdgeIndices.GetMinimalMapped();
    const int   e_b_s2_idx = bcAttr->SpringEdgeIndices.GetMaximalMapped();
    TopoDS_Edge e_b_s1_loc = TopoDS::Edge( AAG->RequestMapOfEdges()(e_b_s1_idx) );
    TopoDS_Edge e_b_s2_loc = TopoDS::Edge( AAG->RequestMapOfEdges()(e_b_s2_idx) );

    // Get support (`s`) faces as neighbors of the blend face through
    // the spring edges.
    TColStd_PackedMapOfInteger f_b_s1_indices = AAG->GetNeighborsThru(f_b_idx, e_b_s1_loc);
    TColStd_PackedMapOfInteger f_b_s2_indices = AAG->GetNeighborsThru(f_b_idx, e_b_s2_loc);
    //
    if ( f_b_s1_indices.Extent() != 1 || f_b_s2_indices.Extent() != 1 )
      return false;
    //
    const int f_b_s1_idx = f_b_s1_indices.GetMinimalMapped();
    const int f_b_s2_idx = f_b_s2_indices.GetMinimalMapped();

    // Initialize topological primitives.
    this->f_s1   = this->AAG->GetFace(f_b_s1_idx);
    this->f_s2   = this->AAG->GetFace(f_b_s2_idx);
    this->e_b_s1 = e_b_s1_loc;
    this->e_b_s2 = e_b_s2_loc;

    return true; // Identified.
  }

};

#endif
