//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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
#include <asiVisu_InteractorStylePick.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

// Qt includes
#pragma warning(push, 0)
#include <QPoint>
#pragma warning(pop)

vtkStandardNewMacro(asiVisu_InteractorStylePick)

//! Default constructor.
asiVisu_InteractorStylePick::asiVisu_InteractorStylePick()
: vtkInteractorStyleTrackballCamera (),
  m_bIsLeftButtonDown               (false),
  m_bIsRotation                     (false)
{
  m_pPickInput = new asiVisu_PickInput();
  //
  m_PickedPos[0] = m_PickedPos[1] = 0;
}

//! Default destructor.
asiVisu_InteractorStylePick::~asiVisu_InteractorStylePick() 
{
  delete m_pPickInput;
}

//! Prohibited copy constructor.
asiVisu_InteractorStylePick::asiVisu_InteractorStylePick(const asiVisu_InteractorStylePick&)
: vtkInteractorStyleTrackballCamera (),
  m_pPickInput                      (nullptr),
  m_bIsLeftButtonDown               (false),
  m_bIsRotation                     (false)
{
}

//! Prohibited assignment operator.
void asiVisu_InteractorStylePick::operator=(const asiVisu_InteractorStylePick&)
{}

//! Sets Renderer instance.
//! \param theRenderer [in] Renderer instance to set.
void asiVisu_InteractorStylePick::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Returns Renderer instance.
//! \return Renderer instance.
const vtkSmartPointer<vtkRenderer>& asiVisu_InteractorStylePick::GetRenderer() const
{
  return m_renderer;
}

//! Callback for "Mouse Move" event.
void asiVisu_InteractorStylePick::OnMouseMove()
{
  // Invoke basic method
  vtkInteractorStyleTrackballCamera::OnMouseMove();

  m_PickedPos[0] = this->Interactor->GetEventPosition()[0];
  m_PickedPos[1] = this->Interactor->GetEventPosition()[1];

  QPoint aPickPoint(m_PickedPos[0], m_PickedPos[1]);
  m_pPickInput->Start      = aPickPoint;
  m_pPickInput->Finish     = aPickPoint;
  m_pPickInput->IsMultiple = false;

  if ( m_bIsLeftButtonDown )
  {
    if ( !m_bIsRotation )
      m_bIsRotation = true;

    this->InvokeEvent(EVENT_ROTATION_START, m_pPickInput);
    return;
  }

  // Invoke observers
  if ( !m_bIsRotation )
    this->InvokeEvent(EVENT_DETECT_DEFAULT, m_pPickInput);
  //
  if ( m_bIsLeftButtonDown && !this->Interactor->GetShiftKey() )
  {
    this->InvokeEvent(EVENT_ROTATION_START, m_pPickInput);
    //
    for ( NCollection_Sequence<unsigned long>::Iterator it(m_rotationCallbackIds);
          it.More(); it.Next() )
    {
      this->InvokeEvent( it.Value() );
    }
  }
}

void asiVisu_InteractorStylePick::OnRightButtonDown()
{
  // Allow the default processing only if the control key is pressed.
  // Otherwise, we'd like to use our custom behavior such as popping
  // up context menus.
  if ( this->Interactor->GetControlKey() )
    vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

//! Callback for "Left Button Down" event.
void asiVisu_InteractorStylePick::OnLeftButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);

  if ( this->CurrentRenderer == nullptr )
    return;

  m_bIsLeftButtonDown = true;

  m_PickedPos[0] = this->Interactor->GetEventPosition()[0];
  m_PickedPos[1] = this->Interactor->GetEventPosition()[1];

  this->StartRotate();
}

//! Callback for "Left Button Up" event.
void asiVisu_InteractorStylePick::OnLeftButtonUp()
{
  vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
  m_bIsLeftButtonDown = false;

  const int pos[2] = { this->Interactor->GetEventPosition()[0],
                       this->Interactor->GetEventPosition()[1] };
  //
  if ( pos[0] == m_PickedPos[0] && pos[1] == m_PickedPos[1] )
  {
    QPoint aPickPoint(m_PickedPos[0], m_PickedPos[1]);
    m_pPickInput->Start  = aPickPoint;
    m_pPickInput->Finish = aPickPoint;

    if ( this->Interactor->GetShiftKey() )
    {
      m_pPickInput->IsMultiple = true;
    }
    else
    {
      m_pPickInput->IsMultiple = false;
    }

    // Invoke observers
    if ( !m_bIsRotation )
      this->InvokeEvent(EVENT_PICK_DEFAULT, m_pPickInput);
  }
  else
  {
    m_PickedPos[0] = pos[0];
    m_PickedPos[1] = pos[1];
  }

  // Invoke observers
  m_bIsRotation = false;
  this->EndRotate();
  this->InvokeEvent(EVENT_ROTATION_END, nullptr);
}

//! Callback for "Key Press" event.
void asiVisu_InteractorStylePick::OnKeyPress()
{
  std::string key = this->Interactor->GetKeySym();

  if ( this->Interactor->GetControlKey() && key == "f" )
    this->InvokeEvent(EVENT_FIND_FACE);
  //
  if ( this->Interactor->GetControlKey() && key == "e" )
    this->InvokeEvent(EVENT_FIND_EDGE);
  //
  if ( this->Interactor->GetControlKey() && key == "t" )
    this->InvokeEvent(EVENT_REFINE_TESSELLATION);
}

bool asiVisu_InteractorStylePick::IsControlPressed() const
{
  return this->Interactor ? this->Interactor->GetControlKey() : false;
}

//! Callback for rotation finishing action.
void asiVisu_InteractorStylePick::EndRotate()
{
  vtkInteractorStyleTrackballCamera::EndRotate();
}

//! Adds custom callback.
//! \param theEventID  [in] ID of callback action.
//! \param theCallback [in] command to be added.
//! \return tag of the event (see vtkObject::AddObserver() method for VTK event tags).
unsigned long asiVisu_InteractorStylePick::AddRotationCallback(unsigned long theEventID,
                                                               vtkCommand*   theCallback)
{
  m_rotationCallbackIds.Append(theEventID);
  return this->AddObserver(theEventID, theCallback);
}

//! Removes a callback with tag that is returned by AddRotationCallback() method.
//! \param theEventID [in] the event ID on which events are invoked
//! \param theTag     [in] the tag which is assigned to particular event entity.
//! \return true/false.
bool asiVisu_InteractorStylePick::RemoveRotationCallback(unsigned long theEventID,
                                                         unsigned long theTag)
{
  if ( m_rotationCallbackIds.IsEmpty() )
    return false;

  this->RemoveObserver(theTag);

  if ( !this->HasObserver(theEventID) )
    m_rotationCallbackIds.Remove(theEventID);

  return true;
}
