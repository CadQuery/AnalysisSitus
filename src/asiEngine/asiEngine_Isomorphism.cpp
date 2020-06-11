//-----------------------------------------------------------------------------
// Created on: 01 June 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiEngine_Isomorphism.h>

// asiAlgo includes
#include <asiAlgo_Isomorphism.h>
#include <asiAlgo_Timer.h>

//-----------------------------------------------------------------------------

bool asiEngine_Isomorphism::Compute(const TCollection_AsciiString& featureName,
                                    asiAlgo_Feature&               featureFaces,
                                    const int                      flags)
{
  // Get problem graph.
  Handle(asiAlgo_AAG) G = m_model->GetPartNode()->GetAAG();
  //
  if ( G.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "AAG is null.");
    return false;
  }

  // Get feature model.
  Handle(asiData_IVTopoItemNode)
    featureNode = Handle(asiData_IVTopoItemNode)::DownCast( m_model->FindNodeByName(featureName) );
  //
  if ( featureNode.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1."
                                             << featureName);
    return false;
  }
  //
  TopoDS_Shape        featureShape = featureNode->GetShape();
  Handle(asiAlgo_AAG) P            = new asiAlgo_AAG(featureShape);

  // Dump P.
  if ( P->GetNumberOfNodes() < 50 ) // Limit for printing.
  {
    asiAlgo_AdjacencyMx::t_indexMap mapping;

    std::stringstream buff;
    buff << P->GetNeighborhood().AsEigenMx(mapping);
    m_progress.SendLogMessage( LogInfo(Normal) << "P:\n%1" << buff.str() );
    std::cout << "P:\n" << buff.str() << std::endl;
  }

  /* ======================
   *  Reduce problem graph.
   * ====================== */

  asiAlgo_Feature faces2Exclude;

  // Reduce AAG if requested.
  if ( flags & ExcludeConvexOnly )
  {
    asiAlgo_Feature convexOnlyFaces;
    G->FindConvexOnly(convexOnlyFaces);
    faces2Exclude.Unite(convexOnlyFaces);

    m_progress.SendLogMessage( LogInfo(Normal) << "Num. of convex faces to exclude: %1."
                                               << convexOnlyFaces.Extent() );
  }
  //
  if ( flags & ExcludeBase )
  {
    asiAlgo_Feature baseOnlyFaces;
    G->FindBaseOnly(baseOnlyFaces);
    faces2Exclude.Unite(baseOnlyFaces);

    m_progress.SendLogMessage( LogInfo(Normal) << "Num. of base faces to exclude: %1."
                                               << baseOnlyFaces.Extent() );
  }

  if ( faces2Exclude.Extent() > 0 )
  {
    G->PushSubgraphX(faces2Exclude);
  }

  std::vector<asiAlgo_Feature> ccomps;
  G->GetConnectedComponents(ccomps);

  /* =====================================================
   *  Search for isomorphisms in each connected component.
   * ===================================================== */

  m_progress.SetMessageKey("Searching for isomorphisms");
  m_progress.Init( ccomps.size() == 1 ? INT_MAX : int( ccomps.size() ) );

  std::cout << "There are " << int( ccomps.size() )<< " connected components in the AAG to analyze." << std::endl;
  m_progress.SendLogMessage( LogInfo(Normal) << "There are %1 connected components to analyze."
                                             << int( ccomps.size() ) );

  for ( auto cit = ccomps.cbegin(); cit != ccomps.cend(); ++cit )
  {
    // Recognize features in each connected component separately.
    G->PushSubgraph(*cit);
    {
      if ( flags & Verbose )
      {
        m_progress.SendLogMessage( LogInfo(Normal) << "Number of nodes in G: %1."
                                                   << G->GetNumberOfNodes() );

        // Dump G.
        if ( G->GetNumberOfNodes() < 50 )
        {
          asiAlgo_AdjacencyMx::t_indexMap mapping;

          std::stringstream buff;
          buff << G->GetNeighborhood().AsEigenMx(mapping);
          m_progress.SendLogMessage( LogInfo(Normal) << "G:\n%1" << buff.str() );
          std::cout << "G:\n" << buff.str() << std::endl;
        }
      }

      // Prepare isomorphism algo.
      asiAlgo_Isomorphism isomorphism(G, m_progress, m_plotter);

      // Find isomorphisms.
      if ( !isomorphism.Perform(P) )
        continue;

      // Get bijection matrices.
      const std::vector<Eigen::MatrixXd>& isos = isomorphism.GetIsomorphisms();

      if ( flags & Verbose )
      {
        m_progress.SendLogMessage( LogInfo(Normal) << "Found %1 isomorphism(s)."
                                                   << int( isos.size() ) );
      }

      // Get all found feature faces.
      featureFaces.Unite( isomorphism.GetAllFeatures() );
    }
    G->PopSubgraph();

    // Step progress and check for cancellation.
    m_progress.StepProgress(1);
    //
    if ( m_progress.IsCancelling() )
    {
      m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Canceled);
      G->PopSubgraphs();

      return false;
    }
  }

  // Restore the full graph if it has been reduced.
  G->PopSubgraphs();

  m_progress.SetProgressStatus(ActAPI_ProgressStatus::Progress_Succeeded);
  return true;
}
