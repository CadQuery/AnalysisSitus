//-----------------------------------------------------------------------------
// Created on: 21 July 2016
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
#include <asiVisu_IVTextItemPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVTextItemPrs::asiVisu_IVTextItemPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_DefaultPrs(theNode)
{
  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVTextItemPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVTextItemPrs(theNode);
}

//! Callback after initialization.
void asiVisu_IVTextItemPrs::afterInitPipelines()
{
  TCollection_AsciiString
    title = Handle(asiData_IVTextItemNode)::DownCast(m_node)->GetText();

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( title.ToCString() );
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_IVTextItemPrs::renderPipelines(vtkRenderer* theRenderer) const
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

//! Callback for de-rendering.
//! \param renderer [in] renderer.
void asiVisu_IVTextItemPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);
  m_textWidget->Off();
}
