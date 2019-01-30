//-----------------------------------------------------------------------------
// Created on: 30 January 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiVisu_SurfaceDeviationPipeline.h>

// asiVisu includes
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_SurfaceDeviationDataProvider.h>

// VTK includes
#include <vtkPolyDataMapper.h>

// OCCT includes
#include <BRep_Tool.hxx>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_SurfaceDeviationPipeline::asiVisu_SurfaceDeviationPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  //
  m_iStepsNumber (0),
  //
  m_source( vtkSmartPointer<asiVisu_SurfaceSource>::New() )
{}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param[in] DP Data Provider.
void asiVisu_SurfaceDeviationPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_SurfaceDeviationDataProvider)
    dp = Handle(asiVisu_SurfaceDeviationDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  double uMin, uMax, vMin, vMax;
  Handle(Geom_Surface)      surf = dp->GetSurface(uMin, uMax, vMin, vMax);
  Handle(asiAlgo_BVHFacets) mesh = dp->GetReferenceMesh();

  if ( !m_iStepsNumber || surf.IsNull() || mesh.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline.
    vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp.
    return; // Do nothing.
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( dp->MustExecute( this->GetMTime() ) )
  {
    // Configure surface source
    m_source->SetInputSurface  (surf);
    m_source->SetReferenceMesh (mesh);
    m_source->SetScalars       (asiVisu_SurfaceSource::Scalars_Deviation);
    m_source->SetNumberOfSteps (m_iStepsNumber );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_SurfaceDeviationPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_SurfaceDeviationPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_SurfaceDeviationPipeline::callback_update()
{
  const double min_k = m_source->GetMinScalar();
  const double max_k = m_source->GetMaxScalar();
  //
  if ( m_source->HasScalars() )
    asiVisu_MeshResultUtils::InitPointScalarMapper(m_mapper, ARRNAME_SURF_SCALARS,
                                                   min_k, max_k, false);
}
