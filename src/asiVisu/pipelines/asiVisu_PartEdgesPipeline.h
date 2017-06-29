//-----------------------------------------------------------------------------
// Created on: 06 April 2017
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

#ifndef asiVisu_PartEdgesPipeline_h
#define asiVisu_PartEdgesPipeline_h

// asiVisu includes
#include <asiVisu_PartPipelineBase.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PartEdgesPipeline, asiVisu_PartPipelineBase)

//! Visualization pipeline for wireframe of OCCT shape. This pipeline is
//! secondary: it should be feeded with a source of a main shape pipeline.
class asiVisu_PartEdgesPipeline : public asiVisu_PartPipelineBase
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartEdgesPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_PartEdgesPipeline(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source);

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

};

#endif
