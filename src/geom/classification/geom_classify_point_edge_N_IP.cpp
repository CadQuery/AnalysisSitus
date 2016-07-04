//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_point_edge_N_IP.h>

// Geometry includes
#include <geom_classify_point_face.h>
#include <geom_epsilon_face_interior_hemi.h>
#include <geom_epsilon_face_interior_sector.h>
#include <geom_project_point_curve.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

namespace
{
  gp_Dir route(const TopoDS_Edge& E,
               const TopoDS_Face& F,
               const double       W)
  {
    // Extract curve
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);

    // Get derivative
    gp_Pnt P;
    gp_Vec D1;
    C->D1(W, P, D1);

    // Get tangent
    gp_Dir T = D1;

    // Apply orientation of edge
    if ( E.Orientation() == TopAbs_REVERSED )
      T.Reverse();

    // Apply orientation of wire
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(F, TopAbs_EDGE, TopAbs_WIRE, M);
    const TopTools_ListOfShape& Wires = M.FindFromKey(E);
    for ( TopTools_ListIteratorOfListOfShape it(Wires); it.More(); it.Next() )
    {
      const TopoDS_Wire& W = TopoDS::Wire( it.Value() );
      if ( W.Orientation() == TopAbs_REVERSED )
        T.Reverse();
    }

    return T;
  }
};

//! Constructor.
//! \param E          [in] edge to classify the point against.
//! \param F          [in] base face for neighborhood resolution.
//! \param inaccuracy [in] inaccuracy of the input geometry.
//! \param precision  [in] numerical precision for optimization.
geom_classify_point_edge_N_IP::geom_classify_point_edge_N_IP(const TopoDS_Edge& E,
                                                             const TopoDS_Face& F,
                                                             const double       inaccuracy,
                                                             const double       precision)
: geom_classify_point_edge(E, inaccuracy, precision), m_F(F)
{}

//! Performs point-edge classification.
//! \param PonCC   [in]     point to classify.
//! \param N       [in]     neighborhood information.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_edge_N_IP::operator()(const geom_intersection_point_probe_curve& PonCC,
                                            Handle(geom_epsilon_base)&                 N,
                                            ActAPI_ProgressEntry                       Journal)
{
  return this->operator()(PonCC.P, PonCC.W2, N, Journal);
}

//! Performs point-edge classification.
//! \param P       [in]     point to classify.
//! \param N       [in]     neighborhood information.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_edge_N_IP::operator()(const gp_Pnt&              P,
                                            Handle(geom_epsilon_base)& N,
                                            ActAPI_ProgressEntry       Journal)
{
  double f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(m_edge, f, l);

  // Project point on curve
  gp_Pnt res_P;
  double res_W, gap;
  geom_project_point_curve ProjPC(C, f, l, m_fPrecision);
  ProjPC(P, res_P, res_W, gap, Journal);

  // Classify
  return this->operator()(P, res_W, N, Journal);
}

