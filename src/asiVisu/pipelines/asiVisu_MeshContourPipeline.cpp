//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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
