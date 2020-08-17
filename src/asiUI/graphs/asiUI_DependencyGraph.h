//-----------------------------------------------------------------------------
// Created on: 11 August 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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

#ifndef asiUI_DependencyGraph_h
#define asiUI_DependencyGraph_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiUI includes
#include <asiUI_VtkWindow.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <vtkGraph.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextWidget.h>
#pragma warning(pop)

//! Visualizes dependency graph.
class asiUI_DependencyGraph : public QObject
{
  Q_OBJECT

public:

  asiUI_DependencyGraph(const Handle(asiEngine_Model)& model);

  ~asiUI_DependencyGraph();

public:

  void
    Render();

  void
    RenderEventCallback();

protected:

  vtkSmartPointer<vtkGraph>
    convertToGraph();

protected slots:

  void onVertexPicked(const vtkIdType);

protected:

  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  vtkTextWidget*          m_textWidget; //!< Text.
};

#endif
