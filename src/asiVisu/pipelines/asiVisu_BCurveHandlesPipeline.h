//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiVisu_BCurveHandlesPipeline_h
#define asiVisu_BCurveHandlesPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for handles on a b-curve.
class asiVisu_BCurveHandlesPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_BCurveHandlesPipeline, asiVisu_Pipeline)

public:

  //! Ctor.
  asiVisu_EXPORT
    asiVisu_BCurveHandlesPipeline();

public:

  //! Sets input data for the pipeline.
  //! \param[in] DP Data Provider.
  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  //! Sets transient active handle.
  //! \param[in] handleId handle ID to use.
  void SetForcedActiveHandle(const int handleId)
  {
    m_iForcedActiveHandle = handleId;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer*) {}
  virtual void callback_remove_from_renderer (vtkRenderer*) {}
  virtual void callback_update               ()             {}

private:

  //! Copying prohibited.
  asiVisu_BCurveHandlesPipeline(const asiVisu_BCurveHandlesPipeline&);

  //! Assignment prohibited.
  asiVisu_BCurveHandlesPipeline& operator=(const asiVisu_BCurveHandlesPipeline&);

private:

  int m_iForcedActiveHandle; //!< Transient active handle.

};

#endif
