//-----------------------------------------------------------------------------
// Created on: 03 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiVisu_ThicknessPrs_h
#define asiVisu_ThicknessPrs_h

// asiVisu includes
#include <asiVisu_DefaultPrs.h>

// asiData includes
#include <asiData_ThicknessNode.h>

// VTK includes
#include <vtkScalarBarWidget.h>

//-----------------------------------------------------------------------------

//! Presentation class for Thickness Node.
class asiVisu_ThicknessPrs : public asiVisu_DefaultPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ThicknessPrs, asiVisu_DefaultPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_ThicknessNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  //! Factory method for Presentation.
  //! \param[in] N Node to create a Presentation for.
  //! \return new Presentation instance.
  static Handle(asiVisu_Prs) Instance(const Handle(ActAPI_INode)& N)
  {
    return new asiVisu_ThicknessPrs(N);
  }

private:

  //! Creates a Presentation object for the passed Node.
  //! Allocation is allowed only via Instance() method.
  //! \param[in] N Node to create a Presentation for.
  asiVisu_EXPORT asiVisu_ThicknessPrs(const Handle(ActAPI_INode)& N);

// Callbacks:
private:

  //! Callback for updating of Presentation pipelines invoked after the
  //! kernel update routine completes.
  virtual void afterUpdatePipelines() const;

  //! Callback for rendering.
  //! \param[in] renderer renderer.
  virtual void renderPipelines(vtkRenderer* renderer) const;

  //! Callback for derendering.
  virtual void deRenderPipelines(vtkRenderer* renderer) const;

private:

  vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget; //!< Scalar bar.

};

#endif
