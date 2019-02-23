//-----------------------------------------------------------------------------
// Created on: 20 April 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiAlgo_CheckToler.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

asiAlgo_CheckToler::asiAlgo_CheckToler(const TopoDS_Shape&  shape,
                                       ActAPI_ProgressEntry progress,
                                       ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_shape = shape;
}

//-----------------------------------------------------------------------------

bool asiAlgo_CheckToler::PerformRanges(const std::vector<double>& tolerRanges,
                                       std::vector<TopoDS_Shape>& tolerShapes,
                                       TopoDS_Shape&              outOfRangeMin,
                                       TopoDS_Shape&              outOfRangeMax) const
{
  TIMER_NEW
  TIMER_GO

  // Populate vector with shapes.
  for ( size_t k = 0; k < tolerRanges.size() - 1; ++k )
    tolerShapes.push_back( TopoDS_Shape() );

  TopTools_IndexedMapOfShape allVertices, allEdges, allFaces;
  TopExp::MapShapes(m_shape, TopAbs_VERTEX, allVertices);
  TopExp::MapShapes(m_shape, TopAbs_EDGE,   allEdges);
  TopExp::MapShapes(m_shape, TopAbs_FACE,   allFaces);

  // Analyze vertices.
  for ( int k = 1; k <= allVertices.Extent(); ++k )
    this->analyzeSubShape(allVertices(k), tolerRanges, tolerShapes, outOfRangeMin, outOfRangeMax);

  // Analyze edges.
  for ( int k = 1; k <= allEdges.Extent(); ++k )
    this->analyzeSubShape(allEdges(k), tolerRanges, tolerShapes, outOfRangeMin, outOfRangeMax);

  // Analyze faces.
  for ( int k = 1; k <= allFaces.Extent(); ++k )
    this->analyzeSubShape(allFaces(k), tolerRanges, tolerShapes, outOfRangeMin, outOfRangeMax);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Check tolerances")

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_CheckToler::analyzeSubShape(const TopoDS_Shape&        subshape,
                                         const std::vector<double>& tolerRanges,
                                         std::vector<TopoDS_Shape>& tolerShapes,
                                         TopoDS_Shape&              outOfRangeMin,
                                         TopoDS_Shape&              outOfRangeMax) const
{
  double tolerance = -1;
  //
  if ( subshape.ShapeType() == TopAbs_FACE )
    tolerance = BRep_Tool::Tolerance( TopoDS::Face(subshape) );
  //
  else if ( subshape.ShapeType() == TopAbs_EDGE )
    tolerance = BRep_Tool::Tolerance( TopoDS::Edge(subshape) );
  //
  else
    tolerance = BRep_Tool::Tolerance( TopoDS::Vertex(subshape) );

  const double veryMinTol = tolerRanges[0];
  const double veryMaxTol = tolerRanges[tolerRanges.size() - 1];

  // Find the corresponding range.
  if ( tolerance < veryMinTol )
  {
    if ( outOfRangeMin.IsNull() )
    {
      TopoDS_Compound comp;
      BRep_Builder().MakeCompound(comp);
      outOfRangeMin = comp;
    }

    BRep_Builder().Add(TopoDS::Compound(outOfRangeMin), subshape);
  }
  else if ( tolerance > veryMaxTol )
  {
    if ( outOfRangeMax.IsNull() )
    {
      TopoDS_Compound comp;
      BRep_Builder().MakeCompound(comp);
      outOfRangeMax = comp;
    }

    BRep_Builder().Add(TopoDS::Compound(outOfRangeMax), subshape);
  }
  else
  {
    for ( size_t k = 0; k < tolerRanges.size() - 1; ++k )
    {
      const double tolMin = tolerRanges[k];
      const double tolMax = tolerRanges[k + 1];

      if ( tolerance >= tolMin && tolerance <= tolMax )
      {
        if ( tolerShapes[k].IsNull() )
        {
          TopoDS_Compound comp;
          BRep_Builder().MakeCompound(comp);
          tolerShapes[k] = comp;
        }

        BRep_Builder().Add(TopoDS::Compound(tolerShapes[k]), subshape);
        break;
      }
    }
  }
}
