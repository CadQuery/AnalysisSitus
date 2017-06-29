//-----------------------------------------------------------------------------
// Created on: 28 April 2017
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

#ifndef asiVisu_FuncLevelsPipeline3d_h
#define asiVisu_FuncLevelsPipeline3d_h

// asiVisu includes
#include <asiVisu_FuncDataProvider.h>
#include <asiVisu_FuncLevelsSource3d.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for function level sets.
template <typename T_VARIABLE>
class asiVisu_FuncLevelsPipeline3d : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncLevelsPipeline3d, asiVisu_Pipeline)

public:

  asiVisu_FuncLevelsPipeline3d() : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                                                     vtkSmartPointer<vtkActor>::New() )
  {
    this->Actor()->GetProperty()->SetPointSize(3.0);
    this->Mapper()->ScalarVisibilityOn();
  }

public:

  virtual void SetInput(const Handle(asiVisu_DataProvider)& DP)
  {
    const Handle(asiVisu_FuncDataProvider<T_VARIABLE>)&
      provider = Handle(asiVisu_FuncDataProvider<T_VARIABLE>)::DownCast(DP);
    //
    if ( provider.IsNull() )
      return;

    /* ===========================
     *  Validate input Parameters
     * =========================== */

    Handle(asiAlgo_Function<T_VARIABLE>) func = provider->GetFunction();
    //
    if ( func.IsNull() )
    {
      // Pass empty data set in order to have valid pipeline
      vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
      this->SetInputData(aDummyDS);
      this->Modified(); // Update modification timestamp
      return; // Do nothing
    }

    /* ============================
     *  Prepare polygonal data set
     * ============================ */

    if ( provider->MustExecute( this->GetMTime() ) )
    {
      vtkSmartPointer< asiVisu_FuncLevelsSource3d<T_VARIABLE> >
        src = vtkSmartPointer< asiVisu_FuncLevelsSource3d<T_VARIABLE> >::New();

      src->SetInputFunction( func,
                             provider->GetMinCorner(),
                             provider->GetMaxCorner() );

      // Initialize pipeline
      this->SetInputConnection( src->GetOutputPort() );
    }

    // Update modification timestamp
    this->Modified();
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer*) {}
  virtual void callback_remove_from_renderer (vtkRenderer*) {}
  virtual void callback_update               ()             {}

private:

  //! Copying prohibited.
  asiVisu_FuncLevelsPipeline3d(const asiVisu_FuncLevelsPipeline3d&);

  //! Assignment prohibited.
  asiVisu_FuncLevelsPipeline3d& operator=(const asiVisu_FuncLevelsPipeline3d&);

};

#endif
