//-----------------------------------------------------------------------------
// Created on: 02 September 2016
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

#ifndef asiVisu_BCurveKnotsSource_h
#define asiVisu_BCurveKnotsSource_h

// Visualization includes
#include <asiVisu_Utils.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//! Data source giving a visual feedback on the distribution of knots
//! in a b-curve.
class asiVisu_BCurveKnotsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_BCurveKnotsSource, vtkPolyDataAlgorithm);
  static asiVisu_BCurveKnotsSource* New();

// Kernel methods:
public:

  bool SetInputCurve(const Handle(Geom_BSplineCurve)& bcurve);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  vtkIdType
    registerVertex(const vtkIdType pid,
                   vtkPolyData*    polyData);

protected:

  asiVisu_BCurveKnotsSource();
  ~asiVisu_BCurveKnotsSource();

private:

  asiVisu_BCurveKnotsSource(const asiVisu_BCurveKnotsSource&);
  asiVisu_BCurveKnotsSource& operator=(const asiVisu_BCurveKnotsSource&);

private:

  Handle(Geom_BSplineCurve) m_curve; //!< B-curve as a data container.

};

#endif
