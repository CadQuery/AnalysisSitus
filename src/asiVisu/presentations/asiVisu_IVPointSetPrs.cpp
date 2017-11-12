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
#include <asiVisu_IVPointSetPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_IVPointSetDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVPointSetPrs::asiVisu_IVPointSetPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_IVPointSetDataProvider) DP = new asiVisu_IVPointSetDataProvider(theNode);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Adjust point size
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetPointSize(8.0);

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVPointSetPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVPointSetPrs(theNode);
}

//! Callback after initialization.
void asiVisu_IVPointSetPrs::afterInitPipelines()
{
  Handle(asiVisu_IVPointSetDataProvider)
    DP = Handle(asiVisu_IVPointSetDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumberOfElements();

  if ( nPts > 1 )
  {
    TCollection_AsciiString title("Num. of points: "); title += nPts;

    // Update text on the annotation
    m_textWidget->GetTextActor()->SetInput( title.ToCString() );
  }
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_IVPointSetPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  Handle(asiVisu_IVPointSetDataProvider)
    DP = Handle(asiVisu_IVPointSetDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumberOfElements();
  //
  if ( nPts > 1 )
  {
    if ( !m_textWidget->GetCurrentRenderer() )
    {
      m_textWidget->SetInteractor      ( theRenderer->GetRenderWindow()->GetInteractor() );
      m_textWidget->SetDefaultRenderer ( theRenderer );
      m_textWidget->SetCurrentRenderer ( theRenderer );
      m_textWidget->On                 ( );
      m_textWidget->ReleaseFocus       ( );
    }
  }
}

//! Callback for de-rendering.
//! \param renderer [in] renderer.
void asiVisu_IVPointSetPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);
  m_textWidget->Off();
}
