//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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

// Own include
#include <asiAlgo_AttrBlendCandidate.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

void asiAlgo_AttrBlendCandidate::Dump(Standard_OStream& out) const
{
  out << "\n\t" << this->DynamicType()->Name();
  out << "\n\tFeature ID: " << this->GetFeatureId();
  out << "\n\tType: ";
  //
  if ( Kind == BlendType_Uncertain )
    out << "uncertain";
  else if ( Kind == BlendType_Ordinary )
    out << "ordinary";
  else if ( Kind == BlendType_Vertex )
    out << "vertex";
  else if ( Kind == BlendType_Cliff )
    out << "cliff";

  out << "\n\tConfirmed: "              << (Confirmed ? "true" : "false");
  out << "\n\tNum. smooth edges: "      << SmoothEdgeIndices.Extent();
  out << "\n\tNum. spring edges: "      << SpringEdgeIndices.Extent();
  out << "\n\tNum. cross edges: "       << CrossEdgeIndices.Extent();
  out << "\n\tNum. terminating edges: " << TerminatingEdgeIndices.Extent();
}

//-----------------------------------------------------------------------------

void asiAlgo_AttrBlendCandidate::Dump(ActAPI_PlotterEntry plotter) const
{
  asiAlgo_AAG* pAAG = this->getAAG();
  const int    fid  = this->getFaceId();
  //
  if ( !pAAG || !pAAG->HasFace(fid) )
    return;

  // Get face
  const TopoDS_Face& face = pAAG->GetFace(fid);

  // Get map of edges
  const TopTools_IndexedMapOfShape& allEdges = pAAG->GetMapOfEdges();

  // Draw spring edges
  for ( TColStd_MapIteratorOfPackedMapOfInteger mit(this->SpringEdgeIndices);
        mit.More(); mit.Next() )
  {
    plotter.DRAW_SHAPE( allEdges.FindKey( mit.Key() ), Color_Green, 1.0, true, "DUMP spring edge" );
  }

  // Draw cross edges
  for ( TColStd_MapIteratorOfPackedMapOfInteger mit(this->CrossEdgeIndices);
        mit.More(); mit.Next() )
  {
    plotter.DRAW_SHAPE( allEdges.FindKey( mit.Key() ), Color_Red, 1.0, true, "DUMP cross edge" );
  }

  // Draw terminating edges
  for ( TColStd_MapIteratorOfPackedMapOfInteger mit(this->TerminatingEdgeIndices);
        mit.More(); mit.Next() )
  {
    plotter.DRAW_SHAPE( allEdges.FindKey( mit.Key() ), Color_Magenta, 1.0, true, "DUMP terminating edge" );
  }
}
