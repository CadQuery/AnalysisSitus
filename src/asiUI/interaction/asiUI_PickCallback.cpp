//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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
#include <asiUI_PickCallback.h>

// asiUI includes
#include <asiUI_Viewer.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickCallback* asiUI_PickCallback::New()
{
  return new asiUI_PickCallback(NULL, NULL);
}

//! Instantiation routine accepting viewer.
//! \param model   [in] Data Model instance.
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PickCallback*
  asiUI_PickCallback::New(const Handle(asiEngine_Model)& model,
                          asiUI_Viewer*                  pViewer)
{
  return new asiUI_PickCallback(model, pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param model   [in] Data Model instance.
//! \param pViewer [in] owning viewer.
asiUI_PickCallback::asiUI_PickCallback(const Handle(asiEngine_Model)& model,
                                       asiUI_Viewer*                  pViewer)
: asiUI_ViewerCallback(pViewer),
  m_model(model),
  m_pickType(PickType_World)
{}

//! Default destructor.
asiUI_PickCallback::~asiUI_PickCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PickCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                 unsigned long eventId,
                                 void*         pCallData)
{
  const vtkSmartPointer<asiVisu_PrsManager>& mgr = this->GetViewer()->PrsMgr();

  // Check if the calling context is valid
  if ( eventId != EVENT_PICK_DEFAULT && eventId != EVENT_DETECT_DEFAULT )
    return;
  //
  if ( !this->GetViewer() )
    return;

  // Access selection context
  const int selMode = mgr->GetSelectionMode();

  // Skip for disabled selection
  if ( selMode & SelectionMode_None )
    return;

  // Now pick
  asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(pCallData);
  //
  const asiVisu_SelectionNature sel_type = (eventId == EVENT_PICK_DEFAULT) ? SelectionNature_Pick
                                                                           : SelectionNature_Detection;
  mgr->Pick(pickInput, sel_type, m_pickType);

  // Notify observers
  if ( eventId == EVENT_PICK_DEFAULT )
    emit picked();
}
