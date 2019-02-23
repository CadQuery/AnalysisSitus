//-----------------------------------------------------------------------------
// Created on: 15 December 2015
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

#ifndef asiVisu_BCurvePolesSource_h
#define asiVisu_BCurvePolesSource_h

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
#include <Geom2d_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//! Data source giving a control polygon of a b-curve.
class asiVisu_BCurvePolesSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_BCurvePolesSource, vtkPolyDataAlgorithm);
  static asiVisu_BCurvePolesSource* New();

// Kernel methods:
public:

  bool SetInputCurve   (const Handle(Geom_BSplineCurve)& bcurve);
  bool SetInputCurve2d (const Handle(Geom2d_BSplineCurve)& bcurve);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  vtkIdType
    registerLine(const vtkIdType pid0,
                 const vtkIdType pid1,
                 vtkPolyData*    polyData);

  vtkIdType
    registerVertex(const vtkIdType pid,
                   vtkPolyData*    polyData);

protected:

  asiVisu_BCurvePolesSource();
  ~asiVisu_BCurvePolesSource();

private:

  asiVisu_BCurvePolesSource(const asiVisu_BCurvePolesSource&);
  asiVisu_BCurvePolesSource& operator=(const asiVisu_BCurvePolesSource&);

private:

  Handle(TColgp_HArray1OfPnt) m_poles; //!< Poles to visualize.

};

#endif
