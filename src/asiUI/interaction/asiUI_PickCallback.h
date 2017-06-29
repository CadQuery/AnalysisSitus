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

#ifndef asiUI_PickCallback_h
#define asiUI_PickCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiVisu includes
#include <asiVisu_Selection.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for pick operation. Cooperates with VTK Selector via Qt signals
//! and with VTK window via VTK Command pattern. Therefore, this class is a
//! messaging port between Qt part of the application and its interactive
//! VTK-based part. Notice that not all callbacks have to play such a role.
//! For example, Zoom Area Callback is a pure VTK class as zooming
//! functionality is somewhat completely covered by VTK library. Unlike
//! zooming, the picking operation requires at least some synchronization with
//! Qt-based GUI. That is why such operations are involved into more
//! sophisticated event chaining mechanism:
//!
//! User-click in VTK window -> Pick Callback -> GUI Observers
class asiUI_PickCallback : public QObject,
                           public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  asiUI_EXPORT static asiUI_PickCallback*
    New();

  asiUI_EXPORT static asiUI_PickCallback*
    New(const Handle(asiEngine_Model)& model,
        asiUI_Viewer*                  pViewer);

  vtkTypeMacro(asiUI_PickCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

public:

  //! Sets Data Model instance.
  //! \param model [in] Data Model instance to set.
  void SetModel(const Handle(asiEngine_Model)& model)
  {
    m_model = model;
  }

  //! Sets type of picker for a workpiece.
  //! \param type [in] type of picker to set.
  void SetPickerType(const asiVisu_PickType type)
  {
    m_pickType = type;
  }

  //! \return type of picker for a workpiece.
  asiVisu_PickType GetPickerType() const
  {
    return m_pickType;
  }

signals:

  void picked();

protected:

  asiUI_EXPORT
    asiUI_PickCallback(const Handle(asiEngine_Model)& model,
                       asiUI_Viewer*                  pViewer);

  asiUI_EXPORT
    ~asiUI_PickCallback();

protected:

  Handle(asiEngine_Model) m_model;    //!< Data Model instance.
  asiVisu_PickType        m_pickType; //!< Type of picker for a workpiece.

};

#endif
