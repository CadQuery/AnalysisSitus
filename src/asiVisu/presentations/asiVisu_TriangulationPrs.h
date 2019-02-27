//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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

#ifndef asiVisu_TriangulationPrs_h
#define asiVisu_TriangulationPrs_h

// asiVisu includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_TriangulationNode.h>

// OCCT includes
#include <Standard_Type.hxx>

// Qt includes
#include <QColor>

//! Presentation class for Triangulation Node.
class asiVisu_TriangulationPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_TriangulationPrs, asiVisu_Prs)

  // Allows to register this Presentation class in runtime.
  // Notice that the first parameter is NODE TYPE, not the Presentation one
  DEFINE_PRESENTATION_FACTORY(asiData_TriangulationNode, Instance)

public:

  //! Pipelines of Presentation.
  enum PipelineId
  {
    Pipeline_Triangulation = 1,
    Pipeline_TriangulationLinks,
    Pipeline_TriangulationNodes
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

  asiVisu_EXPORT void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

// Visualization commands:
public:

  asiVisu_EXPORT void
    VerticesOn() const;

  asiVisu_EXPORT void
    VerticesOff() const;

  asiVisu_EXPORT void
    Colorize(const QColor& color) const;

public:

  asiVisu_EXPORT virtual void
    InitializePicker(const vtkSmartPointer<vtkCellPicker>& picker) const;

public:

  vtkActor* MainActor() const
  {
    return this->GetPipeline(Pipeline_Triangulation).IsNull() ? NULL : this->GetPipeline(Pipeline_Triangulation)->Actor();
  }

  vtkActor* ContourActor() const
  {
    return this->GetPipeline(Pipeline_TriangulationLinks).IsNull() ? NULL : this->GetPipeline(Pipeline_TriangulationLinks)->Actor();
  }

  vtkActor* NodesActor() const
  {
    return this->GetPipeline(Pipeline_TriangulationNodes).IsNull() ? NULL : this->GetPipeline(Pipeline_TriangulationNodes)->Actor();
  }

private:

  //! Allocation is allowed only via Instance() method.
  asiVisu_TriangulationPrs(const Handle(ActAPI_INode)& N);

// Callbacks:
private:

  virtual void beforeInitPipelines   ();
  virtual void afterInitPipelines    ();
  virtual void beforeUpdatePipelines ()                                        const;
  virtual void afterUpdatePipelines  ()                                        const;
  virtual void highlight             (vtkRenderer*                        renderer,
                                      const Handle(asiVisu_PickerResult)& pickRes,
                                      const asiVisu_SelectionNature       selNature) const;
  virtual void unHighlight           (vtkRenderer*                        renderer,
                                      const asiVisu_SelectionNature       selNature) const;
  virtual void renderPipelines       (vtkRenderer*                        renderer)  const;
  virtual void deRenderPipelines     (vtkRenderer*                        renderer)  const;

};

#endif
