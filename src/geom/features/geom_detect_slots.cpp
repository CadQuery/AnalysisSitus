//-----------------------------------------------------------------------------
// Created on: 23 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_detect_slots.h>

// Geometry includes
#include <geom_aag_iterator.h>
#include <geom_aag_vtk.h>
#include <geom_feature_pattern_slot.h>

// Visualization includes
#include <visu_topo_graph.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#undef GRAPH_DEBUG

void SHOW_GRAPH(const Handle(geom_aag)& aag)
{
  // Show graph
  visu_topo_graph* pGraphView = new visu_topo_graph;
  pGraphView->SetAAG(aag);
  vtkSmartPointer<vtkMutableUndirectedGraph> undirected = geom_aag_vtk::Convert(aag);
  pGraphView->Render(undirected, TopoDS_Shape(), visu_topo_graph::Regime_AAG);
}

//! Constructor.
//! \param masterCAD [in] full CAD model.
geom_detect_slots::geom_detect_slots(const TopoDS_Shape& masterCAD)
{
  // Build master AAG
  m_aag = new geom_aag(masterCAD);

#if defined GRAPH_DEBUG
  SHOW_GRAPH(m_aag);
#endif
}

//! Performs identification.
//! \return true in case of success, false -- otherwise.
bool geom_detect_slots::Perform()
{
  // Clean up the result
  m_result.Clear();

  //---------------------------------------------------------------------------
  // STAGE 1: remove convex-only faces
  //---------------------------------------------------------------------------

  TopTools_IndexedMapOfShape convex;
  //
  m_aag->FindConvexOnly(convex);
  m_aag->Remove(convex);

  // Iteration utilities
  Handle(geom_aag_random_iterator)  seed_it    = new geom_aag_random_iterator(m_aag);
  Handle(geom_feature_pattern_slot) recognizer = new geom_feature_pattern_slot(seed_it);
  TColStd_PackedMapOfInteger        traversed;

  //---------------------------------------------------------------------------
  // STAGE 2: attempt to find slots after 1-st AAG decomposition
  //---------------------------------------------------------------------------

  // Iterate over the entire AAG in random manner looking for
  // specific patterns. Those components of AAG which gave no result
  // we collect into a specific collection for "vain clusters"
  NCollection_Sequence<TColStd_PackedMapOfInteger> vainClusters;
  //
  for ( ; seed_it->More(); seed_it->Next() )
  {
    // Recognizer iterates some faces internally. We don't want to
    // use such faces as seeds, so we skip them here
    if ( traversed.Contains( seed_it->GetFaceId() ) )
      continue;

    // Attempt to match the graph components against the slot pattern.
    // If a component gives no result, we mark it as "vain" and defer
    // for future analysis (after second AAG decomposition)
    if ( !recognizer->Recognize(m_result) )
      vainClusters.Append( recognizer->JustTraversed() );

    // Pick up those faces iterated by the recognizer and exclude them
    // for the list to iterate here
    traversed.Unite( recognizer->JustTraversed() );
  }

  //---------------------------------------------------------------------------
  // STAGE 3: perform 2-nd incremental AAG decomposition for vain clusters
  //---------------------------------------------------------------------------

  // Now we have a set of "vain" clusters. These are the graph components
  // which have been iterated but gave no features. Now the idea is to
  // eliminate those faces in the vain clusters which contain internal
  // contours. Such faces are suspected to be the support faces for
  // second-level nested features
  TopTools_IndexedMapOfShape hiddenCandidates;
  for ( int c = 1; c <= vainClusters.Length(); ++ c )
  {
    const TColStd_PackedMapOfInteger& vainCluster = vainClusters(c);
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(vainCluster); cit.More(); cit.Next() )
    {
      const int          face_id = cit.Key();
      const TopoDS_Face& face    = m_aag->GetFace(face_id);

      // Get number of wires
      int nWires = 0;
      for ( TopExp_Explorer exp(face, TopAbs_WIRE); exp.More(); exp.Next() )
        ++nWires;

      if ( nWires > 1 )
      {
        hiddenCandidates.Add(face);

        // If we have found a candidate support face, then the cluster needs
        // to be checked again. This is why we remove the cluster faces from
        // the list of traversed faces, thus allowing to choose seeds from it
        traversed.Subtract(vainCluster);
      }
    }
  }

  // Now try to remove support candidate faces and repeat feature identification.
  // This allows to find second-level nested features

  if ( hiddenCandidates.Extent() ) // The following is done only in presence
                                   // of candidate support faces
  {
    m_aag->Remove(hiddenCandidates);

#if defined GRAPH_DEBUG
    SHOW_GRAPH(m_aag);
#endif

    seed_it->Init(m_aag); // Re-initialize seed iterator

    // Iterate over the entire AAG. Comparing to the iteration done at
    // the 1-st stage, here the list of traversed faces is not empty, so
    // this stage is more fast
    for ( ; seed_it->More(); seed_it->Next() )
    {
      if ( traversed.Contains( seed_it->GetFaceId() ) )
        continue;

      // Recognize
      recognizer->Recognize(m_result);

      // Add the internally traversed faces to the list of excluded seeds
      traversed.Unite( recognizer->JustTraversed() );
    }
  }

  return true; // Success
}
