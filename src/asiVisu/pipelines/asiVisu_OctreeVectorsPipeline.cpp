//-----------------------------------------------------------------------------
// Created on: 18 February 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiVisu_OctreeVectorsPipeline.h>

// asiVisu includes
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_OctreeDataProvider.h>
#include <asiVisu_OctreeSource.h>
#include <asiVisu_PointsVectorFilter.h>

// VTK includes
#include <vtkActor.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkUnstructuredGrid.h>

//-----------------------------------------------------------------------------

asiVisu_OctreeVectorsPipeline::asiVisu_OctreeVectorsPipeline(const vtkSmartPointer<asiVisu_OctreeSource>& source)
: asiVisu_Pipeline ( vtkSmartPointer<vtkPolyDataMapper>::New(),
                     vtkSmartPointer<vtkActor>::New() ),
  m_source         ( source )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating vector data.
  vtkSmartPointer<asiVisu_PointsVectorFilter>
    vecFilter = vtkSmartPointer<asiVisu_PointsVectorFilter>::New();

  // Allocate Transformation filter.
  vtkSmartPointer<vtkTransformPolyDataFilter>
    trsfFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Allocate Glyph filter.
  vtkSmartPointer<vtkGlyph3D>
    glyphFilter = vtkSmartPointer<vtkGlyph3D>::New();

  /* =========================
   *  Register custom filters.
   * ========================= */

  m_filterMap.Bind(Filter_Vector,  vecFilter);
  m_filterMap.Bind(Filter_Trsf,    trsfFilter);
  m_filterMap.Bind(Filter_Glyph3D, glyphFilter);

  // Append custom filters to the pipeline.
  this->append( m_filterMap.Find(Filter_Vector) );
  this->append( m_filterMap.Find(Filter_Glyph3D) );
}

//-----------------------------------------------------------------------------

void asiVisu_OctreeVectorsPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_OctreeDataProvider)
    provider = Handle(asiVisu_OctreeDataProvider)::DownCast(dataProvider);

  /* ===========================
   *  Validate input Parameters.
   * =========================== */

  void* pOctree = provider->GetOctree();
  //
  if ( pOctree == nullptr || !m_source->IsExtractPoints() )
  {
    // Pass empty data set in order to have valid pipeline.
    vtkSmartPointer<vtkUnstructuredGrid> dummyDS = vtkSmartPointer<vtkUnstructuredGrid>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp.
    return;
  }

  /* ============================
   *  Prepare polygonal data set.
   * ============================ */

  if ( provider->MustExecute( this->GetMTime() ) )
  {
    m_source->Update();

    // Pass vectors.
    asiVisu_PointsVectorFilter*
      vecFilter = asiVisu_PointsVectorFilter::SafeDownCast( m_filterMap.Find(Filter_Vector) );
    //
    vecFilter->SetNormals( m_source->GetVectors(),
                           provider->GetMaxVectorModulus() );

    // Transform glyphs.
    vtkTransformPolyDataFilter*
      trsfFilter = vtkTransformPolyDataFilter::SafeDownCast( m_filterMap.Find(Filter_Trsf) );
    //
    trsfFilter->SetInputConnection( asiVisu_MeshResultUtils::GetVectorGlyph()->GetOutputPort() );
    trsfFilter->SetTransform( asiVisu_MeshResultUtils::GetVectorGlyphTransform() );

    // Adjust glyph filter.
    vtkGlyph3D* glyphFilter = vtkGlyph3D::SafeDownCast( m_filterMap.Find(Filter_Glyph3D) );
    //
    glyphFilter->SetSourceConnection( trsfFilter->GetOutputPort() );
    glyphFilter->SetVectorModeToUseVector();
    glyphFilter->SetScaleModeToScaleByVector();
    glyphFilter->SetInputArrayToProcess(1, 0, 0, 0, ARRNAME_POINTCLOUD_VECTORS);
    glyphFilter->SetScaleFactor(1.0); // No global scaling.

    // Complete pipeline.
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp.
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_OctreeVectorsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_OctreeVectorsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback on pipeline update.
void asiVisu_OctreeVectorsPipeline::callback_update()
{}
