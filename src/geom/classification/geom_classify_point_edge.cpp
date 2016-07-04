//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_point_edge.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Extrema_ExtPC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//! Constructor.
//! \param E          [in] edge to classify the point against.
//! \param inaccuracy [in] inaccuracy of the input geometry.
//! \param precision  [in] numerical precision for optimization.
geom_classify_point_edge::geom_classify_point_edge(const TopoDS_Edge& E,
                                                   const double       inaccuracy,
                                                   const double       precision)
: m_edge(E), m_fInaccuracy(inaccuracy), m_fPrecision(precision)
{
}

//! Performs point-edge classification.
//! \param P       [in]     point to classify.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_edge::operator()(const gp_Pnt&        P,
                                       ActAPI_ProgressEntry Journal)
{
  TopoDS_Vertex extremity;
  return this->operator()(P, extremity, Journal);
}

//! Performs point-edge classification.
//! \param P         [in]     point to classify.
//! \param extremity [in]     vertex interacted by the point (if any). This
//!                           output parameter is initialized only if
//!                           the classification result is ON.
//! \param Journal   [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_edge::operator()(const gp_Pnt&        P,
                                       TopoDS_Vertex&       extremity,
                                       ActAPI_ProgressEntry Journal)
{
  // ...
  // Check if the point falls into singularity of vertex
  // ...

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex Vf = sae.FirstVertex(m_edge);
  TopoDS_Vertex Vl = sae.LastVertex(m_edge);

  gp_Pnt Pf = BRep_Tool::Pnt(Vf);
  gp_Pnt Pl = BRep_Tool::Pnt(Vl);

  if ( P.Distance(Pf) < m_fInaccuracy )
  {
    extremity = Vf;
    return Membership_On;
  }

  if ( P.Distance(Pl) < m_fInaccuracy )
  {
    extremity = Vl;
    return Membership_On;
  }

  // ...
  // Check if point belongs the the edge's host curve by optimization
  // ...

  double f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(m_edge, f, l);

  GeomAdaptor_Curve AC(C, f, l);
  Extrema_ExtPC ExtPC(P, AC, m_fPrecision);

  if ( !ExtPC.IsDone() || !ExtPC.NbExt() )
    return Membership_Out;

  for ( int iSol = 1; iSol <= ExtPC.NbExt(); ++iSol )
  {
    const double dist2 = ExtPC.SquareDistance(iSol);
    if ( dist2 > Square(m_fInaccuracy) )
      return Membership_Out;
  }

  return Membership_In;
}
