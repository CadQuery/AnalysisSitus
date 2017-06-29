//-----------------------------------------------------------------------------
// Created on: 13 July 2016
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
#include <asiUI_PartCallback.h>

// UI includes
#include <asiUI_Viewer.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PartCallback* asiUI_PartCallback::New()
{
  return new asiUI_PartCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PartCallback* asiUI_PartCallback::New(asiUI_Viewer* pViewer)
{
  return new asiUI_PartCallback(pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param pViewer [in] owning viewer.
asiUI_PartCallback::asiUI_PartCallback(asiUI_Viewer* pViewer)
: asiUI_ViewerCallback(pViewer)
{}

//! Destructor.
asiUI_PartCallback::~asiUI_PartCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PartCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                 unsigned long eventId,
                                 void*         asiVisu_NotUsed(pCallData))
{
  const vtkSmartPointer<asiVisu_PrsManager>& mgr = this->GetViewer()->PrsMgr();

  if ( eventId == EVENT_FIND_FACE )
    emit findFace();

  if ( eventId == EVENT_FIND_EDGE )
    emit findEdge();

  if ( eventId == EVENT_REFINE_TESSELLATION )
    emit refineTessellation();
}
