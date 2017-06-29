//-----------------------------------------------------------------------------
// Created on: 21 July 2016
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

#ifndef asiVisu_IVTextItemPrs_h
#define asiVisu_IVTextItemPrs_h

// A-Situs (visualization) includes
#include <asiVisu_IVPrs.h>

// A-Situs (visualization) includes
#include <asiData_IVTextItemNode.h>

// VTK includes
#include <vtkTextWidget.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVTextItemPrs, asiVisu_IVPrs)

//! Presentation class for a single text label in IV.
class asiVisu_IVTextItemPrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVTextItemPrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVTextItemNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void afterInitPipelines ();
  virtual void renderPipelines    (vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines  (vtkRenderer* theRenderer) const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_IVTextItemPrs(const Handle(ActAPI_INode)& theNode);

private:

  vtkSmartPointer<vtkTextWidget> m_textWidget; //!< Annotation.

};

#endif
