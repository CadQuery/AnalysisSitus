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
