//-----------------------------------------------------------------------------
// Created on: 11 April 2016
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
#include <asiVisu_ReVertexPrs.h>

// asiVisu includes
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_ReVertexDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//-----------------------------------------------------------------------------

asiVisu_ReVertexPrs::asiVisu_ReVertexPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  // Initialize colors.
  m_color.r = 255.;
  m_color.g = 153./255.;
  m_color.b = 17./255.;
  //
  m_hiliColor.r = 155.;
  m_hiliColor.g = 53./255.;
  m_hiliColor.b = 7./255.;

  // Create Data Provider.
  Handle(asiVisu_ReVertexDataProvider)
    DP = new asiVisu_ReVertexDataProvider( Handle(asiData_ReVertexNode)::DownCast(N) );

  // Pipeline for points.
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline(false) );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Adjust point size.
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetPointSize(20.0);
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetColor(m_color.r,
                                                                     m_color.g,
                                                                     m_color.b);
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs) asiVisu_ReVertexPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_ReVertexPrs(N);
}

//-----------------------------------------------------------------------------

void asiVisu_ReVertexPrs::highlight(vtkRenderer*                        renderer,
                                    const Handle(asiVisu_PickerResult)& pickRes,
                                    const asiVisu_SelectionNature       selNature) const
{
  asiVisu_NotUsed(renderer);
  asiVisu_NotUsed(pickRes);
  asiVisu_NotUsed(selNature);

  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetColor(m_hiliColor.r,
                                                                     m_hiliColor.g,
                                                                     m_hiliColor.b);
}

//-----------------------------------------------------------------------------

void asiVisu_ReVertexPrs::unHighlight(vtkRenderer*                  renderer,
                                      const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);
  asiVisu_NotUsed(selNature);

  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetColor(m_color.r,
                                                                     m_color.g,
                                                                     m_color.b);
}
