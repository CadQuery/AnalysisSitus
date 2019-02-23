//-----------------------------------------------------------------------------
// Created on: 28 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
