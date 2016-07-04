//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_point_face_N_IP.h>

// Geometry includes
#include <geom_classify_point_edge_N_IP.h>
#include <geom_epsilon_face_interior_full.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//! Constructor.
//! \param F          [in] face to classify the point against.
//! \param inaccuracy [in] inaccuracy of the input geometry.
//! \param precision  [in] numerical precision for optimization.
geom_classify_point_face_N_IP::geom_classify_point_face_N_IP(const TopoDS_Face& F,
                                                             const double       inaccuracy,
                                                             const double       precision)
: geom_classify_point_face(F, inaccuracy, precision)
{}

//! Performs point-face classification with in-plane neighborhood extraction.
//! \param P       [in]     point to classify.
//! \param N       [out]    neighborhood information.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_face_N_IP::operator()(const gp_Pnt&              P,
                                            Handle(geom_epsilon_base)& N,
                                            ActAPI_ProgressEntry       Journal)
{
  // ...
  // First check if the point falls to the boundary
  // ...

  for ( TopExp_Explorer f_exp(m_F, TopAbs_EDGE); f_exp.More(); f_exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( f_exp.Current() );

    // We do not want to deal with invalid edges
    double f, l;
    if ( BRep_Tool::Curve(E, f, l).IsNull() )
      continue;

    // Prepare classifier
    geom_classify_point_edge_N_IP ClassPE(E, m_F, m_fInaccuracy, m_fPrecision);
    if ( ClassPE(P, N, Journal) & Membership_InOn )
      return Membership_On;
  }

  // ...
  // Now check if the point falls to the interior
  // ...

  // Basic classification
  gp_Pnt2d UV;
  const geom_membership result = geom_classify_point_face::operator()(P, true, Journal);

  // Resolve type
  if ( result == Membership_In )
  {
    Handle(Geom_Surface) S = BRep_Tool::Surface(m_F);

    // Calculate surface normal
    gp_Pnt S_P;
    gp_Vec S_Du, S_Dv;
    S->D1(UV.X(), UV.Y(), S_P, S_Du, S_Dv);
    gp_Dir S_Norm = S_Du.Crossed(S_Dv);

    // Create neighborhood
    N = new geom_epsilon_face_interior_full(S_Norm);
  }
  else if ( result == Membership_On )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_point_face_N_IP.UnexpectedClassificationResult" );
    return Membership_Unknown;
  }

  return result;
}
