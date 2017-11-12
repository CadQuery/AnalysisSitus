//-----------------------------------------------------------------------------
// Created on: 25 April 2016
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

#ifndef asiAlgo_DihedralAngle_h
#define asiAlgo_DihedralAngle_h

// asiAlgo includes
#include <asiAlgo_FeatureAngle.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_DihedralAngle, ActAPI_IAlgorithm)

//! Utility to analyze dihedral angles.
class asiAlgo_DihedralAngle : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DihedralAngle, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_DihedralAngle(ActAPI_ProgressEntry progress,
                          ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT asiAlgo_FeatureAngle
    AngleBetweenFaces(const TopoDS_Face&          F,
                      const TopoDS_Face&          G,
                      const bool                  allowSmooth,
                      const double                smoothAngularTol,
                      TopTools_IndexedMapOfShape& commonEdges) const;

public:

  //! Sets a priori known common edge.
  //! \param[in] commonEdge common edge.
  void SetCommonEdge(const TopoDS_Edge& commonEdge)
  {
    m_commonEdge = commonEdge;
  }

protected:

  TopoDS_Edge m_commonEdge; //!< Optional a priori known common edge.

};

#endif
