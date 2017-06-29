//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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

#ifndef asiVisu_MeshPrs_h
#define asiVisu_MeshPrs_h

// A-Situs includes
#include <asiVisu.h>

// A-Situs (mesh) includes
#include <asiData_TessNode.h>

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <Standard_Type.hxx>

// Qt includes
#include <QColor>

DEFINE_STANDARD_HANDLE(asiVisu_MeshPrs, asiVisu_Prs)

//! Presentation class for Mesh Node.
class asiVisu_MeshPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshPrs, asiVisu_Prs)

  // Allows to register this Presentation class in runtime.
  // Notice that the first parameter is NODE TYPE, not the Presentation one
  DEFINE_PRESENTATION_FACTORY(asiData_TessNode, Instance)

public:

  //! Pipelines of Presentation.
  enum PipelineId
  {
    Pipeline_Mesh = 1,
    Pipeline_MeshContour
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

// Visualization commands:
public:

  asiVisu_EXPORT void doShading() const;
  asiVisu_EXPORT void doWireframe() const;

  asiVisu_EXPORT void doColor(const QColor& theColor) const;
  asiVisu_EXPORT void doUnColor() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_MeshPrs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines   ();
  virtual void afterInitPipelines    ();
  virtual void beforeUpdatePipelines ()                                           const;
  virtual void afterUpdatePipelines  ()                                           const;
  virtual void highlight             (vtkRenderer*                  theRenderer,
                                      const asiVisu_PickResult&     thePickRes,
                                      const asiVisu_SelectionNature theSelNature) const;
  virtual void unHighlight           (vtkRenderer*                  theRenderer,
                                      const asiVisu_SelectionNature theSelNature) const;
  virtual void renderPipelines       (vtkRenderer*                  theRenderer)  const;
  virtual void deRenderPipelines     (vtkRenderer*                  theRenderer)  const;

};

#endif
