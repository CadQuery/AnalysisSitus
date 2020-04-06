//-----------------------------------------------------------------------------
// Created on: 13 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiVisu_MeshEScalarPipeline.h>

// asiVisu includes
#include <asiVisu_MeshEScalarDataProvider.h>
#include <asiVisu_MeshEScalarFilter.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_MeshSource.h>
#include <asiVisu_NodeInfo.h>
#include <asiVisu_TriangulationSource.h>

// OpenCascade includes
#include <Precision.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
asiVisu_MeshEScalarPipeline::asiVisu_MeshEScalarPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  m_fToler(0.)
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating scalar arrays
  vtkSmartPointer<asiVisu_MeshEScalarFilter>
    scFilter = vtkSmartPointer<asiVisu_MeshEScalarFilter>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_EScalar, scFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_EScalar) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void asiVisu_MeshEScalarPipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_MeshEScalarDataProvider)
    meshDp = Handle(asiVisu_MeshEScalarDataProvider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( meshDp->MustExecute( this->GetMTime() ) )
  {
    // Prepare source.
    vtkSmartPointer<asiVisu_TriangulationSource>
      trisSource = vtkSmartPointer<asiVisu_TriangulationSource>::New();
    //
    trisSource->CollectEdgesModeOff();
    trisSource->CollectVerticesModeOff();
    trisSource->SetInputTriangulation( meshDp->GetTriangulation() );

    // Initialize scalar range.
    m_fMinScalar = meshDp->GetMinScalar();
    m_fMaxScalar = meshDp->GetMaxScalar();

    // Initialize scalars filter.
    asiVisu_MeshEScalarFilter*
      scFilter = asiVisu_MeshEScalarFilter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );
    //
    scFilter->SetScalarArrays( meshDp->GetElementIDs(),
                               meshDp->GetElementScalars() );

    // Complete pipeline.
    this->SetInputConnection( trisSource->GetOutputPort() );

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object.
    asiVisu_NodeInfo::Store( meshDp->GetNodeID(), this->Actor() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

void asiVisu_MeshEScalarPipeline::initLookupTable()
{
  // Get scalar filter to access the scalar range.
  asiVisu_MeshEScalarFilter*
    scFilter = asiVisu_MeshEScalarFilter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );
  //
  scFilter->Update();

  // Get scalar range.
  const double minScalar = Precision::IsInfinite(m_fMinScalar) ? scFilter->GetMinScalar() : m_fMinScalar;
  const double maxScalar = Precision::IsInfinite(m_fMaxScalar) ? scFilter->GetMaxScalar() : m_fMaxScalar;
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
}

//-----------------------------------------------------------------------------

void asiVisu_MeshEScalarPipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToFlat();
}

//-----------------------------------------------------------------------------

void asiVisu_MeshEScalarPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

void asiVisu_MeshEScalarPipeline::callback_update()
{
  // Initialize lookup table.
  this->initLookupTable();

  // Bind lookup table to mapper.
  asiVisu_MeshResultUtils::InitCellScalarMapper(m_mapper, m_lookupTable,
                                                ARRNAME_MESH_E_SCALARS,
                                                true);
}
