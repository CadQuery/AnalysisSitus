//-----------------------------------------------------------------------------
// Created on: 27 April 2020
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

#ifndef asiAlgo_AdjacencyMx_h
#define asiAlgo_AdjacencyMx_h

// asiAlgo includes
#include <asiAlgo.h>

// OpenCascade includes
#include <NCollection_DataMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

// Standard includes
#include <vector>

// Eigen includes
#ifdef _WIN32
#pragma warning(disable : 4701 4702)
#endif
#include <Eigen/Dense>
#ifdef _WIN32
#pragma warning(default : 4701 4702)
#endif

//-----------------------------------------------------------------------------

//! Adjacency matrix.
class asiAlgo_AdjacencyMx
{
public:

  //! Type definition for the internal data structure.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_mx;

  //typedef NCollection_DataMap< int, std::vector<int> >

public:

  t_mx mx; //!< Adjacency rows.

public:

  //! \return equivalent Eigen maxtrix.
  asiAlgo_EXPORT Eigen::MatrixXd
    AsEigenMx() const;
};

#endif
