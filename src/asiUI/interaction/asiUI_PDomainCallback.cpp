//-----------------------------------------------------------------------------
// Created on: 30 March 2016
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
#include <asiUI_PDomainCallback.h>

// UI includes
#include <asiUI_Viewer.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PDomainCallback* asiUI_PDomainCallback::New()
{
  return new asiUI_PDomainCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PDomainCallback* asiUI_PDomainCallback::New(asiUI_Viewer* pViewer)
{
  return new asiUI_PDomainCallback(pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param pViewer [in] owning viewer.
asiUI_PDomainCallback::asiUI_PDomainCallback(asiUI_Viewer* pViewer)
: asiUI_ViewerCallback(pViewer)
{}

//! Destructor.
asiUI_PDomainCallback::~asiUI_PDomainCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PDomainCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                    unsigned long eventId,
                                    void*         asiVisu_NotUsed(pCallData))
{
  vtkSmartPointer<asiVisu_PrsManager> mgr = this->GetViewer()->PrsMgr();

  if ( eventId == EVENT_DELETE )
    emit killEdges();
  else if ( eventId == EVENT_JOIN )
    emit joinEdges();
}
