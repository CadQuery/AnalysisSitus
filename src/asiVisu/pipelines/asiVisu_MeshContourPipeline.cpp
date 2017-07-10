//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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
#include <asiVisu_MeshContourPipeline.h>

// asiVisu includes
#include <asiVisu_MeshUtils.h>

// VTK includes
#include <vtkExtractEdges.h>
#include <vtkProperty.h>

//! Creates new Mesh Contour Pipeline instance.
//! \param doUseDefaultColor [in] indicates whether to use default color.
asiVisu_MeshContourPipeline::asiVisu_MeshContourPipeline(const bool doUseDefaultColor)
: asiVisu_MeshPipeline()
{
  /* ======================
   *  Basic initialization
   * ====================== */

  if ( doUseDefaultColor )
    asiVisu_MeshUtils::DefaultContourColor(m_fColor[0], m_fColor[1], m_fColor[2]);

  m_fOpacity = asiVisu_MeshUtils::DefaultContourOpacity();
  m_fLineWidth = 1.0;

  // Set visual properties
  this->Actor()->GetProperty()->SetColor(m_fColor[0], m_fColor[1], m_fColor[2]);
  this->Actor()->GetProperty()->SetOpacity(m_fOpacity);
  this->Actor()->GetProperty()->SetLineWidth(m_fLineWidth);
  this->Actor()->SetPickable(0);

  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Filter to extract edges
  vtkSmartPointer<vtkExtractEdges> aEEFilter = vtkSmartPointer<vtkExtractEdges>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_ExtractEdges, aEEFilter);

  this->append( m_filterMap.Find(Filter_ExtractEdges) );
}

//! Callback for AddToRenderer() base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_MeshContourPipeline::callback_add_to_renderer(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer() base routine.
void asiVisu_MeshContourPipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void asiVisu_MeshContourPipeline::callback_update()
{
}
