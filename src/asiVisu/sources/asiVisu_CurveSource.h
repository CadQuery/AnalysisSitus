//-----------------------------------------------------------------------------
// Created on: 02 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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

#ifndef asiVisu_CurveSource_h
#define asiVisu_CurveSource_h

// asiVisu includes
#include <asiVisu_CurveSourceBase.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Edge.hxx>

//! Source of polygonal data representing a 3D curve.
class asiVisu_CurveSource : public asiVisu_CurveSourceBase
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_CurveSource, asiVisu_CurveSourceBase);

  asiVisu_EXPORT static asiVisu_CurveSource*
    New();

// Kernel methods:
public:

  asiVisu_EXPORT bool
    SetInputEdge(const TopoDS_Edge& edge);

  asiVisu_EXPORT bool
    SetInputCurve(const Handle(Geom_Curve)& curve,
                  const double              first,
                  const double              last,
                  const asiVisu_Orientation ori = VisuOri_Internal);

  asiVisu_EXPORT bool
    SetInputCurve2d(const Handle(Geom2d_Curve)& curve,
                    const double                first,
                    const double                last,
                    const asiVisu_Orientation   ori = VisuOri_Internal);

  asiVisu_EXPORT void
    SetInputArrays(const Handle(HRealArray)& xCoords,
                   const Handle(HRealArray)& yCoords,
                   const Handle(HRealArray)& zCoords,
                   const asiVisu_Orientation ori = VisuOri_Undefined);

  asiVisu_EXPORT void
    GetInputArrays(Handle(HRealArray)&  xCoords,
                   Handle(HRealArray)&  yCoords,
                   Handle(HRealArray)&  zCoords,
                   asiVisu_Orientation& ori) const;

protected:

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        request,
                vtkInformationVector** inputVector,
                vtkInformationVector*  outputVector);

protected:

  //! Default ctor.
  asiVisu_EXPORT
    asiVisu_CurveSource();

  //! Dtor.
  asiVisu_EXPORT
    ~asiVisu_CurveSource();

private:

  asiVisu_CurveSource(const asiVisu_CurveSource&) = delete;
  asiVisu_CurveSource& operator=(const asiVisu_CurveSource&) = delete;

};

#endif
