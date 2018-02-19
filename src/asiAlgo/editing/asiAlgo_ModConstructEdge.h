//-----------------------------------------------------------------------------
// Created on: 18 February 2018
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

#ifndef asiAlgo_ModConstructEdge_h
#define asiAlgo_ModConstructEdge_h

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_ModBase.h>
#include <asiAlgo_ModEdgeInfo.h>

// OCCT includes
#include <TopoDS_Vertex.hxx>

//! Modification for geometric adjustment of a single edge.
class asiAlgo_ModConstructEdge : public asiAlgo_ModBase
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ModConstructEdge, asiAlgo_ModBase)

public:

  //! Constructor.
  //! \param[in] aag      Attributed Adjacency Graph.
  //! \param[in] notifier progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_ModConstructEdge(const Handle(asiAlgo_AAG)& aag,
                             ActAPI_ProgressEntry       notifier,
                             ActAPI_PlotterEntry        plotter);

public:

  //! Initializes Modification.
  //! \param[in] target shape being modified.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT virtual bool
    Init(const TopoDS_Shape& target) override;

public:

  //! Returns true if the face <F> has been modified.
  //!
  //! In this case, <S> is the new geometric support of the face, <L> is the
  //! new location, and <tol> is the new tolerance. The argument <revWires>
  //! has to be set to true if the modification reverses normal of the surface
  //! (so the wires have to be reversed). The argument <revFace> has to be
  //! set to true if the orientation of the modified face changes in the
  //! shells which contain it.
  asiAlgo_EXPORT bool
    NewSurface(const TopoDS_Face&    F,
               Handle(Geom_Surface)& S,
               TopLoc_Location&      L,
               double&               tol,
               bool&                 revWires,
               bool&                 revFace) override;

  //! Returns true if the edge <E> has been modified.
  //!
  //! In this case, <C> is the new geometric support of the edge, <L> is the
  //! new location, and <tol> is the new tolerance. If the edge was not
  //! modified, this method returns false and output arguments are not
  //! significant.
  asiAlgo_EXPORT bool
    NewCurve(const TopoDS_Edge&  E,
             Handle(Geom_Curve)& C,
             TopLoc_Location&    L,
             double&             tol) override;

  //! Returns true if the vertex <V> has been modified.
  //!
  //! In this case, <P> is the new geometric support of the vertex, and
  //! <tol> is the new tolerance.
  asiAlgo_EXPORT bool
    NewPoint(const TopoDS_Vertex& V,
             gp_Pnt&              P,
             double&              tol) override;

  //! Returns true if the edge <E> has a new curve on surface
  //! on the given face <F>.
  //!
  //! In this case, <C> is the new geometric support of the edge, <L> is the
  //! new location, and <tol> is the new tolerance.
  asiAlgo_EXPORT bool
    NewCurve2d(const TopoDS_Edge&    E,
               const TopoDS_Face&    F,
               const TopoDS_Edge&    NewE,
               const TopoDS_Face&    NewF,
               Handle(Geom2d_Curve)& C,
               double&               tol) override;

  //! Returns true if the vertex <V> has a new parameter on the given
  //! edge <E>.
  //!
  //! In this case, <p> is the new parameter, and <tol> is the new tolerance.
  asiAlgo_EXPORT bool
    NewParameter(const TopoDS_Vertex& V,
                 const TopoDS_Edge&   E,
                 double&              p,
                 double&              tol) override;

  //! Returns continuity of the edge <NewE> between faces <NewF1> and <NewF2>.
  //!
  //! <NewE> is the new edge created from <E>. <NewF1> (resp. <NewF2>) is
  //! the new face created from <F1> (resp. <F2>).
  asiAlgo_EXPORT GeomAbs_Shape
    Continuity(const TopoDS_Edge& E,
               const TopoDS_Face& F1,
               const TopoDS_Face& F2,
               const TopoDS_Edge& NewE,
               const TopoDS_Face& NewF1,
               const TopoDS_Face& NewF2) override;

protected:

  //! Initializes topological situation near the target edge.
  //! \param[in] targetEdge target edge.
  //! \return false in case of unexpected topological situation.
  asiAlgo_EXPORT bool
    initSituation(const TopoDS_Edge& targetEdge);

protected:

  //! Attributed Adjacency Graph.
  Handle(asiAlgo_AAG) m_aag;

  //! Data structure describing local geometric
  //! situation near the target edge.
  asiAlgo_ModEdgeInfo m_edgeInfo;

};

#endif
