//-----------------------------------------------------------------------------
// Created on: 21 July 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiVisu_ReCoedgeSource_h
#define asiVisu_ReCoedgeSource_h

// asiVisu includes
#include <asiVisu_CurveSourceBase.h>

// OCCT includes
#include <Geom_Surface.hxx>

//! Visualization source for markers representing coedges.
class asiVisu_ReCoedgeSource : public asiVisu_CurveSourceBase
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_ReCoedgeSource, asiVisu_CurveSourceBase);

  asiVisu_EXPORT static asiVisu_ReCoedgeSource*
    New();

public:

  //! Initializes data source with a parametric curve.
  //! \param[in] curve edge curve.
  //! \return true in case of success, false -- otherwise.
  asiVisu_EXPORT bool
    SetCurve(const Handle(Geom_Curve)& curve);

  //! Initializes data source with a parametric surface.
  //! \param[in] surf host surface.
  //! \return true in case of success, false -- otherwise.
  asiVisu_EXPORT bool
    SetSurface(const Handle(Geom_Surface)& surface);

  //! Sets orientation flag.
  //! \param[in] samesense value to set.
  //! \return true in case of success, false -- otherwise.
  asiVisu_EXPORT bool
    SetSameSense(const bool samesense);

protected:

  //! This method (called by superclass) performs conversion of OCCT
  //! data structures to VTK polygonal representation.
  //!
  //! \param[in]  request      describes "what" algorithm should do. This is
  //!                          typically just one key such as REQUEST_INFORMATION.
  //! \param[in]  inputVector  inputs of the algorithm.
  //! \param[out] outputVector outputs of the algorithm.
  //! \return execution status.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

protected:

  //! Default ctor.
  asiVisu_EXPORT
    asiVisu_ReCoedgeSource();

  //! Dtor.
  asiVisu_EXPORT
    ~asiVisu_ReCoedgeSource();

private:

  asiVisu_ReCoedgeSource(const asiVisu_ReCoedgeSource&) = delete;
  asiVisu_ReCoedgeSource& operator=(const asiVisu_ReCoedgeSource&) = delete;

protected:

  Handle(Geom_Surface) m_surf;       //!< Host surface.
  bool                 m_bSameSense; //!< Orientation flag.

};

#endif
