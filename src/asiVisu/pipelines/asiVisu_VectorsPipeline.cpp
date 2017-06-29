//-----------------------------------------------------------------------------
// Created on: 13 January 2017
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
