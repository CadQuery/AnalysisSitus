//-----------------------------------------------------------------------------
// Created on: 02 December 2015
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

#ifndef asiUI_ViewerDomain_h
#define asiUI_ViewerDomain_h

// asiUI includes
#include <asiUI_PDomainCallback.h>
#include <asiUI_PickCallback.h>
#include <asiUI_Viewer.h>

// asiEngine includes
#include <asiEngine_Model.h>

// VTK includes
#include <vtkTextWidget.h>

//! Viewer for domains of topological entities (edges, faces).
class asiUI_ViewerDomain : public asiUI_Viewer
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ViewerDomain(const Handle(asiEngine_Model)& model,
                       QWidget*                       parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ViewerDomain();

public:

  asiUI_EXPORT QSize
    sizeHint() const;

public:

  void Repaint();

public slots:

  void onResetView();
  void onDomainPicked();
  void onKillEdges();
  void onJoinEdges();

signals:

  void pointPicked(const double x, const double y);
  void partModified();

private:

  Handle(asiEngine_Model)                m_model;          //!< Data Model instance.
  vtkSmartPointer<asiUI_PDomainCallback> m_domainCallback; //!< Callback for domain operations.
  vtkSmartPointer<asiUI_PickCallback>    m_pickCallback;   //!< Pick callback.
  vtkSmartPointer<vtkTextWidget>         m_textWidget;     //!< Annotation.

};

#endif
