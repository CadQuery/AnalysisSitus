//-----------------------------------------------------------------------------
// Created on: 28 April 2020
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
#include <asiAlgo_Isomorphism.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_FeatureAttrAngle.h>
#include <asiAlgo_Timer.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_Isomorphism::asiAlgo_Isomorphism(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_iNumTests = 0;
  m_bMatchGeomProps = false;
}

//-----------------------------------------------------------------------------

asiAlgo_Isomorphism::asiAlgo_Isomorphism(const Handle(asiAlgo_AAG)& G,
                                         ActAPI_ProgressEntry       progress,
                                         ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_iNumTests = 0;
  m_bMatchGeomProps = false;

  this->InitGraph(G);
}

//-----------------------------------------------------------------------------

void asiAlgo_Isomorphism::InitGraph(const Handle(asiAlgo_AAG)& G_aag)
{
  m_G_aag = G_aag;

  this->fillFacesInfo(m_G_aag, m_faceInfo_G);
}

//-----------------------------------------------------------------------------

void asiAlgo_Isomorphism::SetMatchGeomProps(const bool on)
{
  m_bMatchGeomProps = on;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Isomorphism::Perform(const Handle(asiAlgo_AAG)& P_aag)
{
  // Initialize the AAG of the pattern.
  m_P_aag = P_aag;
  //
  this->fillFacesInfo(m_P_aag, m_faceInfo_P);

  // Clean up the results.
  m_Ms.clear();

  // Reset the number of tests.
  m_iNumTests = 0;

#if defined COUT_DEBUG
  TIMER_NEW
  TIMER_GO
#endif

  // Convert to Eigen matrices. The mappings between the indices are preserved in the member-field maps.
  m_P = m_P_aag->GetNeighborhood().AsEigenMx(m_P_eigenMapping);
  m_G = m_G_aag->GetNeighborhood().AsEigenMx(m_G_eigenMapping);

  // Convert to standard matrices. The mappings between the indices are preserved in the member-field maps.
  m_P_std = m_P_aag->GetNeighborhood().AsStandard(m_P_stdMapping);
  m_G_std = m_G_aag->GetNeighborhood().AsStandard(m_G_stdMapping);

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Convert G and P to the computation-ready matrices")

  TIMER_RESET
  TIMER_GO
#endif

  // Initial bijection.
  Eigen::MatrixXd M0 = this->init_M0();

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Initialize M0")

  TIMER_RESET
  TIMER_GO
#endif

#if defined COUT_DEBUG
  const int K = int( M0.rows() );
  const int N = int( M0.cols() );

  std::cout << "Rows (K): "      << K         << std::endl;
  std::cout << "Columns (N): "   << N         << std::endl;
  std::cout << "N^K threshold: " << Pow(N, K) << std::endl;
#endif

  // Find isomorphisms recursively.
  TColStd_PackedMapOfInteger usedCols;
  this->recurse(0, M0, usedCols);

#if defined COUT_DEBUG
  std::cout << "Num. of conducted tests for isomorphism: " << m_iNumTests << std::endl;
  std::cout << "Num. of found isomorphisms: " << m_Ms.size() << std::endl;

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Find isomorphisms recursively")

  TIMER_RESET
  TIMER_GO
#endif

  // Collect the indices of the feature faces in `G`.
  this->collectFeatures();

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Collect feature faces")
#endif

  return true; // Even if there are no isomorphisms, we return `true` to indicate success.
}

//-----------------------------------------------------------------------------

const std::vector<Eigen::MatrixXd>&
  asiAlgo_Isomorphism::GetIsomorphisms() const
{
  return m_Ms;
}

//-----------------------------------------------------------------------------

const std::vector<TColStd_PackedMapOfInteger>&
  asiAlgo_Isomorphism::GetFeatures() const
{
  return m_features;
}

//-----------------------------------------------------------------------------

TColStd_PackedMapOfInteger asiAlgo_Isomorphism::GetAllFeatures() const
{
  TColStd_PackedMapOfInteger result;
  //
  for ( size_t k = 0; k < m_features.size(); ++k )
    result.Unite(m_features[k]);

  return result;
}

//-----------------------------------------------------------------------------

void asiAlgo_Isomorphism::fillFacesInfo(const Handle(asiAlgo_AAG)&            aag,
                                        NCollection_DataMap<int, t_faceInfo>& map)
{
  // Iterate AAG in whatever order.
  Handle(asiAlgo_AAGRandomIterator)
    it = new asiAlgo_AAGRandomIterator(aag);
  //
  for ( ; it->More(); it->Next() )
  {
    const t_topoId     fid = it->GetFaceId();
    const TopoDS_Face& F   = aag->GetFace(fid);

    TopTools_IndexedMapOfShape verts;
    TopExp::MapShapes(F, TopAbs_VERTEX, verts);

    TopTools_IndexedMapOfShape edges;
    TopExp::MapShapes(F, TopAbs_EDGE, edges);

    TopTools_IndexedMapOfShape wires;
    TopExp::MapShapes(F, TopAbs_WIRE, wires);

    t_faceInfo info;
    info.surf   = BRep_Tool::Surface(F);
    info.nVerts = verts.Extent();
    info.nEdges = edges.Extent();
    info.nWires = wires.Extent();

    // For the trimmed surfaces, use the basis ones.
    if ( info.surf->IsInstance( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) )
    {
      Handle(Geom_RectangularTrimmedSurface)
        RT = Handle(Geom_RectangularTrimmedSurface)::DownCast(info.surf);

      // Use basis surface.
      info.surf = RT->BasisSurface();
    }

    map.Bind(fid, info);
  }
}

//-----------------------------------------------------------------------------

Eigen::MatrixXd asiAlgo_Isomorphism::init_M0() const
{
  const int nRows = m_P_aag->GetNumberOfNodes();
  const int nCols = m_G_aag->GetNumberOfNodes();

  Eigen::MatrixXd M0(nRows, nCols);

  // Iterate rows of P.
  for ( int idx_P = 0; idx_P < nRows; ++idx_P )
  {
    // Iterate rows of G.
    for ( int idx_G = 0; idx_G < nCols; ++idx_G )
    {
      if ( this->areMatching(idx_P, idx_G) )
        M0(idx_P, idx_G) = 1;
      else
        M0(idx_P, idx_G) = 0;
    }
  }

  return M0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Isomorphism::areMatching(const int V_P_eigenIdx,
                                      const int V_G_eigenIdx) const
{
  const t_topoId V_P = m_P_eigenMapping.Find1(V_P_eigenIdx);
  const t_topoId V_G = m_G_eigenMapping.Find1(V_G_eigenIdx);

  const t_faceInfo& info_G = m_faceInfo_G(V_G);
  const t_faceInfo& info_P = m_faceInfo_P(V_P);

  const TColStd_PackedMapOfInteger& row_P     = m_P_aag->GetNeighbors(V_P);
  const int                         valence_P = row_P.Extent();

  const TColStd_PackedMapOfInteger& row_G     = m_G_aag->GetNeighbors(V_G);
  const int                         valence_G = row_G.Extent();

  /* ==============
   *  Heuristic 01.
   * ============== */

  // Check degrees.
  if ( valence_P > valence_G )
    return false;

  /* ==============
   *  Heuristic 02.
   * ============== */

  // If degrees are Ok, we can go further and check that the arc attributes
  // in G contain the arc attribute in P as a subset.

  int P_angles[FeatureAngleType_LAST];
  //
  for ( int k = 0; k < FeatureAngleType_LAST; ++k ) P_angles[k] = 0;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(row_P); nit.More(); nit.Next() )
  {
    const t_topoId nid = nit.Key();

    Handle(asiAlgo_FeatureAttrAngle)
      P_angleAttr = m_P_aag->ATTR_ARC<asiAlgo_FeatureAttrAngle>( asiAlgo_AAG::t_arc(V_P, nid) );

    if ( !P_angleAttr.IsNull() )
      P_angles[ P_angleAttr->GetAngleType() ]++;
  }

  int G_angles[FeatureAngleType_LAST];
  //
  for ( int k = 0; k < FeatureAngleType_LAST; ++k ) G_angles[k] = 0;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger nit(row_G); nit.More(); nit.Next() )
  {
    const t_topoId nid = nit.Key();

    Handle(asiAlgo_FeatureAttrAngle)
      G_angleAttr = m_G_aag->ATTR_ARC<asiAlgo_FeatureAttrAngle>( asiAlgo_AAG::t_arc(V_G, nid) );

    if ( !G_angleAttr.IsNull() )
      G_angles[ G_angleAttr->GetAngleType() ]++;
  }

  for ( int k = 0; k < FeatureAngleType_LAST; ++k )
    if ( G_angles[k] - P_angles[k] < 0 ) // Not a subset.
      return false;

  // Check topology.
  {
    /* ==============
     *  Heuristic 03.
     * ============== */

    if ( info_G.nVerts != info_P.nVerts )
      return false;

    if ( info_G.nEdges != info_P.nEdges )
      return false;

    if ( info_G.nWires != info_P.nWires )
      return false;
  }

  // Check geometry.
  {
    /* ==============
     *  Heuristic 04.
     * ============== */

    // Surface type.
    if ( info_G.surf->DynamicType() != info_P.surf->DynamicType() )
      return false;

    /* ==================
     *  Extra heuristics.
     * ================== */

    // Match props.
    if ( m_bMatchGeomProps )
    {
      if ( info_G.surf->IsKind( STANDARD_TYPE(Geom_CylindricalSurface) ) )
      {
        Handle(Geom_CylindricalSurface) cyl_P = Handle(Geom_CylindricalSurface)::DownCast(info_P.surf);
        Handle(Geom_CylindricalSurface) cyl_G = Handle(Geom_CylindricalSurface)::DownCast(info_G.surf);

        if ( Abs( cyl_P->Radius() - cyl_G->Radius() ) > 1e-3 )
          return false;
      }

      // TODO: add more checks (to implement them in asiAlgo_Utils?)

      // TODO: add abstract rules instead of fully-fledged matching.
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Isomorphism::solutionExists(const Eigen::MatrixXd& M) const
{
  for ( int r = 0; r < M.rows(); ++r )
  {
    bool hasOneElem = false;
    for ( int c = 0; c < M.cols(); ++c )
    {
      if ( M(r, c) == 1 )
      {
        hasOneElem = true;
        break;
      }
    }

    if ( !hasOneElem )
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Isomorphism::isIsomorphism(const Eigen::MatrixXd& M)
{
  m_iNumTests++;

  Eigen::MatrixXd MG = M*(M*m_G).transpose();

  return ( MG == m_P );
}

//-----------------------------------------------------------------------------

void asiAlgo_Isomorphism::prune(Eigen::MatrixXd& M)
{
  // The order of iteration does matter. Simply swapping the following
  // two loops (so that to iterate columns first, instead of rows like
  // it is now) may slow down runtime twice for some cases.
  for ( int r = 0; r < M.rows(); ++r ) // P graph.
  {
    const std::vector<int>& neighbors_P = m_P_std[r];

    for ( int c = 0; c < M.cols(); ++c ) // G graph.
    {
      const std::vector<int>& neighbors_G = m_G_std[c];

      if ( M(r, c) == 1 )
      {
        // The neighbors should also match in M. If not, then
        // the current M(r,c) = 1 matching is impossible, and
        // we have to cancel it.

        bool neighborsMatch = true;
        for ( std::vector<int>::const_iterator P_it = neighbors_P.cbegin();
              P_it != neighbors_P.cend(); P_it++ )
        {
          const int nr = *P_it;

          bool foundNeighborInG = false;
          for ( std::vector<int>::const_iterator G_it = neighbors_G.cbegin();
                G_it != neighbors_G.cend(); G_it++ )
          {
            const int nc = *G_it;

            if ( M(nr, nc) == 1 )
            {
              foundNeighborInG = true;
              break;
            }
          }

          if ( !foundNeighborInG )
          {
            neighborsMatch = false;
            break;
          }
        }

        if ( !neighborsMatch )
          M(r, c) = 0;
      }
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_Isomorphism::recurse(const int                   curRow,
                                  const Eigen::MatrixXd&      M,
                                  TColStd_PackedMapOfInteger& usedCols)
{
  // Check the stopping criterion.
  const int numRows = int( M.rows() );
  //
  if ( curRow == numRows )
  {
#if defined COUT_DEBUG
    // Dump M.
    {
      std::stringstream buff;
      buff << M;
      m_progress.SendLogMessage( LogInfo(Normal) << "Checking isomorphism for M:\n%1"
                                                  << buff.str() );
    }
#endif

    // Dump M.
    /*{
      std::stringstream buff;
      buff << M;
      std::cout << "M: " << buff.str();
    }*/

    const bool isIsomorphism = this->isIsomorphism(M);

#if defined COUT_DEBUG
    m_progress.SendLogMessage( LogInfo(Normal) << "Isomorphism: %1."
                                               << isIsomorphism );
#endif

    if ( isIsomorphism )
    {
      m_Ms.push_back(M);
    }

    return; // Do not allow further checks.
  }

  const int numCols = int( M.cols() );

  // Adjust matrix M.
  Eigen::MatrixXd MM = M;
  this->prune(MM);

  // After pruning, some matrices may encode no bijections.
  if ( !this->solutionExists(MM) )
    return;

  Eigen::MatrixXd M_next = MM;

  // Eliminate 1's.
  for ( int c = 0; c < numCols; ++c )
  {
    if ( usedCols.Contains(c) )
      continue;

    if ( M_next(curRow, c) == 0 )
      continue;

    // Modify matrix.
    if ( M_next(curRow, c) == 1 )
    {
      // Nullify other columns.
      for ( int cc = 0; cc < numCols; ++cc )
        if ( cc != c )
          M_next(curRow, cc) = 0;

      // Nullify other rows.
      for ( int rr = 0; rr < numRows; ++rr )
        if ( rr != curRow )
          M_next(rr, c) = 0;
    }

    // Proceed recursively.
    usedCols.Add(c);
    //
    this->recurse(curRow + 1, M_next, usedCols);
    //
    usedCols.Remove(c);

    // Reset the modifications done on the matrix M.
    for ( int cc = 0; cc < numCols; ++cc )
      M_next(curRow, cc) = M(curRow, cc);
    //
    for ( int rr = 0; rr < numRows; ++rr )
      if ( rr != curRow )
        M_next(rr, c) = M(rr, c);
  }
}

//-----------------------------------------------------------------------------

t_topoId asiAlgo_Isomorphism::getDomainImage(const int              V_P_eigenIdx,
                                             const Eigen::MatrixXd& M) const
{
  const int numCols = int( M.cols() );
  for ( int c = 0; c < numCols; ++c )
  {
    if ( M(V_P_eigenIdx, c) == 1 )
      return m_G_eigenMapping.Find1(c); // Use G mapping as G is a universum.
  }

  return 0; // Invalid index.
}

//-----------------------------------------------------------------------------

void asiAlgo_Isomorphism::collectFeatures()
{
  // Loop over the found isomorphisms.
  for ( size_t i = 0; i < m_Ms.size(); ++i )
  {
    bool isConfirmed = true;

    TColStd_PackedMapOfInteger candidates;

    // Loop over the arcs of the pattern graph.
    const asiAlgo_AAG::t_arc_attributes& P_arcAttrs = m_P_aag->GetArcAttributes();
    //
    for ( asiAlgo_AAG::t_arc_attributes::Iterator ait(P_arcAttrs); ait.More(); ait.Next() )
    {
      // Get the arc from the pattern graph.
      const asiAlgo_AAG::t_arc& P_arc = ait.Key();
      //
      Handle(asiAlgo_FeatureAttrAngle)
        P_angleAttr = Handle(asiAlgo_FeatureAttrAngle)::DownCast( ait.Value() );

      const t_topoId imF1 = this->getDomainImage(m_P_eigenMapping.Find2(P_arc.F1), m_Ms[i]);
      const t_topoId imF2 = this->getDomainImage(m_P_eigenMapping.Find2(P_arc.F2), m_Ms[i]);

      candidates.Add(imF1);
      candidates.Add(imF2);

      // Get the image of the arc, i.e. the arc in the problem graph.
      asiAlgo_AAG::t_arc G_arc(imF1, imF2);
      //
      Handle(asiAlgo_FeatureAttrAngle)
        G_angleAttr = m_G_aag->ATTR_ARC<asiAlgo_FeatureAttrAngle>(G_arc);

      // Compare the attributes.
      if ( ( P_angleAttr.IsNull() || G_angleAttr.IsNull() ) ||
           ( P_angleAttr->GetAngleType() != G_angleAttr->GetAngleType() ) )
      {
        isConfirmed = false;
        break;
      }
    }

    // Treat special case of one-node pattern.
    if ( m_P_aag->GetNumberOfNodes() == 1 )
    {
      Handle(asiAlgo_FeatureAttrAngle)
        P_angleAttr = m_P_aag->ATTR_NODE<asiAlgo_FeatureAttrAngle>(1);

      const t_topoId imF = this->getDomainImage(m_P_eigenMapping.Find2(0), m_Ms[i]);

      candidates.Add(imF);

      // Get the corresponding node attribute from the image in G.
      Handle(asiAlgo_FeatureAttrAngle)
        G_angleAttr = m_G_aag->ATTR_NODE<asiAlgo_FeatureAttrAngle>(imF);

      // Compare the attributes.
      if ( ( P_angleAttr.IsNull() || G_angleAttr.IsNull() ) ||
           ( P_angleAttr->GetAngleType() != G_angleAttr->GetAngleType() ) )
      {
        isConfirmed = false;
      }
    }

    if ( isConfirmed && !candidates.IsEmpty() )
    {
      m_features.push_back(candidates);
    }
  }
}
