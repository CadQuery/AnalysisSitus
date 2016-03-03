//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

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
    TopoDS_Face F = faceProvider->ExtractFace();

    // Compute tip size for the orientation markers
    const double tip_size = this->computeTipSize(F);

    // Append filter
    vtkSmartPointer<vtkAppendPolyData>
      appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

    // Explode by edges
    for ( TopExp_Explorer exp(F.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next() )
    {
      // Allocate Data Source
      vtkSmartPointer<visu_pcurve_source>
        pcurveSource = vtkSmartPointer<visu_pcurve_source>::New();

      // Access edge
      const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

      // Initialize data source
      pcurveSource->SetEdgeOnFace(E, F);
      pcurveSource->SetTipSize(tip_size);

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

//-----------------------------------------------------------------------------

//! Computes size for orientation tip glyph. This size is decided for the
//! whole face to have it identical for all edges.
//! \param F [in] face to compute the orientation tip size for.
//! \return tip size.
double visu_face_domain_pipeline::computeTipSize(const TopoDS_Face& F) const
{
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(F, uMin, uMax, vMin, vMax);

  // Take a ratio of a bounding diagonal
  return ( gp_XY(uMax, vMax) - gp_XY(uMin, vMin) ).Modulus() * 0.025;
}
