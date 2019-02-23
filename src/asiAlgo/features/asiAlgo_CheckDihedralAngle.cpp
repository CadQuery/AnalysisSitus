//-----------------------------------------------------------------------------
// Created on: 25 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

// Own include
#include <asiAlgo_CheckDihedralAngle.h>

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

//-----------------------------------------------------------------------------

asiAlgo_CheckDihedralAngle::asiAlgo_CheckDihedralAngle(ActAPI_ProgressEntry progress,
                                                       ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_FeatureAngleType
  asiAlgo_CheckDihedralAngle::AngleBetweenFaces(const TopoDS_Face&          F,
                                                const TopoDS_Face&          G,
                                                const bool                  allowSmooth,
                                                const double                smoothAngularTol,
                                                TopTools_IndexedMapOfShape& commonEdges,
                                                double&                     angRad) const
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
    return FeatureAngleType_Undefined;

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
    return FeatureAngleType_Undefined;

  // Pick up two points on the curve
  const double midParam  = (f + l)*0.5;
  const double paramStep = (l - f)*0.01;
  const double A_param   = midParam - paramStep;
  const double B_param   = midParam + paramStep;
  //
  gp_Pnt A = probeCurve->Value(A_param);
  gp_Pnt B = probeCurve->Value(B_param);

  this->GetPlotter().DRAW_POINT(A, Color_Red,  "A");
  this->GetPlotter().DRAW_POINT(B, Color_Blue, "B");

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
      return FeatureAngleType_Undefined;
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
        return FeatureAngleType_Undefined; // Face is invalid

      UV = c2d_F->Value(A_param);
    }
    else
    {
      ShapeAnalysis_Surface SAS(S1);
      UV = SAS.ValueOfUV(A, 1.0e-1);
    }

    // N (Vz)
    gp_Pnt P;
    gp_Vec D1U, D1V;
    S1->D1(UV.X(), UV.Y(), P, D1U, D1V);
    //
    gp_Vec N = D1U.Crossed(D1V);
    if ( N.Magnitude() < RealEpsilon() )
      return FeatureAngleType_Undefined;
    //
    if ( F.Orientation() == TopAbs_REVERSED )
      N.Reverse();

    //this->Plotter().DRAW_VECTOR_AT(A, N, Color_Blue, "Vz");

    // Vy
    gp_Vec Vy = N.Crossed(Vx);
    if ( Vy.Magnitude() < RealEpsilon() )
      return FeatureAngleType_Undefined;
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
      return FeatureAngleType_Undefined;
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
        return FeatureAngleType_Undefined; // Face is invalid

      ST = c2d_G->Value(A_param);
    }
    else
    {
      ShapeAnalysis_Surface SAS(S2);
      ST = SAS.ValueOfUV(A, 1.0e-1);
    }

    // N (Vz)
    gp_Pnt P;
    gp_Vec D1S, D1T;
    S2->D1(ST.X(), ST.Y(), P, D1S, D1T);
    //
    gp_Vec N = D1S.Crossed(D1T);
    if ( N.Magnitude() < RealEpsilon() )
      return FeatureAngleType_Undefined;
    //
    if ( G.Orientation() == TopAbs_REVERSED )
      N.Reverse();

    // Vy
    gp_Vec Vy = N.Crossed(Vx);
    if ( Vy.Magnitude() < RealEpsilon() )
      return FeatureAngleType_Undefined;
    //
    TG = Vy.Normalized();

    //this->Plotter().DRAW_VECTOR_AT(A, Vy, Color_Violet, "S2_Vy");
  }

  // Calculate dihedral angle
  angRad = TF.AngleWithRef(TG, Ref);

#if defined COUT_DEBUG
  std::cout << "Angle is " << angle << std::endl;
