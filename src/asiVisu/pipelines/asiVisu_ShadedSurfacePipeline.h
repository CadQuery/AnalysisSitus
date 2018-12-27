//-----------------------------------------------------------------------------
// Created on: 01 April 2016
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

#ifndef asiVisu_ShadedSurfacePipeline_h
#define asiVisu_ShadedSurfacePipeline_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_SurfaceSource.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for a parametric surface in shaded view.
class asiVisu_ShadedSurfacePipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShadedSurfacePipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_ShadedSurfacePipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  void SetStepsNumber (const int nbSteps)       { m_iStepsNumber = nbSteps; }
  int  GetStepsNumber ()                  const { return m_iStepsNumber; }
  void ForceUpdate    ()                        { m_bForced = true; }
  //
  bool IsCurvature         () const;
  bool IsGaussianCurvature () const;
  bool IsMeanCurvature     () const;
  void GaussianCurvatureOn ();
  void MeanCurvatureOn     ();
  void CurvatureOff        ();

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_ShadedSurfacePipeline(const asiVisu_ShadedSurfacePipeline&);

  //! Assignment prohibited.
  asiVisu_ShadedSurfacePipeline& operator=(const asiVisu_ShadedSurfacePipeline&);

protected:

  int  m_iStepsNumber; //!< Number of steps for sampling the parametric space.
  bool m_bForced;      //!< Not very graceful flag to force execution even if
                       //!< no persistent data has changed.
  //
  vtkSmartPointer<asiVisu_SurfaceSource> m_source; //!< Polygonal data source.

};

#endif
