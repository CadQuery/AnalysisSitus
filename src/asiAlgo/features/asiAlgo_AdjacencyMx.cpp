//-----------------------------------------------------------------------------
// Created on: 02 May 2020
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
#include <asiAlgo_AdjacencyMx.h>

// OpenCascade includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

Eigen::MatrixXd
  asiAlgo_AdjacencyMx::AsEigenMx(t_indexMap& idxMap) const
{
  // Get dimensions.
  const int nbRows = this->mx.Extent();
  int       nbCols = nbRows;

  // Allocate Eigen matrix and initialize it with zeroes as no
  // initialization of elements is done by default.
  Eigen::MatrixXd emx(nbRows, nbCols);
  //
  for ( int r = 0; r < nbRows; ++r )
    for ( int c = 0; c < nbCols; ++c )
      emx(r, c) = 0.;

  // Fill mapping.
  auto eigenRowIdx = 0;
  for ( t_mx::Iterator rowIt(this->mx); rowIt.More(); rowIt.Next(), ++eigenRowIdx )
  {
    const t_topoId rowFaceId = rowIt.Key();

    // Set mapping.
    idxMap.Bind(eigenRowIdx, rowFaceId);
  }

  // Populate the matrix.
  eigenRowIdx = 0;
  for ( t_mx::Iterator rowIt(this->mx); rowIt.More(); rowIt.Next(), ++eigenRowIdx )
  {
    const TColStd_PackedMapOfInteger& row = rowIt.Value();

    // Populate the adjacency matrix.
    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(row); cit.More(); cit.Next() )
    {
      const t_topoId nid = cit.Key();

      emx( eigenRowIdx, idxMap.Find2(nid) ) = 1;
    }
  }

  return emx;
}

//-----------------------------------------------------------------------------

asiAlgo_AdjacencyMx::t_std_mx
  asiAlgo_AdjacencyMx::AsStandard(t_indexMap& idxMap) const
{
  t_std_mx smx;

  // Fill mapping.
  auto stdRowIdx = 0;
  for ( t_mx::Iterator rowIt(this->mx); rowIt.More(); rowIt.Next(), ++stdRowIdx )
  {
    const t_topoId rowFaceId = rowIt.Key();

    // Set mapping.
    idxMap.Bind(stdRowIdx, rowFaceId);
  }

  // Populate the matrix.
  stdRowIdx = 0;
  for ( t_mx::Iterator rowIt(this->mx); rowIt.More(); rowIt.Next(), ++stdRowIdx )
  {
    const TColStd_PackedMapOfInteger& row = rowIt.Value();

    smx.push_back( std::vector<int>() );
    //
    std::vector<int>& stdRow = smx[stdRowIdx];

    // Populate the adjacency matrix.
    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(row); cit.More(); cit.Next() )
    {
      const t_topoId nid = cit.Key();

      stdRow.push_back( idxMap.Find2(nid) );
    }
  }

  return smx;
}
