//-----------------------------------------------------------------------------
// Created on: 20 November 2015
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

#ifndef asiVisu_ShrinkPolyFilter_h
#define asiVisu_ShrinkPolyFilter_h

// Visualization includes
#include <asiVisu.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//! Filter performing shrink operation on polygonal data set and having
//! a polygonal data set as an output as well. Unlike vtkShrinkPolyData
//! filter shipped with VTK, this one is more stable (the native one leads
//! to memory corruption crashes on VTK 5.8.0).
class asiVisu_ShrinkPolyFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_ShrinkPolyFilter, vtkPolyDataAlgorithm);
  static asiVisu_ShrinkPolyFilter* New();

  vtkSetClampMacro(ShrinkFactor, double, 0.0, 1.0);
  vtkGetMacro(ShrinkFactor, double);

protected:

  virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

protected:

  asiVisu_ShrinkPolyFilter();
  ~asiVisu_ShrinkPolyFilter();

private:

  double ShrinkFactor; //!< Shrink factor to apply.

};

#endif
