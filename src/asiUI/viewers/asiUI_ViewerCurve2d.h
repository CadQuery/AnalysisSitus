//-----------------------------------------------------------------------------
// Created on: 04 February 2016
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

#ifndef asiUI_ViewerCurve2d_h
#define asiUI_ViewerCurve2d_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_Viewer.h>

// A-Situs (visualization) includes
#include <asiVisu_InteractorStylePick.h>
#include <asiUI_PickCallback.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for a two-dimensional parametric curve.
class asiUI_ViewerCurve2d : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ViewerCurve2d(vtkSmartPointer<asiVisu_PrsManager>& prs_mgr,
                        QWidget*                             parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ViewerCurve2d();

public:

  void Repaint();

public slots:

  void onResetView();

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
