//-----------------------------------------------------------------------------
// Created on: 02 June 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiAlgo_Cloudify.h>

// Geometry includes
#include <asiAlgo_ClassifyPointFace.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <gce_MakeCirc.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param uv_step  [in] sampling step.
//! \param progress [in] Progress Notifier.
//! \param plotter  [in] Imperative Plotter.
asiAlgo_Cloudify::asiAlgo_Cloudify(const double         uv_step,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm     (progress, plotter),
  m_fLinStep            (uv_step),
  m_bUseExternalUVSteps (false),
  m_fUStep              (0.0),
  m_fVStep              (0.0)
{}

//-----------------------------------------------------------------------------

//! Builds a representative point cloud for the surfaces of the given model.
//! \param model       [in]  target CAD model.
//! \param point_cloud [out] result point cloud.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_Cloudify::Sample_Faces(const TopoDS_Shape&                model,
                                    Handle(asiAlgo_BaseCloud<double>)& point_cloud)
{
  point_cloud = new asiAlgo_BaseCloud<double>;

  { // Progress [begin]
    TopTools_IndexedMapOfShape M;
    TopExp::MapShapes(model, TopAbs_FACE, M);
    //
    this->GetProgress().Init( M.Extent() );
  } // Progress [end]

  // Iterate over the faces, and sample points in their domains
  for ( TopExp_Explorer exp(model, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( exp.Current() );

    // Surface adaptor
    BRepAdaptor_Surface bas(face);

    // Get parametric bounds
    double uMin, uMax, vMin, vMax;
    BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);

    // Choose an adequate sampling step in parametric space for the
    // current face. A user is not aware of any parametric spaces as a rule,
    // so we'd better treat his sampling argument as a linear metric
    // property in three dimensions. Obviously, some distortion
    // coefficient has to be applied to stick to this metric constraint
    // in 3D when we are sampling 2D

    // We do not want to use curvature since parameterization of a host
    // surface may happen to be irregular. So we are using D0-wise
    // heuristic, like we pick up two points, take the one in-between,
    // and pass a circle using three point. The radius of that circle
    // we take as a curvature radius. Having this radius R, it is easy
    // to derive an angle giving us a certain arc length:
    // L = alpha * R => alpha = L / R

    double uStep, vStep;
    //
    if ( m_bUseExternalUVSteps )
    {
      uStep = m_fUStep;
      vStep = m_fVStep;
    }
    else
    {
      uStep = this->chooseParametricStep(bas, true, uMin, uMax, vMin, vMax);
      vStep = this->chooseParametricStep(bas, false, uMin, uMax, vMin, vMax);
    }

    // Prepare classifier
    asiAlgo_ClassifyPointFace classifier(face, BRep_Tool::Tolerance(face), 0.01);

    // Sample points
    double u = uMin;
    bool uStop = false;
    while ( !uStop )
    {
      if ( u > uMax )
      {
        u     = uMax;
        uStop = true;
      }

      double v = vMin;
      bool vStop = false;
      while ( !vStop )
      {
        if ( v > vMax )
        {
          v     = vMax;
          vStop = true;
        }

        // Perform point membership classification
        asiAlgo_Membership pmc = classifier( gp_Pnt2d(u, v) );
        //
        if ( pmc & Membership_InOn )
        {
          gp_Pnt P = bas.Value(u, v);
          point_cloud->AddElement( P.X(), P.Y(), P.Z() );
        }

        v += vStep;
      }

      u += uStep;
    }

    // Progress
    this->GetProgress().StepProgress(1);
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Builds a representative point cloud for the facets of the given model.
//! \param model       [in]  target CAD model to take facets from.
//! \param point_cloud [out] result point cloud.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_Cloudify::Sample_Facets(const TopoDS_Shape&                model,
                                     Handle(asiAlgo_BaseCloud<double>)& point_cloud)
{
  point_cloud = new asiAlgo_BaseCloud<double>;

  // Constants
  const double lower = 0.0, upper = 1.0;

  // Loop over the facets
  for ( TopExp_Explorer exp(model, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );

    // Ask for the facets belonging to the given face
    TopLoc_Location L;
    const Handle(Poly_Triangulation)& T = BRep_Tool::Triangulation(F, L);
    //
    if ( T.IsNull() )
      continue;

    // Take data arrays
    const TColgp_Array1OfPnt&   nodes = T->Nodes();
    const Poly_Array1OfTriangle& tris = T->Triangles();

    // Loop over the array of triangles, so that we can work with each
    // individual facet independently
    for ( int t = 1; t <= tris.Length(); ++t )
    {
      const Poly_Triangle& tri = tris(t);

      // Take triangle's nodes
      int n[3];
      tri.Get(n[0], n[1], n[2]);

      // Check out the nodes
      gp_XYZ r[3] = { nodes(n[0]).XYZ(), nodes(n[1]).XYZ(), nodes(n[2]).XYZ() };

      // There are two parameters for sampling. One runs from r[1] to r[2],
      // while the second runs from r[0] to the intermediate point between
      // r[1] and r[2] (defined by the first parameter). The first parameter
      // is alpha, the second is beta
      double alpha   = lower;
      bool alphaStop = false;
      //
      while ( !alphaStop )
      {
        if ( alpha > upper )
        {
          alpha     = upper;
          alphaStop = true;
        }

        double beta   = lower;
        bool betaStop = false;
        //
        while ( !betaStop )
        {
          if ( beta > upper )
          {
            beta     = upper;
            betaStop = true;
          }

          gp_XYZ P = r[0] + beta*(r[1] + alpha*(r[2] - r[1]) - r[0]);
          //
          point_cloud->AddElement( P.X(), P.Y(), P.Z() );

          beta += m_fLinStep;
        }

        alpha += m_fLinStep;
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

//! This method allows to choose a parametric step in adaptive way, basing on
//! the linear step in 3D and the curvature radius of the host surface.
//! \param bas  [in] host surface.
//! \param isU  [in] true for U, false for V.
//! \param uMin [in] minimal U.
//! \param uMax [in] maximal U.
//! \param vMin [in] minimal V.
//! \param vMax [in] maximal V.
//! \return calculated parametric step.
double asiAlgo_Cloudify::chooseParametricStep(const BRepAdaptor_Surface& bas,
                                              const bool                 isU,
                                              const double               uMin,
                                              const double               uMax,
                                              const double               vMin,
                                              const double               vMax) const
{
  const double tMin = isU ? uMin : vMin;
  const double tMax = isU ? uMax : vMax;

  double step;
  //
  {
    const double dt = (tMax - tMin)*0.01;
    const double t0 = tMin;
    const double t1 = tMin + dt;
    const double tm = tMin + dt*0.5;
    //
    const gp_Pnt P0 = isU ? bas.Value(t0, vMin) : bas.Value(uMin, t0);
    const gp_Pnt P1 = isU ? bas.Value(t1, vMin) : bas.Value(uMin, t1);
    const gp_Pnt Pm = isU ? bas.Value(tm, vMin) : bas.Value(uMin, tm);
    //
    bool isOk;
    double R = 0.0;
    try
    {
      gce_MakeCirc mkCirc(P0, P1, Pm);
      isOk = mkCirc.IsDone();
      R = mkCirc.Value().Radius();
    }
    catch ( ... )
    {
      isOk = false;
    }
    //
    if ( isOk )
    {
      // If we're not done, it means that our points are collinear, so
      // there is no sense to apply heuristic here; just use what the user
      // has given us
      step = ( R > RealEpsilon() ) ? (m_fLinStep / R) : m_fLinStep;
    }
    else
    {
      step = m_fLinStep;
    }
  }

  return step;
}
