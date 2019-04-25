//-----------------------------------------------------------------------------
// Created on: 27 November 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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
#include <asiAlgo_Naming.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_TopoAttrName.h>
#include <asiAlgo_Utils.h>

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Naming::PrepareName(const TopAbs_ShapeEnum shapeType,
                              const int              shapeIndex)
{
  // Generate base name which corresponds to the topological type.
  TCollection_AsciiString baseName;
  //
  switch ( shapeType )
  {
    case TopAbs_COMPOUND:
      baseName = "compound"; break;
    case TopAbs_COMPSOLID:
      baseName = "compsolid"; break;
    case TopAbs_SOLID:
      baseName = "solid"; break;
    case TopAbs_SHELL:
      baseName = "shell"; break;
    case TopAbs_FACE:
      baseName = "face"; break;
    case TopAbs_WIRE:
      baseName = "wire"; break;
    case TopAbs_EDGE:
      baseName = "edge"; break;
    case TopAbs_VERTEX:
      baseName = "vertex"; break;
    case TopAbs_SHAPE:
      baseName = "shape"; break;
  }

  // Add suffix.
  baseName += "_";
  baseName += shapeIndex;

  return baseName;
}

//-----------------------------------------------------------------------------

TopAbs_ShapeEnum
  asiAlgo_Naming::ShapeTypeByName(const TCollection_AsciiString& name)
{
  std::vector<TCollection_AsciiString> chunks;
  asiAlgo_Utils::Str::Split(name, "_", chunks);

  if ( chunks.size() != 2 )
    return TopAbs_SHAPE;

  const TCollection_AsciiString& baseName = chunks[0];

  if ( baseName == "compound" )
    return TopAbs_COMPOUND;

  if ( baseName == "compsolid" )
    return TopAbs_COMPSOLID;

  if ( baseName == "solid" )
    return TopAbs_SOLID;

  if ( baseName == "shell" )
    return TopAbs_SHELL;

  if ( baseName == "face" )
    return TopAbs_FACE;

  if ( baseName == "wire" )
    return TopAbs_WIRE;

  if ( baseName == "edge" )
    return TopAbs_EDGE;

  if ( baseName == "vertex" )
    return TopAbs_VERTEX;

  return TopAbs_SHAPE;
}

//-----------------------------------------------------------------------------

asiAlgo_Naming::asiAlgo_Naming(ActAPI_ProgressEntry progress,
                               ActAPI_PlotterEntry  plotter)
: asiAlgo_History(progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_Naming::InitNames(const Handle(asiAlgo_AAG)& aag)
{
  TopTools_IndexedMapOfShape allFaces, allEdges, allVertices;

  aag->RequestMapOf(TopAbs_FACE,   allFaces);
  aag->RequestMapOf(TopAbs_EDGE,   allEdges);
  aag->RequestMapOf(TopAbs_VERTEX, allVertices);

  // Add roots to the history for faces.
  for ( int f = 1; f <= allFaces.Extent(); ++f )
    this->AddRoot( allFaces(f), f );

  // Add roots to the history for edges.
  for ( int e = 1; e <= allEdges.Extent(); ++e )
    this->AddRoot( allEdges(e), e );

  // Add roots to the history for vertices.
  for ( int v = 1; v <= allVertices.Extent(); ++v )
    this->AddRoot( allVertices(v), v );

  return true;
}

//-----------------------------------------------------------------------------

int asiAlgo_Naming::FindRigidId(const TopoDS_Shape& shape) const
{
  if ( !m_items.Contains(shape) )
    return 0;

  t_item* pItem = m_items.FindFromKey(shape);
  return pItem->RigidId;
}

//-----------------------------------------------------------------------------

TopoDS_Face asiAlgo_Naming::FindAliveFace(const int rigidId) const
{
  // Get all roots of a certain type.
  std::vector<t_item*> roots;
  this->GetRootsOfType(TopAbs_FACE, roots);

  // Iterate over the roots finding the origin with the given rigid ID.
  t_item* pFoundOrigin = NULL;
  for ( size_t k = 0; k < roots.size(); ++k )
    if ( roots[k]->RigidId == rigidId )
    {
      pFoundOrigin = roots[k];
      break;
    }

  if ( !pFoundOrigin )
    return TopoDS_Face();

  // Find the ultimate leaf.
  TopoDS_Shape image = this->GetLastImageOrArg(pFoundOrigin->TransientPtr);
  //
  if ( image.IsNull() || image.ShapeType() != TopAbs_FACE )
    return TopoDS_Face();

  return TopoDS::Face(image);
}

//-----------------------------------------------------------------------------

TopoDS_Edge asiAlgo_Naming::FindAliveEdge(const int rigidId) const
{
  // Get all roots of a certain type.
  std::vector<t_item*> roots;
  this->GetRootsOfType(TopAbs_EDGE, roots);

  // Iterate over the roots finding the origin with the given rigid ID.
  t_item* pFoundOrigin = NULL;
  for ( size_t k = 0; k < roots.size(); ++k )
    if ( roots[k]->RigidId == rigidId )
    {
      pFoundOrigin = roots[k];
      break;
    }

  if ( !pFoundOrigin )
    return TopoDS_Edge();

  // Find the ultimate leaf.
  TopoDS_Shape image = this->GetLastImageOrArg(pFoundOrigin->TransientPtr);
  //
  if ( image.IsNull() || image.ShapeType() != TopAbs_EDGE )
    return TopoDS_Edge();

  return TopoDS::Edge(image);
}

//-----------------------------------------------------------------------------

TopoDS_Vertex asiAlgo_Naming::FindAliveVertex(const int rigidId) const
{
  // Get all roots of a certain type.
  std::vector<t_item*> roots;
  this->GetRootsOfType(TopAbs_VERTEX, roots);

  // Iterate over the roots finding the origin with the given rigid ID.
  t_item* pFoundOrigin = NULL;
  for ( size_t k = 0; k < roots.size(); ++k )
    if ( roots[k]->RigidId == rigidId )
    {
      pFoundOrigin = roots[k];
      break;
    }

  if ( !pFoundOrigin )
    return TopoDS_Vertex();

  // Find the ultimate leaf.
  TopoDS_Shape image = this->GetLastImageOrArg(pFoundOrigin->TransientPtr);
  //
  if ( image.IsNull() || image.ShapeType() != TopAbs_VERTEX )
    return TopoDS_Vertex();

  return TopoDS::Vertex(image);
}

//-----------------------------------------------------------------------------

TopoDS_Shape
  asiAlgo_Naming::FindAliveShape(const TCollection_AsciiString& name) const
{
  // Get rigid index.
  std::vector<TCollection_AsciiString> chunks;
  asiAlgo_Utils::Str::Split(name, "_", chunks);
  //
  if ( chunks.size() != 2 || !chunks[1].IsIntegerValue() )
    return TopoDS_Shape();

  const int rigidId = chunks[1].IntegerValue();

  // Get shape type.
  const TopAbs_ShapeEnum shapeType = ShapeTypeByName(name);

  if ( shapeType == TopAbs_FACE )
    return this->FindAliveFace(rigidId);

  if ( shapeType == TopAbs_EDGE )
    return this->FindAliveEdge(rigidId);

  if ( shapeType == TopAbs_VERTEX )
    return this->FindAliveVertex(rigidId);

  return TopoDS_Shape();
}
