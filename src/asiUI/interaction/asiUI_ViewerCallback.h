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

#ifndef asiUI_ViewerCallback_h
#define asiUI_ViewerCallback_h

// asiUI includes
#include <asiUI.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkSmartPointer.h>

class asiUI_Viewer;

//! Base class for all callbacks associated with 3D viewer.
class asiUI_ViewerCallback : public vtkCommand
{
public:

  vtkTypeMacro(asiUI_ViewerCallback, vtkCommand);

public:

  asiUI_EXPORT void
    SetViewer(asiUI_Viewer* pViewer);

  asiUI_EXPORT asiUI_Viewer*
    GetViewer();

protected:

  asiUI_EXPORT
    asiUI_ViewerCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_ViewerCallback();

private:

  //! Host viewer. Notice that lifetime of a Callback instance can be longer
  //! that the lifetime of the viewer because user may close that window. In
  //! that case make sure that SetWindow(NULL) is called in order to keep the
  //! callbacks in a consistent (not initialized however) states.
  asiUI_Viewer* m_pViewer;

};

#endif
