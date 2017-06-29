//-----------------------------------------------------------------------------
// Created on: 17 December 2015
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
  m_pPickInput(NULL)
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

  if ( this->CurrentRenderer == NULL )
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
