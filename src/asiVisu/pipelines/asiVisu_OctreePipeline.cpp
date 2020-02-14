//-----------------------------------------------------------------------------
// Created on: 25 November 2019
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
#include <asiVisu_OctreePipeline.h>

// asiVisu includes
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_OctreeDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkUnstructuredGrid.h>

//-----------------------------------------------------------------------------

//! Ctor.
asiVisu_OctreePipeline::asiVisu_OctreePipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkDataSetMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  m_fToler        (0.),
  m_fMinScalar    (1.e100),
  m_fMaxScalar    (-1.e100)
{}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param[in] DP Data Provider.
void asiVisu_OctreePipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_OctreeDataProvider)
    provider = Handle(asiVisu_OctreeDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  void* pOctree = provider->GetOctree();
  //
  if ( pOctree == nullptr )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkUnstructuredGrid> dummyDS = vtkSmartPointer<vtkUnstructuredGrid>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( provider->MustExecute( this->GetMTime() ) )
  {
    m_source = vtkSmartPointer<asiVisu_OctreeSource>::New();
    //
    m_source->SetInputFacets      ( provider->GetFacets() );
    m_source->SetInputOctree      ( pOctree );
    m_source->SetSamplingStrategy ( provider->GetSamplingStrategy() );
    m_source->SetExtractPoints    ( provider->IsPointExtraction() );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

bool asiVisu_OctreePipeline::initLookupTable()
{
  if ( !m_source.Get() )
    return false;

  // Update source to access the scalar range.
  m_source->Update();

  // Get scalar range.
  const double minScalar = Precision::IsInfinite(m_fMinScalar) ? m_source->GetMinScalar() : m_fMinScalar;
  const double maxScalar = Precision::IsInfinite(m_fMaxScalar) ? m_source->GetMaxScalar() : m_fMaxScalar;
  const double range     = maxScalar - minScalar;

  // Extra variables.
  const int numColors = 256;

  // Build and initialize lookup table.
  m_lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  //
  m_lookupTable->SetTableRange(minScalar, maxScalar);
  m_lookupTable->SetScaleToLinear();
  m_lookupTable->SetNumberOfTableValues(numColors + 1);

  // Populate table.
  double r, g, b;
  for ( int i = 0; i < numColors + 1; i++ )
  {
    const double val = minScalar + ( (double) i / numColors ) * range;
    asiVisu_MeshResultUtils::GetColdHotColorForValue(val, minScalar, maxScalar, m_fToler, r, g, b);
    m_lookupTable->SetTableValue(i, r, g, b);
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_OctreePipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_OctreePipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_OctreePipeline::callback_update()
{
  // Initialize lookup table.
  if ( this->initLookupTable() )
  {
    // Bind lookup table to mapper.
    asiVisu_MeshResultUtils::InitPointScalarMapper(m_mapper, m_lookupTable,
                                                   ARRNAME_VOXEL_N_SCALARS,
                                                   true);
  }
}
