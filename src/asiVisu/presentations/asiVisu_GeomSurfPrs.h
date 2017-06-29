//-----------------------------------------------------------------------------
// Created on: 04 December 2015
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

#ifndef asiVisu_GeomSurfPrs_h
#define asiVisu_GeomSurfPrs_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// A-Situs (geometry) includes
#include <asiData_PartNode.h>

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCommand.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextWidget.h>

class asiVisu_SurfBtnCallback;

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_GeomSurfPrs, asiVisu_Prs)

//! Presentation class for a host surface of a b-rep face.
class asiVisu_GeomSurfPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_GeomSurfPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_SurfNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1,
    Pipeline_Shaded
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

//-----------------------------------------------------------------------------
public:

  asiVisu_EXPORT void DoWireframe();
  asiVisu_EXPORT void DoShaded();
  asiVisu_EXPORT void DoShadedAndGaussian();
  asiVisu_EXPORT void DoShadedAndMean();
  //
  asiVisu_EXPORT bool IsShaded() const;
  asiVisu_EXPORT bool IsShadedAndScalars() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_GeomSurfPrs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const asiVisu_PickResult& thePickRes,
                         const asiVisu_SelectionNature theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const asiVisu_SelectionNature theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

private:

  vtkSmartPointer<vtkTextWidget>           m_textWidget;      //!< Annotation.
  vtkSmartPointer<vtkScalarBarWidget>      m_scalarBarWidget; //!< Scalar bar.
  vtkSmartPointer<vtkButtonWidget>         m_modeButton;      //!< Button to switch visualization modes.
  vtkSmartPointer<asiVisu_SurfBtnCallback> m_modeCallback;    //!< Callback for visualization mode button.

};

//-----------------------------------------------------------------------------

//! Callback for switching visualization mode.
class asiVisu_SurfBtnCallback : public vtkCommand
{
public:

  static asiVisu_SurfBtnCallback* New();
  vtkTypeMacro(asiVisu_SurfBtnCallback, vtkCommand);

  asiVisu_SurfBtnCallback() {}

public:

  void SetPresentation(const Handle(asiVisu_GeomSurfPrs)& prs)
  {
    m_prs = prs;
  }

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

private:

  Handle(asiVisu_GeomSurfPrs) m_prs; //!< Listened Presentation.

};

#endif
