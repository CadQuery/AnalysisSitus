//-----------------------------------------------------------------------------
// Created on: 15 December 2017
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

// Own include
#include <asiAlgo_Euler.h>

// OCCT include
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

//-----------------------------------------------------------------------------

asiAlgo_Euler::asiAlgo_Euler(const TopoDS_Shape&  masterCAD,
                             ActAPI_ProgressEntry progress,
                             ActAPI_PlotterEntry  plotter)
//
: ActAPI_IAlgorithm(progress, plotter), m_master(masterCAD)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_Euler::Perform(const bool doApply)
{
  return this->perform(doApply);
}

//-----------------------------------------------------------------------------

static int subShapesNb(const TopoDS_Shape&    shape,
                       const TopAbs_ShapeEnum type)
{
  TopTools_IndexedMapOfShape aMap;
  TopExp::MapShapes(shape, type, aMap);
  return aMap.Size();
}

//-----------------------------------------------------------------------------

int asiAlgo_Euler::EulerPoincareCharacteristic(const TopoDS_Shape& shape,
                                               const int           genus)
{
  // The formula is:
  // V - E + F - R - 2 * (S - G) = 0
  // Where:
  // V: the number of vertices.
  // E: the number of edges. Degenerated edges and seams should be excluded from consideration.
  // F: the number of faces.
  // G: the number of holes that penetrate the solid, usually referred to as genus in topology.
  // S: the number of shells.
  // R: the number of internal wires.
  // Usually it is hard to compute genus, so it is passed as input parameter.

  const int V = subShapesNb(shape, TopAbs_VERTEX);
  const int F = subShapesNb(shape, TopAbs_FACE);
  const int G = genus;
  const int S = subShapesNb(shape, TopAbs_SHELL);

  // Routine to calculate number of the edges.
  int E = 0;
  TopTools_IndexedMapOfShape mapE; TopExp::MapShapes(shape, TopAbs_EDGE, mapE);
  TopTools_IndexedMapOfShape::Iterator iterMapE(mapE);
  for(; iterMapE.More() ; iterMapE.Next() )
  {
    const TopoDS_Edge& edge = TopoDS::Edge(iterMapE.Value());
    bool isDegen = BRep_Tool::Degenerated(edge);

    if ( isDegen )
      continue;

    E++;
  }

  // Routine to calculate number of the internal wires.
  int R = 0;
  TopExp_Explorer explF(shape, TopAbs_FACE);
  for ( ; explF.More(); explF.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face(explF.Current());
    TopoDS_Wire faceOuterWire = ShapeAnalysis::OuterWire(face);

    TopExp_Explorer explW(face, TopAbs_WIRE);
    for ( ; explW.More() ; explW.Next() )
    {
      const TopoDS_Wire& wire = TopoDS::Wire(explW.Current());

      if  (wire.IsSame(faceOuterWire) )
        continue;

      R++;
    }
  }

  const int res = V - E + F - R - 2 * (S - G);
  return res;
}
