//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_aag_vtk.h>

// Geometry includes
#include <geom_feature_angle_attr.h>
#include <geom_utils.h>

// Visualization includes
#include <visu_topo_graph_item.h>

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
  geom_aag_vtk::Convert(const Handle(geom_aag)& aag)
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
  const geom_aag::t_adjacency&      Neighborhood = aag->GetNeighborhood();

  // Add vertices for faces
  NCollection_DataMap<int, vtkIdType> FaceVertexMap;
  for ( geom_aag::t_adjacency::Iterator it(Neighborhood); it.More(); it.Next() )
  {
    const int       f_idx     = it.Key();
    const vtkIdType vertex_id = result->AddVertex();
    //
    FaceVertexMap.Bind(f_idx, vertex_id);

    // Fill property arrays
    std::string faceName = geom_utils::FaceGeometryName( TopoDS::Face( Faces(f_idx) ) );
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
  for ( geom_aag::t_adjacency::Iterator it(Neighborhood); it.More(); it.Next() )
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
      geom_aag::t_arc arc(f_idx, neighbor_f_idx);
      Handle(geom_feature_angle_attr)
        attr = Handle(geom_feature_angle_attr)::DownCast( aag->GetAttribute(arc) );
      //
      if ( !attr.IsNull() )
        angleArr->InsertNextValue( attr->GetAngle() );
    }
  }

  return result;
}
