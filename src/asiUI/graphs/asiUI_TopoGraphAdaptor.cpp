//-----------------------------------------------------------------------------
// Created on: 08 September 2016 (*)
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
#include <asiUI_TopoGraphAdaptor.h>

// asiUI includes
#include <asiUI_PartGraphItem.h>

// asiAlgo includes
#include <asiAlgo_TopoAttrName.h>
#include <asiAlgo_TopoAttrLocation.h>
#include <asiAlgo_TopoAttrOrientation.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkStringArray.h>

//! Converts topology graph to VTK presentable form.
//! \param[in] topograph topology graph to convert.
//! \param[in] naming    optional naming service.
//! \param[in] leafType  type of leafs. This argument allows to stop recursion
//!                      at a certain sub-shape type level.
//! \return VTK graph.
vtkSmartPointer<vtkMutableDirectedGraph>
  asiUI_TopoGraphAdaptor::Convert(const Handle(asiAlgo_TopoGraph)& topograph,
                                  const Handle(asiAlgo_Naming)&    naming,
                                  const TopAbs_ShapeEnum           leafType)
{
  vtkSmartPointer<vtkMutableDirectedGraph>
    result = vtkSmartPointer<vtkMutableDirectedGraph>::New();

  // Get entire shape
  TopoDS_Shape masterShape = topograph->GetShape( topograph->GetRoot() );

  // Prepare maps of sub-shapes
  const TopTools_IndexedMapOfShape& mapOfFaces     = topograph->GetMapOfFaces();
  const TopTools_IndexedMapOfShape& mapOfEdges     = topograph->GetMapOfEdges();
  const TopTools_IndexedMapOfShape& mapOfVertices  = topograph->GetMapOfVertices();
  const TopTools_IndexedMapOfShape& mapOfSubshapes = topograph->GetMapOfSubShapes();

  // Array for groups
  vtkNew<vtkStringArray> groupArr;
  groupArr->SetNumberOfComponents(1);
  groupArr->SetName(ARRNAME_GROUP);

  // Array for vertex labels
  vtkNew<vtkStringArray> labelArr;
  labelArr->SetNumberOfComponents(1);
  labelArr->SetName(ARRNAME_LABELS);

  // Array for orientation attributes associated with arcs
  vtkNew<vtkIntArray> childOriArr;
  childOriArr->SetNumberOfComponents(1);
  childOriArr->SetName(ARRNAME_CHILD_ORIENTATION);

  // Array for location attributes associated with arcs
  vtkNew<vtkStringArray> childLocArr;
  childLocArr->SetNumberOfComponents(1);
  childLocArr->SetName(ARRNAME_CHILD_LOCATION);

  // Array for pedigree indices (sub-shape IDs)
  vtkNew<vtkIntArray> idsArr;
  idsArr->SetNumberOfComponents(1);
  idsArr->SetName(ARRNAME_PIDS);

  NCollection_DataMap<int, vtkIdType> ShapeNodeMap;

  // Add nodes for (sub-)shapes
  const NCollection_IndexedMap<TopoDS_Shape, asiAlgo_TopoGraph::t_partner_hasher>& nodes = topograph->GetNodes();
  //
  for ( int n = 1; n <= nodes.Extent(); ++n )
  {
    // Get shape corresponding to the current node
    const TopoDS_Shape& shape = nodes(n);

    // Do not add nodes representing sub-shapes lower than requested
    if ( shape.ShapeType() > leafType )
      continue;

    // Create VTK data set for graph data
    const vtkIdType root_vid = result->AddVertex();
    ShapeNodeMap.Bind(n, root_vid);

    // Sub-shape ID
    int gid = mapOfSubshapes.FindIndex(shape);
    int pid = 0;
    if ( shape.ShapeType() == TopAbs_FACE )
      pid = mapOfFaces.FindIndex(shape);
    else if ( shape.ShapeType() == TopAbs_EDGE )
      pid = mapOfEdges.FindIndex(shape);
    else if ( shape.ShapeType() == TopAbs_VERTEX )
      pid = mapOfVertices.FindIndex(shape);
    //
    idsArr->InsertNextValue(pid);

    // Prepare label
    std::string label;
    //
    label += "Node ID: ";
    label += asiAlgo_Utils::Str::ToString(n);
    label += "\n";
    //
    label += "Shape: ";
    label += asiAlgo_Utils::ShapeAddrWithPrefix(shape).c_str();
    if ( !shape.Location().IsIdentity() )
      label += "(with location)";
    label += "\n";
    //
    if ( pid )
    {
      label += "Sub-shape pedigree ID: ";
      label += asiAlgo_Utils::Str::ToString(pid);
      label += "\n";
    }
    //
    label += "Sub-shape global ID: ";
    label += asiAlgo_Utils::Str::ToString(gid);
    //
    if ( !naming.IsNull() )
    {
      // If naming is available, the topology graph is enriched with names.
      Handle(asiAlgo_TopoAttr)
        nameAttrBase = topograph->GetNodeAttribute( n, asiAlgo_TopoAttrName::GUID() );
      //
      if ( !nameAttrBase.IsNull() )
      {
        Handle(asiAlgo_TopoAttrName)
          nameAttr = Handle(asiAlgo_TopoAttrName)::DownCast(nameAttrBase);
        //
        label += "\n";
        label += "Topo name: ";
        label += nameAttr->GetName().ToCString();
      }
    }
    //
    labelArr->InsertNextValue(label);

    // Node type
    if ( shape.ShapeType() == TopAbs_COMPOUND )
      groupArr->InsertNextValue(ARRNAME_GROUP_COMPOUND);
    else if ( shape.ShapeType() == TopAbs_COMPSOLID )
      groupArr->InsertNextValue(ARRNAME_GROUP_COMPSOLID);
    else if ( shape.ShapeType() == TopAbs_SOLID )
      groupArr->InsertNextValue(ARRNAME_GROUP_SOLID);
    else if ( shape.ShapeType() == TopAbs_SHELL )
      groupArr->InsertNextValue(ARRNAME_GROUP_SHELL);
    else if ( shape.ShapeType() == TopAbs_FACE )
      groupArr->InsertNextValue(ARRNAME_GROUP_FACE);
    else if ( shape.ShapeType() == TopAbs_WIRE )
      groupArr->InsertNextValue(ARRNAME_GROUP_WIRE);
    else if ( shape.ShapeType() == TopAbs_EDGE )
      groupArr->InsertNextValue(ARRNAME_GROUP_EDGE);
    else if ( shape.ShapeType() == TopAbs_VERTEX )
      groupArr->InsertNextValue(ARRNAME_GROUP_VERTEX);
    else
      groupArr->InsertNextValue(ARRNAME_GROUP_ORDINARY);
  }

  // Add arcs
  for ( asiAlgo_TopoGraph::t_adjacency::Iterator it( topograph->GetArcs() ); it.More(); it.Next() )
  {
    const int                         parentId = it.Key();
    const TColStd_PackedMapOfInteger& children = it.Value();

    // Do not add arcs for nodes representing sub-shapes lower than requested
    if ( topograph->GetShape(parentId).ShapeType() >= leafType )
      continue;

    // Loop over the children
    for ( TColStd_MapIteratorOfPackedMapOfInteger cit(children); cit.More(); cit.Next() )
    {
      const int childId = cit.Key();

      // Get sub-shape
      TopoDS_Shape subShape = topograph->GetShape(childId);
      //
      if ( subShape.ShapeType() > leafType )
        continue;

      // Add arc
      result->AddEdge( ShapeNodeMap(parentId), ShapeNodeMap(childId) );

      // Get orientation attribute available as arc attribute in topology graph
      Handle(asiAlgo_TopoAttr)
        topoAttr = topograph->GetArcAttribute( asiAlgo_TopoGraph::t_arc(parentId, childId),
                                               asiAlgo_TopoAttrOrientation::GUID() );
      //
      if ( !topoAttr.IsNull() )
      {
        TopAbs_Orientation
          ori = Handle(asiAlgo_TopoAttrOrientation)::DownCast(topoAttr)->GetOrientation();

        // Set scalar
        if ( ori == TopAbs_FORWARD )
          childOriArr->InsertNextValue(ARRNAME_CHILD_ORIENTATION_F);
        else if ( ori == TopAbs_REVERSED )
          childOriArr->InsertNextValue(ARRNAME_CHILD_ORIENTATION_R);
        else if ( ori == TopAbs_INTERNAL )
          childOriArr->InsertNextValue(ARRNAME_CHILD_ORIENTATION_I);
        else // EXTERNAL
          childOriArr->InsertNextValue(ARRNAME_CHILD_ORIENTATION_E);
      }

      // Get location attribute available as arc attribute in topology graph
      topoAttr = topograph->GetArcAttribute( asiAlgo_TopoGraph::t_arc(parentId, childId),
                                             asiAlgo_TopoAttrLocation::GUID() );
      //
      if ( !topoAttr.IsNull() )
      {
        const TopLoc_Location&
          loc = Handle(asiAlgo_TopoAttrLocation)::DownCast(topoAttr)->GetLocation();

        // Set string
        childLocArr->InsertNextValue( asiAlgo_Utils::LocationToString(loc).ToCString() );
      }
      else
        childLocArr->InsertNextValue("");
    }
  }

  // Set arrays
  result->GetVertexData() ->AddArray( labelArr.GetPointer() );
  result->GetVertexData() ->AddArray( groupArr.GetPointer() );
  result->GetEdgeData()   ->AddArray( childOriArr.GetPointer() );
  result->GetEdgeData()   ->AddArray( childLocArr.GetPointer() );
  result->GetVertexData() ->AddArray( idsArr.GetPointer() );

  return result;
}
