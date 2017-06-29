//-----------------------------------------------------------------------------
// Created on: 21 March 2016
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

// Own include
#include <asiUI_AAGAdaptor.h>

// asiUI includes
#include <asiUI_TopoGraphItem.h>

// asiAlgo includes
#include <asiAlgo_FeatureAttrAngle.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopoDS.hxx>

// Qr includes
#include <QrCore.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkStringArray.h>

//! Converts AAG to VTK presentable form.
//! \param aag [in] AAG to convert.
//! \return VTK graph.
vtkSmartPointer<vtkMutableUndirectedGraph>
  asiUI_AAGAdaptor::Convert(const Handle(asiAlgo_AAG)& aag)
{
  vtkSmartPointer<vtkMutableUndirectedGraph>
    result = vtkSmartPointer<vtkMutableUndirectedGraph>::New();

  // Array for vertex groups
  vtkNew<vtkStringArray> groupArr;
  groupArr->SetNumberOfComponents(1);
  groupArr->SetName(ARRNAME_GROUP);

  // Array for vertex labels
  vtkNew<vtkStringArray> labelArr;
  labelArr->SetNumberOfComponents(1);
  labelArr->SetName(ARRNAME_LABELS);

  // Array for angle attributes associated with arcs
  vtkNew<vtkIntArray> angleArr;
  angleArr->SetNumberOfComponents(1);
  angleArr->SetName(ARRNAME_ANGLES);

  // Array for pedigree indices (sub-shape IDs)
  vtkNew<vtkIntArray> idsArr;
  idsArr->SetNumberOfComponents(1);
  idsArr->SetName(ARRNAME_PIDS);

  // Get faces from AAG.
  const TopTools_IndexedMapOfShape& Faces        = aag->GetMapOfFaces();
  const TColStd_PackedMapOfInteger& SelFaces     = aag->GetSelectedFaces();
  const asiAlgo_AAG::t_adjacency&   Neighborhood = aag->GetNeighborhood();

  // Add vertices for faces
  NCollection_DataMap<int, vtkIdType> FaceVertexMap;
  for ( asiAlgo_AAG::t_adjacency::Iterator it(Neighborhood); it.More(); it.Next() )
  {
    const int       f_idx     = it.Key();
    const vtkIdType vertex_id = result->AddVertex();
    //
    FaceVertexMap.Bind(f_idx, vertex_id);

    // Fill property arrays
    std::string faceName = asiAlgo_Utils::FaceGeometryName( TopoDS::Face( Faces(f_idx) ) );
    faceName += ":";
    faceName += QrCore::to_string<int>(f_idx).c_str();
    labelArr->InsertNextValue(faceName);
    //
    if ( SelFaces.Contains(f_idx) )
      groupArr->InsertNextValue(ARRNAME_GROUP_HIGHLIGHTED);
    else
      groupArr->InsertNextValue(ARRNAME_GROUP_ORDINARY);
    //
    idsArr->InsertNextValue( aag->GetMapOfSubShapes().FindIndex( aag->GetFace(f_idx) ) );
  }

  // Set property arrays
  result->GetVertexData()->AddArray( labelArr.GetPointer() );
  result->GetVertexData()->AddArray( groupArr.GetPointer() );
  result->GetVertexData()->AddArray( idsArr  .GetPointer() );
  result->GetEdgeData()  ->AddArray( angleArr.GetPointer() );

  // Add links for adjacency relations
  for ( asiAlgo_AAG::t_adjacency::Iterator it(Neighborhood); it.More(); it.Next() )
  {
    const int       f_idx = it.Key();
    const vtkIdType v_idx = FaceVertexMap(f_idx);

    // Get face neighbors
    const TColStd_PackedMapOfInteger& neighbors = it.Value();

    // Add links for each neighbor
    for ( TColStd_MapIteratorOfPackedMapOfInteger mit(neighbors); mit.More(); mit.Next() )
    {
      const int       neighbor_f_idx = mit.Key();
      const vtkIdType neighbor_v_idx = FaceVertexMap(neighbor_f_idx);

      // Add link
      result->AddEdge(v_idx, neighbor_v_idx);

      // Check angle
      asiAlgo_AAG::t_arc arc(f_idx, neighbor_f_idx);
      Handle(asiAlgo_FeatureAttrAngle)
        attr = Handle(asiAlgo_FeatureAttrAngle)::DownCast( aag->GetArcAttribute(arc) );
      //
      if ( !attr.IsNull() )
        angleArr->InsertNextValue( attr->GetAngle() );
    }
  }

  return result;
}
