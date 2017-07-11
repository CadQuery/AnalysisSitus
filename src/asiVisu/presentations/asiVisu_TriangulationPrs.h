//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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
    Pipeline_TriangulationLinks
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

  asiVisu_EXPORT void doShading() const;
  asiVisu_EXPORT void doWireframe() const;

  asiVisu_EXPORT void doColor(const QColor& color) const;
  asiVisu_EXPORT void doUnColor() const;

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

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_TriangulationPrs(const Handle(ActAPI_INode)& N);

// Callbacks:
private:

  virtual void beforeInitPipelines   ();
  virtual void afterInitPipelines    ();
  virtual void beforeUpdatePipelines ()                                        const;
  virtual void afterUpdatePipelines  ()                                        const;
  virtual void highlight             (vtkRenderer*                  renderer,
                                      const asiVisu_PickResult&     pickRes,
                                      const asiVisu_SelectionNature selNature) const;
  virtual void unHighlight           (vtkRenderer*                  renderer,
                                      const asiVisu_SelectionNature selNature) const;
  virtual void renderPipelines       (vtkRenderer*                  renderer)  const;
  virtual void deRenderPipelines     (vtkRenderer*                  renderer)  const;

};

#endif
