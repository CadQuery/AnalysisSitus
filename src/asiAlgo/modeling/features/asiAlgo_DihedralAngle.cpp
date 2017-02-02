//-----------------------------------------------------------------------------
// Created on: 25 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_DihedralAngle.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//! Constructor.
//! \param[in] progress Progress Entry.
//! \param[in] plotter  Imperative Plotter.
asiAlgo_DihedralAngle::asiAlgo_DihedralAngle(ActAPI_ProgressEntry progress,
                                             ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//! Calculates angle between two faces using midpoints to query the normal
//! vectors. Orientation of faces is also taken into account.
//! \param[in]  F                first face.
//! \param[in]  G                second face.
//! \param[in]  allowSmooth      indicates whether to process C1 smooth joints.
//! \param[in]  smoothAngularTol angular tolerance used for recognition
//!                              of smooth dihedral angles. A smooth angle
//!                              may appear to be imperfect by construction,
//!                              but still smooth by the design intent. With
//!                              this parameter you're able to control it.
//! \param[out] commonEdges      common edges.
//! \return angle between faces.
asiAlgo_FeatureAngle
  asiAlgo_DihedralAngle::AngleBetweenFaces(const TopoDS_Face&          F,
                                           const TopoDS_Face&          G,
                                           const bool                  allowSmooth,
                                           const double                smoothAngularTol,
                                           TopTools_IndexedMapOfShape& commonEdges) const
{
  TopoDS_Edge commonEdge = m_commonEdge;
  bool        isSeam     = false;

  // Common edge may be known a priori. If not, we have to find some
  if ( commonEdge.IsNull() )
  {
    if ( F.IsSame(G) )
    {
      // Try to find a seam edge
      for ( TopExp_Explorer exp(F, TopAbs_EDGE); exp.More(); exp.Next() )
      {
        const TopoDS_Edge& edge = TopoDS::Edge( exp.Current() );
        ShapeAnalysis_Edge sae;
        //
        if ( sae.IsSeam(edge, F) )
        {
          isSeam     = true;
          commonEdge = edge;
          commonEdges.Add(edge);
          break;
        }
      }
    }
    else
    {
      // Extract edges for faces
      TopTools_IndexedMapOfShape EdgesF, EdgesG;
      TopExp::MapShapes(F, TopAbs_EDGE, EdgesF);
      TopExp::MapShapes(G, TopAbs_EDGE, EdgesG);

      // Collect common edges
      for ( int ef = 1; ef <= EdgesF.Extent(); ++ef )
      {
        for ( int eg = 1; eg <= EdgesG.Extent(); ++eg )
        {
          if ( EdgesF(ef).IsSame( EdgesG(eg) ) )
          {
            commonEdges.Add( EdgesF(ef) );
            //
            if ( commonEdge.IsNull() ) // A single common edge is enough for analysis
              commonEdge = TopoDS::Edge( EdgesF(ef) );
          }
        }
      }
    }
  }

  if ( commonEdge.IsNull() )
    return Angle_Undefined;

  // Check whether the edge of interest possesses "same parameter" property.
  // If so, we may use its host spatial curves in conjunction with p-curves
  // to sample points on adjacent surfaces. If not, then direction evaluation
  // is not legal anymore, so point inversion problem has to be solved. The
  // latter is quite heavy operation, so it is good that we run it for
  // invalid model only
  const bool isSameParam = BRep_Tool::SameParameter(commonEdge);

  // Get orientation of co-edges on their host faces
  TopAbs_Orientation CumulOriOnF = TopAbs_EXTERNAL, CumulOriOnG = TopAbs_EXTERNAL;
  //
  for ( TopExp_Explorer exp(F, TopAbs_EDGE); exp.More(); exp.Next() )
    if ( exp.Current().IsSame(commonEdge) )
    {
      CumulOriOnF = exp.Current().Orientation();
      break;
    }
  //
  if ( !isSeam )
  {
    for ( TopExp_Explorer exp(G, TopAbs_EDGE); exp.More(); exp.Next() )
      if ( exp.Current().IsSame(commonEdge) )
      {
        CumulOriOnG = exp.Current().Orientation();
        break;
      }
  }
  else
    CumulOriOnG = ( (CumulOriOnF == TopAbs_FORWARD) ? TopAbs_REVERSED : TopAbs_FORWARD );

  // Get host (shared) curve
  double f, l;
  Handle(Geom_Curve) probeCurve = BRep_Tool::Curve(commonEdge, f, l);
  //
  if ( probeCurve.IsNull() )
    return Angle_Undefined;

  // Pick up two points on the curve
  const double midParam  = (f + l)*0.5;
  const double paramStep = (l - f)*0.01;
  const double A_param   = midParam - paramStep;
  const double B_param   = midParam + paramStep;
  //
  gp_Pnt A = probeCurve->Value(A_param);
  gp_Pnt B = probeCurve->Value(B_param);

  this->Plotter().DRAW_POINT(A, Color_Red,  "A");
  this->Plotter().DRAW_POINT(B, Color_Blue, "B");

  // Get host surfaces
  Handle(Geom_Surface) S1 = BRep_Tool::Surface(F),
                       S2 = isSeam ? S1 : BRep_Tool::Surface(G);
  Handle(Geom2d_Curve) c2d_F, c2d_G;

  gp_Pnt2d UV;
  gp_Vec TF, Ref; // Tangent for F
  {
    // Vx
    gp_Vec Vx = B.XYZ() - A.XYZ();
    if ( Vx.Magnitude() < RealEpsilon() )
      return Angle_Undefined;
    //
    if ( CumulOriOnF == TopAbs_REVERSED )
      Vx.Reverse();

    //this->Plotter().DRAW_VECTOR_AT(A, Vx, Color_Red, "Vx");

    // Say (u, v) is the parametric space of S1. Now we take parameters
    // of the point A on this surface
    if ( isSameParam )
    {
      double cons_f, cons_l;
      c2d_F = BRep_Tool::CurveOnSurface(commonEdge, F, cons_f, cons_l);
      //
      if ( c2d_F.IsNull() )
        return Angle_Undefined; // Face is invalid

      UV = c2d_F->Value(A_param);
    }
    else
    {
      ShapeAnalysis_Surface SAS(S1);
      UV = SAS.ValueOfUV(A, 1.0e-7);
    }

    // N (Vz)
    gp_Pnt P;
    gp_Vec D1U, D1V;
    S1->D1(UV.X(), UV.Y(), P, D1U, D1V);
    //
    gp_Vec N = D1U.Crossed(D1V);
    if ( N.Magnitude() < RealEpsilon() )
      return Angle_Undefined;
    //
    if ( F.Orientation() == TopAbs_REVERSED )
      N.Reverse();

    //this->Plotter().DRAW_VECTOR_AT(A, N, Color_Blue, "Vz");

    // Vy
    gp_Vec Vy = N.Crossed(Vx);
    if ( Vy.Magnitude() < RealEpsilon() )
      return Angle_Undefined;
    //
    TF = Vy.Normalized();

    //this->Plotter().DRAW_VECTOR_AT(A, Vy, Color_Green, "Vy");

    // Ref
    Ref = Vx.Normalized();
  }

  gp_Pnt2d ST;
  gp_Vec TG; // Tangent for G
  {
    // Vx
    gp_Vec Vx = B.XYZ() - A.XYZ();
    if ( Vx.Magnitude() < RealEpsilon() )
      return Angle_Undefined;
    //
    if ( CumulOriOnG == TopAbs_REVERSED )
      Vx.Reverse();

    // Say (s, t) is the parametric space of S2. Now we take parameters
    // of the point A on this surface
    if ( isSameParam )
    {
      double cons_f, cons_l;
      c2d_G = BRep_Tool::CurveOnSurface(commonEdge, G, cons_f, cons_l);
      //
      if ( c2d_G.IsNull() )
        return Angle_Undefined; // Face is invalid

      ST = c2d_G->Value(A_param);
    }
    else
    {
      ShapeAnalysis_Surface SAS(S2);
      gp_Pnt2d ST = SAS.ValueOfUV(A, 1.0e-7);
    }

    // N (Vz)
    gp_Pnt P;
    gp_Vec D1S, D1T;
    S2->D1(ST.X(), ST.Y(), P, D1S, D1T);
    //
    gp_Vec N = D1S.Crossed(D1T);
    if ( N.Magnitude() < RealEpsilon() )
      return Angle_Undefined;
    //
    if ( G.Orientation() == TopAbs_REVERSED )
      N.Reverse();

    // Vy
    gp_Vec Vy = N.Crossed(Vx);
    if ( Vy.Magnitude() < RealEpsilon() )
      return Angle_Undefined;
    //
    TG = Vy.Normalized();

    //this->Plotter().DRAW_VECTOR_AT(A, Vy, Color_Violet, "S2_Vy");
  }

  double angle = TF.AngleWithRef(TG, Ref);

#if defined COUT_DEBUG
  std::cout << "Angle is " << angle << std::endl;
#endif

  const double ang_tol = Max(smoothAngularTol, 1.0e-4);
  //
  if ( Abs(Abs(angle) - M_PI) < ang_tol )
  {
    if ( allowSmooth )
      return Angle_Smooth;

    // C1 joint requires additional analysis
    gp_Pnt2d UV_shifted, ST_shifted;
    gp_Pnt   S1_P, S2_P;
    gp_Dir   TF_precised, TG_precised;

    // Let's make small offsets along the in-plane vectors and
    // check geometries in additional sample points. Notice that here
    // we prefer to solve point inversion problem instead of walking in
    // 2D. The matter is that even though evaluation from (u, v) to
    // (x, y, z) is very efficient (far more efficient than point
    // inversion), it is not easy to take into account the inherent
    // 2D vs 3D distortion. If we start from 3D, we ensure that the
    // vectors we check are collinear. If we start from 2D there is no
    // such guarantee.

    S1_P = A.XYZ() + TF.XYZ() * 0.1;
    S2_P = A.XYZ() + TG.XYZ() * 0.1;

    ShapeAnalysis_Surface SAS1(S1), SAS2(S2);
    UV_shifted = SAS1.ValueOfUV(S1_P, 1.0e-3);
    ST_shifted = SAS2.ValueOfUV(S2_P, 1.0e-3);
    S1_P       = SAS1.Value(UV_shifted);
    S2_P       = SAS2.Value(ST_shifted);

    TF_precised = ( S1_P.XYZ() - A.XYZ() );
    TG_precised = ( S2_P.XYZ() - A.XYZ() );

    angle = TF_precised.AngleWithRef(TG_precised, Ref);

    this->Plotter().DRAW_POINT( UV_shifted, Color_Yellow, "S1_UV" );
    this->Plotter().DRAW_POINT( ST_shifted, Color_Red,    "S2_ST" );
    //
    this->Plotter().DRAW_VECTOR_AT(A, TF_precised, Color_Yellow, "TF_precised");
    this->Plotter().DRAW_VECTOR_AT(A, TG_precised, Color_Red,    "TG_precised");
    //
    this->Plotter().DRAW_POINT(S1_P, Color_Yellow, "S1_P");
    this->Plotter().DRAW_POINT(S2_P, Color_Red, "S2_P");
  }

  // Classify angle
  asiAlgo_FeatureAngle angleType = Angle_Undefined;
  if ( angle < 0 )
    angleType = Angle_Convex;
  else
    angleType = Angle_Concave;

  return angleType;
}
