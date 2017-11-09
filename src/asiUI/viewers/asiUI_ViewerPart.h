//-----------------------------------------------------------------------------
// Created on: 27 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
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
#include <QToolBar>

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

  void onResetView();
  void onSubShapesPicked();
  void onWhateverPicked();
  void onFindFace();
  void onFindEdge();
  void onRefineTessellation();
  void onContextMenu(const QPoint&);
  void onDefaultView();
  void onBackView();
  void onBottomView();
  void onFrontView();
  void onLeftView();
  void onRightView();
  void onTopView();

signals:

  void pointPicked(const double, const double, const double);
  void facePicked(const asiVisu_PickResult&);
  void edgePicked(const asiVisu_PickResult&);
  void contextMenu(const QPoint&);

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