//! Performs point-edge classification.
//! \param P       [in]     point to classify.
//! \param W       [in]     parameter of the point on edge.
//! \param N       [in]     neighborhood information.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_edge_N_IP::operator()(const gp_Pnt&              P,
                                            const double               W,
                                            Handle(geom_epsilon_base)& N,
                                            ActAPI_ProgressEntry       Journal)
{
  // Basic classification
  TopoDS_Vertex extremity;
  const geom_membership result = geom_classify_point_edge::operator()(P, extremity, Journal);

  // Get host curve
  double f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(m_edge, f, l);

  // Derive neighborhood
  if ( result == Membership_In )
  {
    gp_Dir Bnd_Norm, S_Norm;

    if ( C->Continuity() >= GeomAbs_C1 )
    {
      // Get route (tangent plus orientation)
      gp_Dir T = ::route(m_edge, m_F, W);

      // ...
      // Get natural normal to the surface
      // ...

      Handle(Geom_Surface) S = BRep_Tool::Surface(m_F);
      ShapeAnalysis_Surface sas(S);
      gp_Pnt2d UV = sas.ValueOfUV(P, m_fPrecision);

      gp_Pnt S_P;
      gp_Vec S_Du, S_Dv;
      S->D1(UV.X(), UV.Y(), S_P, S_Du, S_Dv);
      S_Norm = S_Du.Crossed(S_Dv);

#if defined DRAW_DEBUG
      DRAW_INITGROUP(geom_classify_point_edgeN_SNorm)
      DRAW_LINE3D(P, gp_Pnt(P.XYZ() + S_Norm.XYZ()*500), geom_classify_point_edgeN_SNorm, Draw_violet)
#endif

      // Initialize neighborhood norm with curvature vector
      Bnd_Norm = T.Crossed(S_Norm);

#if defined DRAW_DEBUG
      DRAW_INITGROUP(geom_classify_point_edgeN_BndNorm)
      DRAW_LINE3D(P, gp_Pnt(P.XYZ() + Bnd_Norm.XYZ()*500), geom_classify_point_edgeN_BndNorm, Draw_vert)
#endif
    }
    else
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_point_edgeN.CannotDeriveHeighborhoodForC0Edge" << m_edge );
      return Membership_Unknown;
    }

    // Instantiate neighborhood
    N = new geom_epsilon_face_interior_hemi(Bnd_Norm, S_Norm);
  }
  else if ( result == Membership_On )
  {
    // ...
    // Point falls into singularity which is the edge's vertex. Therefore,
    // in order to construct adequate neighborhood information we need to
    // take into account the second edge
    // ...

    // ...
    // Find another edge which shares the signaled vertex
    // ...

    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(m_F, TopAbs_VERTEX, TopAbs_EDGE, M);

    if ( !M.Contains(extremity) )
    {
      Journal.SendLogMessage( LogWarn(Normal) << "geom_classify_point_edge_N_IP.ExternalExtremity" << extremity );
      return Membership_Unknown;
    }

    // Get route (tangent plus orientation)
    gp_Dir D1_1 = ::route(m_edge, m_F, W);

    // ...
    // Get natural normal to the surface
    // ...

    Handle(Geom_Surface) S = BRep_Tool::Surface(m_F);
    ShapeAnalysis_Surface sas(S);

    // Choose sector by tangents of edges
    const TopTools_ListOfShape& edges = M.FindFromKey(extremity);
    for ( TopTools_ListIteratorOfListOfShape it(edges); it.More(); it.Next() )
    {
      const TopoDS_Edge& neighbor = TopoDS::Edge( it.Value() );

      double f2, l2;
      if ( neighbor.IsSame(m_edge) || BRep_Tool::Curve(neighbor, f2, l2).IsNull() )
        continue;

      // Access geometry
      Handle(Geom_Curve) C2 = BRep_Tool::Curve(neighbor, f2, l2);
      const double W2 = BRep_Tool::Parameter(extremity, neighbor);

      // Route at extremity of E2
      gp_Dir D1_2 = ::route(neighbor, m_F, W2);

      // ...
      // Decide which direction goes first
      // ...

      ShapeAnalysis_Edge sae;
      TopoDS_Vertex neighbor_Vf = sae.FirstVertex(neighbor);
      TopoDS_Vertex neighbor_Vl = sae.LastVertex(neighbor);

      // Apply orientation of wire
      TopTools_IndexedDataMapOfShapeListOfShape M;
      TopExp::MapShapesAndAncestors(m_F, TopAbs_EDGE, TopAbs_WIRE, M);
      const TopTools_ListOfShape& Wires = M.FindFromKey(neighbor);
      for ( TopTools_ListIteratorOfListOfShape it(Wires); it.More(); it.Next() )
      {
        const TopoDS_Wire& W = TopoDS::Wire( it.Value() );
        if ( W.Orientation() == TopAbs_REVERSED )
        {
          TopoDS_Vertex tmp = neighbor_Vf;
          neighbor_Vf = neighbor_Vl;
          neighbor_Vl = tmp;
        }
      }

      gp_Dir T1, T2;
      if ( neighbor_Vf.IsSame(extremity) )
      {
        T1 = D1_1.Reversed();
        T2 = D1_2;
      }
      else
      {
        T1 = D1_2.Reversed();
        T2 = D1_1;
      }

#if defined DRAW_DEBUG
      DRAW_INITGROUP(geom_classify_point_edgeN_ExtremityT)
      DRAW_LINE3D(P, gp_Pnt(P.XYZ() + T1.XYZ()*500), geom_classify_point_edgeN_ExtremityT, Draw_jaune)
      DRAW_LINE3D(P, gp_Pnt(P.XYZ() + T2.XYZ()*500), geom_classify_point_edgeN_ExtremityT, Draw_blanc)
#endif

      // ...
      // We now have to evaluate surface normal, but is generally a very
      // bad idea to do that at face extremity. We'd better make an offset
      // with respect to the known tangents and try to evaluate the surface
      // normal in interior point
      // ...

      gp_XYZ delta = T1.XYZ() + T2.XYZ();

      gp_Pnt P_probe;
      if ( delta.Modulus() < Precision::Confusion() )
      {
        P_probe = P;
      }
      else
      {
        gp_Dir Tm = ( T1.XYZ() + T2.XYZ() )*0.5;
        P_probe = P.XYZ() + Tm.XYZ();
      }

      gp_Pnt2d UV = sas.ValueOfUV(P_probe, m_fPrecision);

      gp_Pnt S_P;
      gp_Vec S_Du, S_Dv;
      S->D1(UV.X(), UV.Y(), S_P, S_Du, S_Dv);
      gp_Dir S_Norm = S_Du.Crossed(S_Dv);

#if defined DRAW_DEBUG
      DRAW_INITGROUP(geom_classify_point_edgeN_SNorm)
      DRAW_LINE3D(P, gp_Pnt(P.XYZ() + S_Norm.XYZ()*500), geom_classify_point_edgeN_SNorm, Draw_violet)
#endif

      // Instantiate neighborhood
      N = new geom_epsilon_face_interior_sector(T1, T2, S_Norm);
      break;
    }
  }

  return result;
}
