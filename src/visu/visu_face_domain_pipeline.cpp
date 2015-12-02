//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_face_domain_pipeline.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_pcurve_source.h>
#include <visu_face_data_provider.h>
#include <visu_utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Creates new Face Domain Pipeline initialized by default VTK mapper and actor.
visu_face_domain_pipeline::visu_face_domain_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet(false)
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_face_domain_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_face_data_provider)
    faceProvider = Handle(visu_face_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  const int face_idx = faceProvider->GetFaceIndex();
  if ( !face_idx )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(aDummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( faceProvider->MustExecute( this->GetMTime() ) )
  {
    // Access owning geometry
    ActAPI_DataObjectId face_node_id = faceProvider->GetNodeID();
    Handle(geom_node)
      geom_n = Handle(geom_node)::DownCast( common_facilities::Instance()->Model->FindNode(face_node_id)->GetParentNode() );

    // Prepare traversal
    TopTools_IndexedMapOfShape M;
    TopExp::MapShapes(geom_n->GetShape(), M);

    // Access face by the stored index
    const TopoDS_Face& F = TopoDS::Face( M.FindKey(face_idx) );

    // Append filter
    vtkSmartPointer<vtkAppendPolyData>
      appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

    // Explode by edges
    for ( TopExp_Explorer exp(F, TopAbs_EDGE); exp.More(); exp.Next() )
    {
      // Allocate Data Source
      vtkSmartPointer<visu_pcurve_source>
        pcurveSource = vtkSmartPointer<visu_pcurve_source>::New();

      // Access edge
      const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

      // Initialize data source
      pcurveSource->SetEdgeOnFace(E, F);

      // Append poly data
      appendFilter->AddInputConnection( pcurveSource->GetOutputPort() );
    }

    // Initialize pipeline
    this->SetInputConnection( appendFilter->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_face_domain_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_face_domain_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_face_domain_pipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = visu_utils::InitDomainLookupTable();
    visu_utils::InitMapper(m_mapper, aLookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
