//-----------------------------------------------------------------------------
// Created on: 13 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiVisu_BSurfKnotsSource_h
#define asiVisu_BSurfKnotsSource_h

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColgp_HArray2OfPnt.hxx>

//! Data source giving a knots isos of a b-surface.
class asiVisu_BSurfKnotsSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_BSurfKnotsSource, vtkPolyDataAlgorithm);
  static asiVisu_BSurfKnotsSource* New();

// Kernel methods:
public:

  bool SetInputSurface(const Handle(Geom_BSplineSurface)& bsurf);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerPoint(const gp_Pnt& P,
                  vtkPolyData*  polyData);

  vtkIdType
    registerCurve(const Handle(Geom_BSplineCurve)& curve,
                  const int                        mult,
                  vtkPolyData*                     polyData);

protected:

  asiVisu_BSurfKnotsSource();
  ~asiVisu_BSurfKnotsSource();

private:

  asiVisu_BSurfKnotsSource(const asiVisu_BSurfKnotsSource&);
  asiVisu_BSurfKnotsSource& operator=(const asiVisu_BSurfKnotsSource&);

private:

  Handle(Geom_BSplineSurface) m_surf; //!< B-surface.

};

#endif
