//-----------------------------------------------------------------------------
// Created on: 05 October 2018
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

#ifndef asiAlgo_MeshComputeNorms_HeaderFile
#define asiAlgo_MeshComputeNorms_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data (API) includes
#include <ActAPI_IAlgorithm.h>

// Active Data includes
#include <ActData_Mesh.h>

//! Utility to compute normal field on mesh.
class asiAlgo_MeshComputeNorms : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshComputeNorms, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] mesh     mesh in question.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshComputeNorms(const Handle(ActData_Mesh)& mesh,
                             ActAPI_ProgressEntry        progress,
                             ActAPI_PlotterEntry         plotter);

public:

  //! Performs computation of normal field in nodes.
  asiAlgo_EXPORT void
    Perform();

public:

  //! \return computed normal field.
  const NCollection_DataMap<int, gp_Vec>& GetResult() const
  {
    return m_norms;
  }

  //! Repacks the resulting data map in form of persistent arrays which can
  //! be stored in OCAF Nodes.
  //! \param[out] ids     node IDs having the vectors associated.
  //! \param[out] vectors coordinates of the corresponding vectors.
  void GetResultArrays(Handle(HIntArray)&  ids,
                       Handle(HRealArray)& vectors) const
  {
    const int numNodes = m_norms.Extent();

    // Allocate arrays.
    ids     = new HIntArray  (0, numNodes - 1);
    vectors = new HRealArray (0, numNodes*3 - 1);

    // Populate arrays.
    int arrIdx = 0;
    for ( NCollection_DataMap<int, gp_Vec>::Iterator it(m_norms); it.More(); it.Next(), ++arrIdx )
    {
      const int     nodeId = it.Key();
      const gp_Vec& vec    = it.Value();
      //
      ids->SetValue     ( arrIdx, nodeId );
      vectors->SetValue ( arrIdx*3 + 0, vec.X() );
      vectors->SetValue ( arrIdx*3 + 1, vec.Y() );
      vectors->SetValue ( arrIdx*3 + 2, vec.Z() );
    }
  }

protected:

  //! Computes normal vector for the given mesh element.
  //! \param[in] elem mesh element in question.
  //! \return normal vector.
  asiAlgo_EXPORT gp_Vec
    computeElemNorm(const Handle(ActData_Mesh_Element)& elem) const;


  //! Returns collection of nodes for the given mesh panel.
  //! \param[in]  elem  element to access the nodes for.
  //! \param[out] nodes collection of element's nodes.
  asiAlgo_EXPORT void
    elementNodes(const Handle(ActData_Mesh_Element)&     elem,
                 std::vector<Handle(ActData_Mesh_Node)>& nodes) const;

protected:

  //!< Mesh in question.
  Handle(ActData_Mesh) m_mesh;

  //!< Normal field associated with node IDs.
  NCollection_DataMap<int, gp_Vec> m_norms;

};

#endif
