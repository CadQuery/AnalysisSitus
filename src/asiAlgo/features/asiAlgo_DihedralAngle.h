//-----------------------------------------------------------------------------
// Created on: 25 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
