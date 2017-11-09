//-----------------------------------------------------------------------------
// Created on: 02 December 2015
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
