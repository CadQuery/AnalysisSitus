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

Eigen::MatrixXd asiAlgo_AdjacencyMx::AsEigenMx() const
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

  // Populate the matrix.
  for ( int r = 1; r <= nbRows; ++r )
  {
    const TColStd_PackedMapOfInteger& row = this->mx(r);

    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(row); cit.More(); cit.Next() )
    {
      const int c = cit.Key();

      emx(r - 1, c - 1) = 1;
    }
  }

  return emx;
}
