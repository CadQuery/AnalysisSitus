//-----------------------------------------------------------------------------
// Created on: 27 November 2015
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
