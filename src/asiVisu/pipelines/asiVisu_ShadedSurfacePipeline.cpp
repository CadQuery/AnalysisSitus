//-----------------------------------------------------------------------------
// Created on: 01 April 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
