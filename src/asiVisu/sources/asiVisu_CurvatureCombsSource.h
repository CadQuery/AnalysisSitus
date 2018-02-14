//-----------------------------------------------------------------------------
// Created on: 16 January 2018
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

#ifndef asiVisu_CurvatureCombsSource_h
#define asiVisu_CurvatureCombsSource_h

// Visualization includes
#include <asiVisu_Utils.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_Curve.hxx>

//! Source of polygonal data representing curvature combs. A curvature comb
//! is a geometric primitive attached to a parametric curve to visualize
//! its continuity and smoothness. Alternatively, you may want to check a
//! curvature plot which is an algebraic function defined in a parametric
//! range of a curve. Both these approaches to study the curve fairness
//! have their pros and cons. For details, refer to the following paper by
//! Gerald Farin:
//!
//! "Farin, G. 2016. Curvature combs and curvature plots. CAD Computer Aided Design 80, 6-8."
class asiVisu_CurvatureCombsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_CurvatureCombsSource, vtkPolyDataAlgorithm);

  asiVisu_EXPORT static asiVisu_CurvatureCombsSource*
    New();

// Kernel methods:
public:

  asiVisu_EXPORT bool
    SetInputCurve(const Handle(Geom_Curve)& curve,
                  const double              first,
                  const double              last);

  asiVisu_EXPORT void
    SetScaleFactor(const double scaleFactor);

  asiVisu_EXPORT void
    SetCurvatureField(const std::vector<gp_Pnt>& points,
                      const std::vector<double>& params,
                      const std::vector<double>& curvatures,
                      const std::vector<gp_Vec>& combs);

protected:

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

protected:

  asiVisu_EXPORT vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  asiVisu_EXPORT vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  asiVisu_EXPORT vtkIdType
    registerLine(const gp_Pnt& pointStart,
                 const gp_Pnt& pointEnd,
                 vtkPolyData*  polyData);


  asiVisu_EXPORT vtkIdType
    registerVertex(const gp_Pnt& point,
                   vtkPolyData*  polyData);

protected:

  asiVisu_EXPORT
    asiVisu_CurvatureCombsSource();

  asiVisu_EXPORT
    ~asiVisu_CurvatureCombsSource();

private:

  asiVisu_CurvatureCombsSource(const asiVisu_CurvatureCombsSource&);
  asiVisu_CurvatureCombsSource& operator=(const asiVisu_CurvatureCombsSource&);

private:

  double               m_fScale;     //!< Scale factor.
  Handle(Geom_Curve)   m_curve;      //!< Curve to analyze.
  double               m_fFirst;     //!< Leading parameter on a curve.
  double               m_fLast;      //!< Trailing parameter on a curve.
  std::vector<gp_Pnt>  m_points;     //!< Discretization points.
  std::vector<double>  m_params;     //!< Parameters at discretization points.
  std::vector<double>  m_curvatures; //!< Curvature values at discretization points.
  std::vector<gp_Vec>  m_combs;      //!< Curvature combs at discretization points.

};

#endif