#endif

  // 3 degrees is the default angular tolerance to recognize smooth angles.
  const double ang_tol = Max(smoothAngularTol, 3.0/180.0*M_PI);
  //
  if ( Abs(Abs(angRad) - M_PI) < ang_tol )
  {
    if ( allowSmooth )
      return FeatureAngleType_Smooth;

    // C1 joint requires additional analysis
    gp_Pnt2d UV_shifted, ST_shifted;
    gp_Pnt   S1_P, S2_P;
    gp_Vec   TF_precised_vec, TG_precised_vec;
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

    // Take average by several samples to be more robust. Initially, we do not
    // know how far we have to go into the face interior to make reliable
    // measurement.
    int    inSamples   = 1;
    double inStep      = 0.4;
    double avrAngleRad = 0.0;
    //
    for ( int s = 1; s <= inSamples; ++s )
    {
      const double inDelta = inStep*s;

      S1_P = A.XYZ() + TF.XYZ() * inDelta;
      S2_P = A.XYZ() + TG.XYZ() * inDelta;

      ShapeAnalysis_Surface SAS1(S1), SAS2(S2);
      UV_shifted = SAS1.ValueOfUV(S1_P, 1.0e-1);
      ST_shifted = SAS2.ValueOfUV(S2_P, 1.0e-1);
      S1_P       = SAS1.Value(UV_shifted);
      S2_P       = SAS2.Value(ST_shifted);

      // For invalid models, we need to protect against null vectors.
      TF_precised_vec = ( S1_P.XYZ() - A.XYZ() );
      TG_precised_vec = ( S2_P.XYZ() - A.XYZ() );
      //
      if ( TF_precised_vec.Magnitude() < Precision::Confusion() ||
           TG_precised_vec.Magnitude() < Precision::Confusion() )
        return FeatureAngleType_Undefined;
      //
      TF_precised = TF_precised_vec;
      TG_precised = TG_precised_vec;

      avrAngleRad += TF_precised.AngleWithRef(TG_precised, Ref);

      this->GetPlotter().DRAW_POINT( UV_shifted, Color_Yellow, "S1_UV" );
      this->GetPlotter().DRAW_POINT( ST_shifted, Color_Red,    "S2_ST" );
      //
      this->GetPlotter().DRAW_VECTOR_AT(A, TF_precised, Color_Yellow, "TF_precised");
      this->GetPlotter().DRAW_VECTOR_AT(A, TG_precised, Color_Red,    "TG_precised");
      //
      this->GetPlotter().DRAW_POINT(S1_P, Color_Yellow, "S1_P");
      this->GetPlotter().DRAW_POINT(S2_P, Color_Red, "S2_P");
    }
    //
    avrAngleRad /= inSamples;
    angRad = avrAngleRad;
  }

  // Classify angle
  asiAlgo_FeatureAngleType angleType = FeatureAngleType_Undefined;
  if ( angRad < 0 )
    angleType = FeatureAngleType_Convex;
  else
    angleType = FeatureAngleType_Concave;

  return angleType;
}

//-----------------------------------------------------------------------------

asiAlgo_FeatureAngleType
  asiAlgo_CheckDihedralAngle::AngleBetweenFaces(const TopoDS_Face&          F,
                                                const TopoDS_Face&          G,
                                                const bool                  allowSmooth,
                                                const double                smoothAngularTol,
                                                TopTools_IndexedMapOfShape& commonEdges) const
{
  double angRad = 0.0;
  return this->AngleBetweenFaces(F, G, allowSmooth, smoothAngularTol, commonEdges, angRad);
}

//-----------------------------------------------------------------------------

asiAlgo_FeatureAngleType
  asiAlgo_CheckDihedralAngle::AngleBetweenFaces(const TopoDS_Face& F,
                                                const TopoDS_Face& G,
                                                const bool         allowSmooth,
                                                const double       smoothAngularTol) const
{
  TopTools_IndexedMapOfShape commonEdges;
  double angRad = 0.0;
  return this->AngleBetweenFaces(F, G, allowSmooth, smoothAngularTol, commonEdges, angRad);
}

//-----------------------------------------------------------------------------

asiAlgo_FeatureAngleType
  asiAlgo_CheckDihedralAngle::AngleBetweenFaces(const TopoDS_Face& F,
                                                const TopoDS_Face& G,
                                                const bool         allowSmooth,
                                                const double       smoothAngularTol,
                                                double&            angRad) const
{
  TopTools_IndexedMapOfShape commonEdges;
  return this->AngleBetweenFaces(F, G, allowSmooth, smoothAngularTol, commonEdges, angRad);
}
