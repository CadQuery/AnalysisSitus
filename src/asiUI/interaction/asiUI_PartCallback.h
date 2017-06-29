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

#ifndef asiUI_PartCallback_h
#define asiUI_PartCallback_h

// A-Situs (visualization) includes
#include <asiUI_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Part viewer.
class asiUI_PartCallback : public QObject,
                           public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  asiUI_EXPORT static asiUI_PartCallback*
    New();

  asiUI_EXPORT static asiUI_PartCallback*
    New(asiUI_Viewer* pViewer);

  vtkTypeMacro(asiUI_PartCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

signals:

  void findFace();
  void findEdge();
  void refineTessellation();

private:

  asiUI_EXPORT
    asiUI_PartCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_PartCallback();

};

#endif
