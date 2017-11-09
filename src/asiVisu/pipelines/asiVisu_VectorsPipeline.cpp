//-----------------------------------------------------------------------------
// Created on: 13 January 2017
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
#include <asiVisu_VectorsPipeline.h>

// Visualization includes
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_PointsSource.h>
#include <asiVisu_PointsVectorFilter.h>
#include <asiVisu_VectorsDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
asiVisu_VectorsPipeline::asiVisu_VectorsPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                      vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating vector data
  vtkSmartPointer<asiVisu_PointsVectorFilter>
    vecFilter = vtkSmartPointer<asiVisu_PointsVectorFilter>::New();

  // Allocate Transformation filter
  vtkSmartPointer<vtkTransformPolyDataFilter>
    trsfFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Allocate Glyph filter
  vtkSmartPointer<vtkGlyph3D>
    glyphFilter = vtkSmartPointer<vtkGlyph3D>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_Vector,  vecFilter);
  m_filterMap.Bind(Filter_Trsf,    trsfFilter);
  m_filterMap.Bind(Filter_Glyph3D, glyphFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_Vector) );
  this->append( m_filterMap.Find(Filter_Glyph3D) );
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to
//! VTK POLYGONAL DATA.
//! \param dataProvider [in] Data Provider.
void asiVisu_VectorsPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_VectorsDataProvider)
    DP = Handle(asiVisu_VectorsDataProvider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer< asiVisu_PointsSource<float> >
      pointsSource = vtkSmartPointer< asiVisu_PointsSource<float> >::New();
    //
    pointsSource->SetInputPoints( DP->GetPointsf() );

    // Pass vector array
    asiVisu_PointsVectorFilter*
      vecFilter = asiVisu_PointsVectorFilter::SafeDownCast( m_filterMap.Find(Filter_Vector) );
    //
    vecFilter->SetNormals( DP->GetVectorsf(),
                           DP->GetMaxVectorModulus() );

    // Transform glyphs
    vtkTransformPolyDataFilter*
      trsfFilter = vtkTransformPolyDataFilter::SafeDownCast( m_filterMap.Find(Filter_Trsf) );
    trsfFilter->SetInputConnection( asiVisu_MeshResultUtils::GetVectorGlyph()->GetOutputPort() );
    trsfFilter->SetTransform( asiVisu_MeshResultUtils::GetVectorGlyphTransform() );

    // Adjust glyph filter
    vtkGlyph3D* glyphFilter = vtkGlyph3D::SafeDownCast( m_filterMap.Find(Filter_Glyph3D) );
    glyphFilter->SetSourceConnection( trsfFilter->GetOutputPort() );
    glyphFilter->SetVectorModeToUseVector();
    glyphFilter->SetScaleModeToScaleByVector();
    glyphFilter->SetInputArrayToProcess(1, 0, 0, 0, ARRNAME_POINTCLOUD_VECTORS);
    glyphFilter->SetScaleFactor(1.0); // No global scaling

    // Complete pipeline
    this->SetInputConnection( pointsSource->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for adding the actor to a renderer.
//! \param renderer [in] target renderer.
void asiVisu_VectorsPipeline::callback_add_to_renderer(vtkRenderer* renderer)
{
  asiVisu_NotUsed(renderer);
}

//-----------------------------------------------------------------------------

//! Callback for removal of the actor from a renderer.
//! \param renderer [in] target renderer.
void asiVisu_VectorsPipeline::callback_remove_from_renderer(vtkRenderer* renderer)
{
  asiVisu_NotUsed(renderer);
}

//-----------------------------------------------------------------------------

//! Callback on pipeline update.
void asiVisu_VectorsPipeline::callback_update()
{}
