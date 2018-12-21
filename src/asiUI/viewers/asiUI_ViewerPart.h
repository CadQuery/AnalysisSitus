//-----------------------------------------------------------------------------
// Created on: 27 November 2015
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

#ifndef asiUI_ViewerPart_h
#define asiUI_ViewerPart_h

// asiUI includes
#include <asiUI_PartCallback.h>
#include <asiUI_PickCallback.h>
#include <asiUI_RotationCallback.h>
#include <asiUI_Viewer.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiVisu includes
#include <asiVisu_InteractorStylePick.h>

// VTK includes
#include <vtkOrientationMarkerWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QToolBar>
#pragma warning(pop)

//! Viewer for part.
class asiUI_ViewerPart : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ViewerPart(const Handle(asiEngine_Model)& model,
                     const bool                     enableInteraction,
                     QWidget*                       parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ViewerPart();

public:

  asiUI_EXPORT QSize
    sizeHint() const;

public:

  asiUI_EXPORT void
    Repaint();

public:

  //! \return pick callback.
  const vtkSmartPointer<asiUI_PickCallback>& GetPickCallback() const
  {
    return m_pickCallback;
  }

public slots:

  asiUI_EXPORT void onResetView();
  asiUI_EXPORT void onFitAll();
  asiUI_EXPORT void onSubShapesPicked();
  asiUI_EXPORT void onWhateverPicked();
  asiUI_EXPORT void onFindFace();
  asiUI_EXPORT void onFindEdge();
  asiUI_EXPORT void onRefineTessellation();
  asiUI_EXPORT void onContextMenu(const QPoint&);
  asiUI_EXPORT void onDefaultView();
  asiUI_EXPORT void onBackView();
  asiUI_EXPORT void onBottomView();
  asiUI_EXPORT void onFrontView();
  asiUI_EXPORT void onLeftView();
  asiUI_EXPORT void onRightView();
  asiUI_EXPORT void onTopView();

signals:

  void whateverPicked (asiVisu_PickerResult*);
  void facePicked     (asiVisu_PickerResult*);
  void edgePicked     (asiVisu_PickerResult*);
  void vertexPicked   (asiVisu_PickerResult*);

protected:

  void createActions();

protected:

  //! Data Model instance.
  Handle(asiEngine_Model) m_model;

  //! Part callback.
  vtkSmartPointer<asiUI_PartCallback> m_partCallback;

  //! Pick callback.
  vtkSmartPointer<asiUI_PickCallback> m_pickCallback;

  //! Rotation callback.
  vtkSmartPointer<asiUI_RotationCallback> m_rotoCallback;

  //! Orientation marker widget.
  vtkSmartPointer<vtkOrientationMarkerWidget> m_axesWidget;

  //! Toolbar.
  QToolBar* m_toolBar;

};

#endif
