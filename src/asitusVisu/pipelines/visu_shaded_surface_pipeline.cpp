//-----------------------------------------------------------------------------
// Created on: 01 April 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_shaded_surface_pipeline.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_mesh_result_utils.h>
#include <visu_face_data_provider.h>
#include <visu_utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>

// OCCT includes
#include <BRep_Tool.hxx>

//-----------------------------------------------------------------------------

//! Creates new Shaded Surface Pipeline initialized by default VTK mapper and actor.
visu_shaded_surface_pipeline::visu_shaded_surface_pipeline()
//
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  //
  m_iStepsNumber (0),
  m_bForced      (false),
  //
  m_source( vtkSmartPointer<visu_surface_source>::New() )
{}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_shaded_surface_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_face_data_provider)
    faceProvider = Handle(visu_face_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  const int face_idx = faceProvider->GetFaceIndexAmongSubshapes();
  if ( !m_iStepsNumber || !face_idx )
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

  if ( m_bForced || faceProvider->MustExecute( this->GetMTime() ) )
  {
    if ( m_bForced ) m_bForced = false; // Reset forced execution flag

    TopoDS_Face F = faceProvider->ExtractFace();
    //
    if ( F.IsNull() )
      return;

    // Configure surface source
    m_source->SetInputSurface  ( BRep_Tool::Surface(F) );
    m_source->SetNumberOfSteps ( m_iStepsNumber );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! \return true if any curvature field is enabled.
bool visu_shaded_surface_pipeline::IsCurvature() const
{
  return m_source->HasScalars();
}

//! \return true if Gaussian curvature field is enabled.
bool visu_shaded_surface_pipeline::IsGaussianCurvature() const
{
  return m_source->GetScalarType() == visu_surface_source::Scalars_GaussianCurvature;
}

//! \return true if mean curvature field is enabled.
bool visu_shaded_surface_pipeline::IsMeanCurvature() const
{
  return m_source->GetScalarType() == visu_surface_source::Scalars_MeanCurvature;
}

//! Enables Gaussian curvature field.
void visu_shaded_surface_pipeline::GaussianCurvatureOn()
{
  m_source->SetScalars(visu_surface_source::Scalars_GaussianCurvature);
}

//! Enables mean curvature field.
void visu_shaded_surface_pipeline::MeanCurvatureOn()
{
  m_source->SetScalars(visu_surface_source::Scalars_MeanCurvature);
}

//! Disables curvature field.
void visu_shaded_surface_pipeline::CurvatureOff()
{
  m_source->SetScalars(visu_surface_source::Scalars_NoScalars);
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_shaded_surface_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_shaded_surface_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_shaded_surface_pipeline::callback_update()
{
  const double min_k = m_source->GetMinScalar();
  const double max_k = m_source->GetMaxScalar();
  //
  if ( m_source->HasScalars() )
    visu_mesh_result_utils::InitPointScalarMapper(m_mapper, ARRNAME_SURF_CURVATURE,
                                                  min_k, max_k, false);
}
