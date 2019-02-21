//-----------------------------------------------------------------------------
// Created on: 27 November 2017
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
#include <asiAlgo_TopoAttrName.h>

//-----------------------------------------------------------------------------

asiAlgo_Naming::asiAlgo_Naming(const Handle(asiAlgo_TopoGraph)& topograph,
                               ActAPI_ProgressEntry             progress)
: Standard_Transient()
{
  m_topograph = topograph;
  m_history   = new asiAlgo_History;
  m_progress  = progress;
}

//-----------------------------------------------------------------------------

asiAlgo_Naming::asiAlgo_Naming(const TopoDS_Shape&  shape,
                               ActAPI_ProgressEntry progress)
: Standard_Transient()
{
  m_topograph = new asiAlgo_TopoGraph(shape);
  m_history   = new asiAlgo_History;
  m_progress  = progress;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Naming::InitNames()
{
  if ( m_topograph.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Topology graph is not initialized.");
    return false;
  }

  // Loop over the topology graph and generate names for its items.
  for ( asiAlgo_TopoGraph::Iterator git(m_topograph); git.More(); git.Next() )
  {
    // Get current topological item.
    const int           itemIndex = git.GetCurrentIndex();
    const TopoDS_Shape& itemShape = git.GetCurrentNode();

    // Generate unique name.
    TCollection_AsciiString itemName = this->GenerateName(itemShape);

    // Bind to map for fast access.
    if ( !this->registerNamedShape(itemName, itemShape) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Attempt to register shape under non-unique name '%1'."
                                               << itemName);
      return false;
    }

    // Prepare nodal attribute to store the name in the topology graph.
    Handle(asiAlgo_TopoAttrName) itemAttr = new asiAlgo_TopoAttrName(itemName);
    //
    m_topograph->AddNodeAttribute(itemIndex, itemAttr);
  }

  return true;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Naming::GenerateName(const TopoDS_Shape& shape)
{
  const TopAbs_ShapeEnum shapeType = shape.ShapeType();

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

  // Add 1-based index as a suffix to make the name unique.
  if ( !m_nameIds.IsBound(shapeType) )
    m_nameIds.Bind(shapeType, 1);
  else
    m_nameIds(shapeType) += 1;
  //
  baseName += "_";
  baseName += m_nameIds(shapeType);

  return baseName;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Naming::Actualize(const TopoDS_Shape& newShape)
{
  if ( m_history.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "No history to actualize naming.");
    return false;
  }

  // Clean up the map of alive sub-shapes.
  m_namedAliveShapes.Clear();

  // Construct new topology graph.
  Handle(asiAlgo_TopoGraph) newTopograph = new asiAlgo_TopoGraph(newShape);

  // Loop over the previous topology graph and try finding images for
  // its topological elements using the history.
  for ( asiAlgo_TopoGraph::Iterator oldIt(m_topograph); oldIt.More(); oldIt.Next() )
  {
    const int           oldIndex = oldIt.GetCurrentIndex();
    const TopoDS_Shape& oldShape = oldIt.GetCurrentNode();

    // Get name of the old shape.
    Handle(asiAlgo_TopoAttr)
      nameAttrBase = m_topograph->GetNodeAttribute( oldIndex, asiAlgo_TopoAttrName::GUID() );
    //
    TCollection_AsciiString name;
    //
    if ( !nameAttrBase.IsNull() )
    {
      Handle(asiAlgo_TopoAttrName)
        nameAttr = Handle(asiAlgo_TopoAttrName)::DownCast(nameAttrBase);
      //
      name = nameAttr->GetName();
    }

    // Adjust mapping between names and alive sub-shapes.
    std::vector<TopoDS_Shape> modified;
    //
    if ( m_history->GetModified(oldShape, modified) )
    {
      // Get all topological elements which substitute the old shape.
      this->actualizeImages(modified, newTopograph, nameAttrBase, false);
    }
    else if ( !m_history->IsDeleted(oldShape) )
    {
      this->passIntact(oldShape, newTopograph, nameAttrBase);
    }

    // Get all topological elements which were generated from the old shape.
    std::vector<TopoDS_Shape> generated;
    //
    if ( m_history->GetGenerated(oldShape, generated) )
      this->actualizeImages(modified, newTopograph, nameAttrBase, true);
  }

  // Replace topology graph.
  m_topograph = newTopograph;
  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_Naming::actualizeImages(const std::vector<TopoDS_Shape>& images,
                                     const Handle(asiAlgo_TopoGraph)& newTopograph,
                                     const Handle(asiAlgo_TopoAttr)&  attr2Pass,
                                     const bool                       isGenerated)
{
  if ( attr2Pass.IsNull() )
    return;

  // Make a copy of this attribute.
  Handle(asiAlgo_TopoAttrName)
    namingAttr = Handle(asiAlgo_TopoAttrName)::DownCast( attr2Pass->Copy() );

  // Prepare name.
  const TCollection_AsciiString& name = namingAttr->GetName();
  TCollection_AsciiString imageName;

  // Add prefix for generated items.
  if ( isGenerated )
  {
    imageName = "Generated from ";
    imageName += name;

    namingAttr->SetName(name);
  }
  else
    imageName = name;

  // Loop over the images to populate them with the attribute.
  for ( size_t k = 0; k < images.size(); ++k )
  {
    const TopoDS_Shape& imageShape = images[k];

    // Add attribute to the topology graph.
    const int imageNodeIdx = newTopograph->GetNodeIndex(imageShape);
    //
    if ( imageNodeIdx )
    {
      // Add name attribute. Notice that the map of labels will be updated
      // only in case if the topology graph accepts the passed name
      // attribute. This is done for better data consistency.
      if ( newTopograph->AddNodeAttribute(imageNodeIdx, namingAttr) )
      {
        // Bind to map for fast access (rewrite on).
        this->registerNamedShape(imageName, imageShape);
      }
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_Naming::passIntact(const TopoDS_Shape&              shape,
                                const Handle(asiAlgo_TopoGraph)& newTopograph,
                                const Handle(asiAlgo_TopoAttr)&  attr2Pass)
{
  if ( attr2Pass.IsNull() )
    return;

  // Make a copy of this attribute.
  Handle(asiAlgo_TopoAttrName)
    namingAttr = Handle(asiAlgo_TopoAttrName)::DownCast( attr2Pass->Copy() );

  // Bind to map for fast access (rewrite on).
  this->registerNamedShape(namingAttr->GetName(), shape);

  // Add attribute to the topology graph.
  const int imageNodeIdx = newTopograph->GetNodeIndex(shape);
  //
  if ( imageNodeIdx )
    newTopograph->AddNodeAttribute(imageNodeIdx, namingAttr);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Naming::registerNamedShape(const TCollection_AsciiString& name,
                                        const TopoDS_Shape&            shape)
{
  if ( m_namedAliveShapes.IsBound(name) )
    return false; // Not unique name passed.

  // We bound the named shape with EXTERNAL orientation as actually orientation
  // has no sense for a boundary element alone. Imagine a vertex which is shared
  // by two edges. Imagine also that we store oriented entities here. Then,
  // once the topological naming service is initialized, all boundary elements
  // are used to populate the map of names. To populate that map, we iterate
  // the topograph which distinguishes only partner elements because orientations
  // are stored as arc attributes. Imagine that you first iterate a Co-Vertex
  // from the first edge and the topograph returns REVERSED entity. In such case,
  // the topological naming map will always store the vertex boundary element
  // in a reversed state even though the second edge may forward-include it.
  m_namedAliveShapes.Bind( name, shape.Oriented(TopAbs_EXTERNAL) );
  return true;
}
