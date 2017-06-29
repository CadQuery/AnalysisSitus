//-----------------------------------------------------------------------------
// Created on: 01 April 2015
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
#include <asiVisu_ShadedSurfacePipeline.h>

// Visualization includes
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_FaceDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>

// OCCT includes
#include <BRep_Tool.hxx>

//-----------------------------------------------------------------------------

//! Creates new Shaded Surface Pipeline initialized by default VTK mapper and actor.
asiVisu_ShadedSurfacePipeline::asiVisu_ShadedSurfacePipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  //
  m_iStepsNumber (0),
  m_bForced      (false),
  //
  m_source( vtkSmartPointer<asiVisu_SurfaceSource>::New() )
{}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_ShadedSurfacePipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_FaceDataProvider)
    faceProvider = Handle(asiVisu_FaceDataProvider)::DownCast(DP);

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
bool asiVisu_ShadedSurfacePipeline::IsCurvature() const
{
  return m_source->HasScalars();
}

//! \return true if Gaussian curvature field is enabled.
bool asiVisu_ShadedSurfacePipeline::IsGaussianCurvature() const
{
  return m_source->GetScalarType() == asiVisu_SurfaceSource::Scalars_GaussianCurvature;
}

//! \return true if mean curvature field is enabled.
bool asiVisu_ShadedSurfacePipeline::IsMeanCurvature() const
{
  return m_source->GetScalarType() == asiVisu_SurfaceSource::Scalars_MeanCurvature;
}

//! Enables Gaussian curvature field.
void asiVisu_ShadedSurfacePipeline::GaussianCurvatureOn()
{
  m_source->SetScalars(asiVisu_SurfaceSource::Scalars_GaussianCurvature);
}

//! Enables mean curvature field.
void asiVisu_ShadedSurfacePipeline::MeanCurvatureOn()
{
  m_source->SetScalars(asiVisu_SurfaceSource::Scalars_MeanCurvature);
}

//! Disables curvature field.
void asiVisu_ShadedSurfacePipeline::CurvatureOff()
{
  m_source->SetScalars(asiVisu_SurfaceSource::Scalars_NoScalars);
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_ShadedSurfacePipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_ShadedSurfacePipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_ShadedSurfacePipeline::callback_update()
{
  const double min_k = m_source->GetMinScalar();
  const double max_k = m_source->GetMaxScalar();
  //
  if ( m_source->HasScalars() )
    asiVisu_MeshResultUtils::InitPointScalarMapper(m_mapper, ARRNAME_SURF_CURVATURE,
                                                   min_k, max_k, false);
}
