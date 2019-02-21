//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

#ifndef asiVisu_PointsPipeline_h
#define asiVisu_PointsPipeline_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Visualization pipeline for point cloud.
class asiVisu_PointsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PointsPipeline, asiVisu_Pipeline)

public:

  //! Ctor.
  //! \param[in] allowSelection indicates whether this pipeline allows
  //!                           selection of a certain portion of data.
  asiVisu_EXPORT
    asiVisu_PointsPipeline(const bool allowSelection);

public:

  //! Sets input data for the pipeline.
  //! \param[in] DP Data Provider.
  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  //! Sets transient active point IDs.
  //! \param[in] reperId reper IDs to use.
  void SetForcedActiveRepers(const Handle(TColStd_HPackedMapOfInteger)& reperIds)
  {
    m_iForcedActiveRepers = reperIds;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_PointsPipeline(const asiVisu_PointsPipeline&);

  //! Assignment prohibited.
  asiVisu_PointsPipeline& operator=(const asiVisu_PointsPipeline&);

private:

  //! Transient active reper points.
  Handle(TColStd_HPackedMapOfInteger) m_iForcedActiveRepers;

  //! Enables/disables partial data visualization.
  bool m_bAllowSelection;

};

#endif
