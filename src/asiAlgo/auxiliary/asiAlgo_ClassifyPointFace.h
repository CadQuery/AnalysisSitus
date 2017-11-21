//-----------------------------------------------------------------------------
// Created on: 21 April 2016
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

#ifndef asiAlgo_ClassifyPointFace_h
#define asiAlgo_ClassifyPointFace_h

// Geometry includes
#include <asiAlgo_Classifier.h>

// OCCT includes
#include <IntTools_FClass2d.hxx>
#include <TopoDS_Face.hxx>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

//! Point-Face classifier.
class asiAlgo_ClassifyPointFace
{
public:

  asiAlgo_EXPORT
    asiAlgo_ClassifyPointFace(const TopoDS_Face&   F,
                              const double         inaccuracy,
                              const double         precision,
                              ActAPI_ProgressEntry progress = NULL,
                              ActAPI_PlotterEntry  plotter = NULL);

public:

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt2d& PonS);

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt& P,
               const bool    checkGap);

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt& P,
               const bool    checkGap,
               gp_Pnt2d&     UV);

protected:

  TopoDS_Face       m_F;           //!< Working face.
  double            m_fPrecision;  //!< Precision of optimization method.
  double            m_fInaccuracy; //!< Inaccuracy in the input geometry.
  IntTools_FClass2d m_fclass;      //!< Internal classifier.
  double            m_fUmin;       //!< U min parametric boundary of the face.
  double            m_fUmax;       //!< U max parametric boundary of the face.
  double            m_fVmin;       //!< V min parametric boundary of the face.
  double            m_fVmax;       //!< V max parametric boundary of the face.
  //
  ActAPI_ProgressEntry m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
