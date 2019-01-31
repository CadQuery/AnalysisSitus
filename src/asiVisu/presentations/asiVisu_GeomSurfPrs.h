//-----------------------------------------------------------------------------
// Created on: 04 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiVisu_GeomSurfPrs_h
#define asiVisu_GeomSurfPrs_h

// asiVisu includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_PartNode.h>

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCommand.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextWidget.h>

class asiVisu_SurfBtnCallback;

//-----------------------------------------------------------------------------

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
    Pipeline_Shaded,
    Pipeline_BPoles,     // For b-spline surfaces only.
    Pipeline_BKnotsIsos, // For b-spline surfaces only.
    Pipeline_Axes
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

//-----------------------------------------------------------------------------
public:

  asiVisu_EXPORT void SetColor(const QColor& color) const override;
  //
  asiVisu_EXPORT void DoWireframe();
  asiVisu_EXPORT void DoShaded();
  asiVisu_EXPORT void DoShadedAndGaussian();
  asiVisu_EXPORT void DoShadedAndMean();
  //
  asiVisu_EXPORT bool IsShaded() const;
  asiVisu_EXPORT bool IsShadedAndScalars() const;

public:

  bool IsColorizable() const
  {
    return true;
  }

private:

  //! Allocation is allowed only via Instance() method.
  asiVisu_GeomSurfPrs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const Handle(asiVisu_PickerResult)& thePickRes,
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

  //! Working Part Node.
  Handle(asiData_PartNode) m_partNode;

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
