//-----------------------------------------------------------------------------
// Created on: 19 February (*) 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiAlgo_ModEdgeInfo_h
#define asiAlgo_ModEdgeInfo_h

// asiAlgo includes
#include <asiAlgo_IntersectionCurveSS.h>
#include <asiAlgo_IntersectionPointCS.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Data structure representing a local topological situation surrounding
//! the given edge of interest.
class asiAlgo_ModEdgeInfo : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ModEdgeInfo, Standard_Transient)

public:

  //! Topological situation.
  struct Situation
  {
    TopoDS_Edge   e_s1_s2;    //!< Edge in question.
    TopoDS_Edge   e_s1_t1;    //!< Edge between owner face s1 and terminating face t1.
    TopoDS_Edge   e_s1_t2;    //!< Edge between owner face s1 and terminating face t2.
    TopoDS_Edge   e_s2_t1;    //!< Edge between owner face s2 and terminating face t1.
    TopoDS_Edge   e_s2_t2;    //!< Edge between owner face s2 and terminating face t2.
    TopoDS_Face   f_s1;       //!< Owner face 1.
    TopoDS_Face   f_s2;       //!< Owner face 2.
    TopoDS_Face   f_t1;       //!< Terminating face(s) t1.
    TopoDS_Face   f_t2;       //!< Terminating face(s) t2.
    TopoDS_Vertex v_s1_s2_t1; //!< First vertex of the edge.
    TopoDS_Vertex v_s1_s2_t2; //!< Last vertex of the edge.

    //! Additional edges to recompute. We expect to have here a fan of
    //! s-t edges. This set does not contain e_s1_t1, e_s1_t2, e_s2_t1, e_s2_t2.
    TopTools_IndexedMapOfShape e_extra;

    //! Extra terminating faces which contain e_extra. This set does not
    //! contain f_t1 and f_t2.
    TopTools_IndexedMapOfShape f_extra;
  };

  //! Geometric primitives to resolve the topological situation.
  struct Resolution
  {
    //! Auxiliary structure representing the result of vertex computation.
    struct t_vertexStatus
    {
      t_vertexStatus() : done(false) {} //!< Default ctor.

      //! Initializes the vertex status structure.
      void Init(const gp_Pnt& _point, const bool _done)
      {
        this->point = _point;
        this->done  = _done;
      }

      gp_Pnt point; //!< Vertex coordinates.
      bool   done;  //!< True if the coordinates were initialized.
    };

    //! Intersection curve between the owner faces (f_1 and f_2).
    Handle(asiAlgo_IntersectionCurveSS) icurve;

    //! Intersection point to recover the geometric support of v_first.
    t_vertexStatus ivf;

    //! Intersection point to recover the geometric support of v_last.
    t_vertexStatus ivl;
  };

public:

  //! Dumps the information object to canvas.
  //! \param[in] plotter imperative plotter.
  void DumpSituation(ActAPI_PlotterEntry plotter) const
  {
    plotter.DRAW_SHAPE(situation.e_s1_s2, Color_Red,  1.0, true, "Situation.e_s1_s2");
    plotter.DRAW_SHAPE(situation.e_s1_t1, Color_Red,  1.0, true, "Situation.e_s1_t1");
    plotter.DRAW_SHAPE(situation.e_s1_t2, Color_Red,  1.0, true, "Situation.e_s1_t2");
    plotter.DRAW_SHAPE(situation.e_s2_t1, Color_Blue, 1.0, true, "Situation.e_s2_t1");
    plotter.DRAW_SHAPE(situation.e_s2_t2, Color_Blue, 1.0, true, "Situation.e_s2_t2");
    //
    plotter.DRAW_SHAPE(situation.f_s1, Color_Red,  0.25,       "Situation.f_s1");
    plotter.DRAW_SHAPE(situation.f_s2, Color_Blue, 0.25,       "Situation.f_s2");
    plotter.DRAW_SHAPE(situation.f_t1, Color_Red,  0.95, true, "Situation.f_t1");
    plotter.DRAW_SHAPE(situation.f_t2, Color_Blue, 0.95, true, "Situation.f_t2");
    //
    plotter.DRAW_SHAPE(situation.v_s1_s2_t1, Color_Red,  1.0, true, "Situation.v_s1_s2_t1");
    plotter.DRAW_SHAPE(situation.v_s1_s2_t2, Color_Blue, 1.0, true, "Situation.v_s1_s2_t2");

    // Dump extra edges.
    for ( int k = 1; k <= situation.e_extra.Extent(); ++k )
    {
      TCollection_AsciiString name("Situation.e_extra["); name += k; name += "]";
      plotter.DRAW_SHAPE(situation.e_extra(k), Color_White, 1.0, true, name);
    }

    // Dump extra faces.
    for ( int k = 1; k <= situation.f_extra.Extent(); ++k )
    {
      TCollection_AsciiString name("Situation.f_extra["); name += k; name += "]";
      plotter.REDRAW_SHAPE(name, situation.f_extra(k), Color_White, 1.0, false);
    }
  }

public:

  Situation  situation;  //!< Topological situation.
  Resolution resolution; //!< Geometric resolution.

};

#endif
