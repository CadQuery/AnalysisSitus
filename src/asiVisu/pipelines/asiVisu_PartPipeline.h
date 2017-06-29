//-----------------------------------------------------------------------------
// Created on: 30 November 2016
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

#ifndef asiVisu_PartPipeline_h
#define asiVisu_PartPipeline_h

// asiVisu includes
#include <asiVisu_PartPipelineBase.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PartPipeline, asiVisu_PartPipelineBase)

//! Visualization pipeline for OCCT shapes. This pipeline serves the purpose
//! of visualization of "not-so-valid" shapes. Such shapes can occur either
//! because of any sort of corruption, or they can represent some intermediate
//! state of modeling, e.g. a result of Euler Operation which breaks geometry,
//! but keeps topology consistent.
class asiVisu_PartPipeline : public asiVisu_PartPipelineBase
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartPipeline, asiVisu_PartPipelineBase)

public:

  asiVisu_EXPORT
    asiVisu_PartPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

};

#endif
