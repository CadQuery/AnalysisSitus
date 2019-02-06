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

// Own include
#include <asiAlgo_FindSpringEdges.h>

// asiAlgo includes
#include <asiAlgo_FeatureAttrAdjacency.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_SLProps.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_FindSpringEdges::asiAlgo_FindSpringEdges(const TopoDS_Shape&        masterCAD,
                                                 const Handle(asiAlgo_AAG)& aag,
                                                 ActAPI_ProgressEntry       progress,
                                                 ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_master          ( masterCAD ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

asiAlgo_FindSpringEdges::asiAlgo_FindSpringEdges(const Handle(asiAlgo_AAG)& aag,
                                                 ActAPI_ProgressEntry       progress,
                                                 ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_master          ( aag->GetMasterCAD() ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

bool asiAlgo_FindSpringEdges::PerformForFace(const int                         face_idx,
                                             const TColStd_PackedMapOfInteger& smooth_edge_ids,
                                             bool&                             isCandidateBlend,
                                             double&                           candidateRadius)
{
  // Build master AAG
  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

    m_aag = new asiAlgo_AAG(m_master);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  ActAPI_PlotterEntry IV( this->GetPlotter() );

  // Initial guess is that this face looks like a blend
  isCandidateBlend = false;

  // Get face of interest and its host surface
  const TopoDS_Face&  A  = m_aag->GetFace(face_idx);
  BRepAdaptor_Surface AS(A);

  // Get all smoothly connected neighbor faces
  TColStd_PackedMapOfInteger smoothNeighbors = m_aag->GetNeighbors(face_idx, smooth_edge_ids);
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(smoothNeighbors); nit.More(); nit.Next() )
  {
    // Get neighbor face and its host surface
    const int           neighbor_idx = nit.Key();
    const TopoDS_Face&  B            = m_aag->GetFace(neighbor_idx);
    BRepAdaptor_Surface BS(B);

    // Pick up just any common edge between the two faces. This edges is by
    // construction a smooth edge
    Handle(asiAlgo_FeatureAttrAdjacency)
      attr = Handle(asiAlgo_FeatureAttrAdjacency)::DownCast( m_aag->GetArcAttribute( asiAlgo_AAG::t_arc(face_idx, neighbor_idx) ) );
    //
    const TColStd_PackedMapOfInteger& commonEdgeIndices = attr->GetEdgeIndices();
    //
    if ( commonEdgeIndices.IsEmpty() )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "Empty common edges attribute for adjacent faces." );
      return false;
    }
    //
    const TopoDS_Edge&
      E = TopoDS::Edge( m_aag->RequestMapOfEdges().FindKey( commonEdgeIndices.GetMinimalMapped() ) );

    // Get a host curve of the common edge and pick up a midpoint (probe point)
    // to analyze the differential properties of the neighbor faces. We also
    // need a local tangent direction on the edge
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);
    //
    if ( C.IsNull() )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "No 3D curve in the probe edge." );
      return false;
    }
    //
    const double midParam  = (f + l)*0.5;
    const double paramStep = (l - f)*0.1;
    const double A_param   = midParam - paramStep;
    const double B_param   = midParam + paramStep;
    //
    gp_Pnt P  = C->Value(midParam);
    gp_Pnt P1 = C->Value(A_param);
    gp_Pnt P2 = C->Value(B_param);
    //
    gp_Vec T = P2.XYZ() - P1.XYZ();
    if ( T.Magnitude() < RealEpsilon() )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "Irregular parameterization of the probe edge." );
      return false;
    }

    // Solve point inversion problem for P on A
    gp_Pnt2d UV;
    {
      ShapeAnalysis_Surface SAS( AS.Surface().Surface() );
      UV = SAS.ValueOfUV(P, 1.0e-4);
    }

    // Solve point inversion problem for P on B
    gp_Pnt2d ST;
    {
      ShapeAnalysis_Surface SAS( BS.Surface().Surface() );
      ST = SAS.ValueOfUV(P, 1.0e-4);
    }

    // Calculate differential properties
    BRepLProp_SLProps AProps(AS, UV.X(), UV.Y(), 2, 1e-7);
    BRepLProp_SLProps BProps(BS, ST.X(), ST.Y(), 2, 1e-7);
    //
    if ( !AProps.IsCurvatureDefined() )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "Curvature is not defined for A surface." );
      return false;
    }
    if ( !BProps.IsCurvatureDefined() )
    {
      this->GetProgress().SendLogMessage( LogErr(Normal) << "Curvature is not defined for B surface." );
      return false;
    }

    gp_Dir A_minD, A_maxD, B_minD, B_maxD;
    //
    AProps.CurvatureDirections(A_maxD, A_minD);
    BProps.CurvatureDirections(B_maxD, B_minD);
    //
    const double A_minK = AProps.MinCurvature();
    const double A_maxK = AProps.MaxCurvature();
    const double B_minK = BProps.MinCurvature();
    const double B_maxK = BProps.MaxCurvature();

    double a1, a2, b1, b2;
    gp_Dir a1_dir, a2_dir, b1_dir, b2_dir;
    //
    if ( Abs( A_minD.Dot(T) ) > Abs( A_maxD.Dot(T) ) )
    {
      a1_dir = A_minD;
      a1     = A_minK;
      a2_dir = A_maxD;
      a2     = A_maxK;
    }
    else
    {
      a1_dir = A_maxD;
      a1     = A_maxK;
      a2_dir = A_minD;
      a2     = A_minK;
    }
    //
    if ( Abs( B_minD.Dot(T) ) > Abs( B_maxD.Dot(T) ) )
    {
      b1_dir = B_minD;
      b1     = B_minK;
      b2_dir = B_maxD;
      b2     = B_maxK;
    }
    else
    {
      b1_dir = B_maxD;
      b1     = B_maxK;
      b2_dir = B_minD;
      b2     = B_minK;
    }

#if defined COUT_DEBUG
    std::cout << "\t[Face #" << face_idx     << "] a1 = " << a1 << std::endl;
    std::cout << "\t[Face #" << face_idx     << "] a2 = " << a2 << std::endl;
    std::cout << "\t[Face #" << neighbor_idx << "] b1 = " << b1 << std::endl;
    std::cout << "\t[Face #" << neighbor_idx << "] b2 = " << b2 << std::endl;
    std::cout << "---" << std::endl;
#endif

    // Self-curvature test
    if ( a1*a2 >= 0 && Abs(a2) > Abs(a1) )
    {
      // Neighbor-curvature test
      if ( Abs(a2) > Abs(b2) )
      {
        // A is more likely to be a blend since blends usually have higher
        // curvature than the support faces
        if ( !isCandidateBlend )
        {
          isCandidateBlend = true;
          candidateRadius  = 1.0 / Abs(a2);
        }
        //
        m_result.edges.Add(E);
      }
    }
  }

  // Recover IDs
  this->getIds(m_result.edges, m_result.ids);

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_FindSpringEdges::getIds(const TopTools_IndexedMapOfShape& edges,
                                     TColStd_PackedMapOfInteger&       indices) const
{
  const TopTools_IndexedMapOfShape& allEdges = m_aag->RequestMapOfEdges();
  //
  for ( int eidx = 1; eidx <= edges.Extent(); ++eidx )
  {
    const TopoDS_Shape& edgeOfInterest = edges(eidx);
    indices.Add( allEdges.FindIndex(edgeOfInterest) );
  }
}
