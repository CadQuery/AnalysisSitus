//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeFeatures_AAGAdaptor.h>

// exeFeatures includes
#include <exeFeatures_TopoGraphItem.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Feature includes
#include <feature_attr_angle.h>

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
  exeFeatures_AAGAdaptor::Convert(const Handle(feature_aag)& aag)
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

  // Get faces from AAG.
  const TopTools_IndexedMapOfShape& Faces        = aag->GetFaces();
  const TColStd_PackedMapOfInteger& SelFaces     = aag->GetSelectedFaces();
  const feature_aag::t_adjacency&   Neighborhood = aag->GetNeighborhood();

  // Add vertices for faces
  NCollection_DataMap<int, vtkIdType> FaceVertexMap;
  for ( feature_aag::t_adjacency::Iterator it(Neighborhood); it.More(); it.Next() )
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
      groupArr->InsertNextValue(ARRNAME_GROUP_ADJACENT);
    else
      groupArr->InsertNextValue(ARRNAME_GROUP_ORDINARY);
  }

  // Set property arrays
  result->GetVertexData()->AddArray( labelArr.GetPointer() );
  result->GetVertexData()->AddArray( groupArr.GetPointer() );
  result->GetEdgeData()  ->AddArray( angleArr.GetPointer() );

  // Add links for adjacency relations
  for ( feature_aag::t_adjacency::Iterator it(Neighborhood); it.More(); it.Next() )
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
      feature_aag::t_arc arc(f_idx, neighbor_f_idx);
      Handle(feature_attr_angle)
        attr = Handle(feature_attr_angle)::DownCast( aag->GetArcAttribute(arc) );
      //
      if ( !attr.IsNull() )
        angleArr->InsertNextValue( attr->GetAngle() );
    }
  }

  return result;
}
