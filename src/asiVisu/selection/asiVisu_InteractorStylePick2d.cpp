//-----------------------------------------------------------------------------
// Created on: 17 December 2015
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
#include <asiVisu_InteractorStylePick2d.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

// Qt includes
#pragma warning(push, 0)
#include <QPoint>
#pragma warning(pop)

vtkStandardNewMacro(asiVisu_InteractorStylePick2d);

//! Default constructor.
asiVisu_InteractorStylePick2d::asiVisu_InteractorStylePick2d()
: vtkInteractorStyleImage()
{
  m_pPickInput = new asiVisu_PickInput();
  //
  m_PickedPos[0] = m_PickedPos[1] = 0;
}

//! Default destructor.
asiVisu_InteractorStylePick2d::~asiVisu_InteractorStylePick2d() 
{
  delete m_pPickInput;
}

//! Prohibited copy constructor.
asiVisu_InteractorStylePick2d::asiVisu_InteractorStylePick2d(const asiVisu_InteractorStylePick2d&)
: vtkInteractorStyleImage(),
  m_pPickInput(nullptr)
{
}

//! Prohibited assignment operator.
void asiVisu_InteractorStylePick2d::operator=(const asiVisu_InteractorStylePick2d&)
{
}

//! Sets Renderer instance.
//! \param theRenderer [in] Renderer instance to set.
void asiVisu_InteractorStylePick2d::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Returns Renderer instance.
//! \return Renderer instance.
vtkSmartPointer<vtkRenderer> asiVisu_InteractorStylePick2d::GetRenderer() const
{
  return m_renderer;
}

//! Callback for "Mouse Move" event.
void asiVisu_InteractorStylePick2d::OnMouseMove()
{
  // Invoke basic method
  vtkInteractorStyleImage::OnMouseMove();

  const int pos[2] = { this->Interactor->GetEventPosition()[0],
                       this->Interactor->GetEventPosition()[1] };

  QPoint aPickPoint(pos[0], pos[1]);
  m_pPickInput->Start      = aPickPoint;
  m_pPickInput->Finish     = aPickPoint;
  m_pPickInput->IsMultiple = false;

  // Invoke observers
  this->InvokeEvent(EVENT_DETECT_DEFAULT, m_pPickInput);
}

//! Callback for "Left Button Down" event.
void asiVisu_InteractorStylePick2d::OnLeftButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);

  if ( this->CurrentRenderer == nullptr )
    return;

  m_PickedPos[0] = this->Interactor->GetEventPosition()[0];
  m_PickedPos[1] = this->Interactor->GetEventPosition()[1];
  //
  QPoint aPickPoint(m_PickedPos[0], m_PickedPos[1]);
  m_pPickInput->Start  = aPickPoint;
  m_pPickInput->Finish = aPickPoint;

  if ( this->Interactor->GetShiftKey() )
    m_pPickInput->IsMultiple = true;
  else
    m_pPickInput->IsMultiple = false;

  // Invoke observers
  this->InvokeEvent(EVENT_PICK_DEFAULT, m_pPickInput);
}

//! Callback for "Key Press" event.
void asiVisu_InteractorStylePick2d::OnKeyPress()
{
  std::string key = this->Interactor->GetKeySym();
  if ( key == "Delete" )
    this->InvokeEvent(EVENT_DELETE);
  if ( key == "j" )
    this->InvokeEvent(EVENT_JOIN);
}
