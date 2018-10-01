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

#ifndef asiAlgo_BlendTopoConditionIsolated_h
#define asiAlgo_BlendTopoConditionIsolated_h

// asiAlgo includes
#include <asiAlgo_BlendTopoCondition.h>

//! Data transfer object for description of isolated blends.
struct asiAlgo_BlendTopoConditionIsolated : public asiAlgo_BlendTopoCondition
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoConditionIsolated, asiAlgo_BlendTopoCondition)

public:

  TopoDS_Face   t1;       //!< Terminating face 1.
  TopoDS_Face   t2;       //!< Terminating face 2.
  TopoDS_Edge   s1_t1;    //!< Edge between s1 and t1.
  TopoDS_Edge   s1_t2;    //!< Edge between s1 and t2.
  TopoDS_Edge   s2_t1;    //!< Edge between s2 and t1.
  TopoDS_Edge   s2_t2;    //!< Edge between s2 and t2.
  TopoDS_Vertex s1_s2_t1; //!< Vertex between s1, s2 and t1.
  TopoDS_Vertex s1_s2_t2; //!< Vertex between s1, s2 and t2.

  //! Modified geometry.
  struct ModifGeom : public asiAlgo_BlendTopoCondition::ModifGeom
  {
    Handle(Geom_Surface) geom_t1;
    Handle(Geom_Surface) geom_t2;
    Handle(Geom_Curve)   geom_s1_t1;
    Handle(Geom_Curve)   geom_s1_t2;
    Handle(Geom_Curve)   geom_s2_t1;
    Handle(Geom_Curve)   geom_s2_t2;
    gp_Pnt               geom_s1_s2_t1;
    gp_Pnt               geom_s1_s2_t2;
  };

  //! \return type of blend condition.
  virtual asiAlgo_BlendTopoConditionType GetType() const
  {
    return BlendTopoConditionType_Isolated;
  }
};

};

#endif
