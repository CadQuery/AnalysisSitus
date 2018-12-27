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

#ifndef asiUI_BaseCurveCallback_h
#define asiUI_BaseCurveCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Base class for interaction callbacks to work with 3D curves.
class asiUI_BaseCurveCallback : public QObject,
                                public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  virtual void Execute(vtkObject*,
                       unsigned long,
                       void*) = 0;

protected:

  bool getPickedPointOnCurve(void*                        pCallData,
                             Handle(asiData_IVCurveNode)& curveNode,
                             gp_Pnt&                      resultPt,
                             double&                      resultParam) const;

  Handle(Geom_Curve)
    getPickedCurve(void*                        pCallData,
                   Handle(asiData_IVCurveNode)& curveNode) const;

  Handle(asiData_IVCurveNode)
    getPickedCurveNode(void* pCallData) const;

protected:

  //! Constructor accepting owning viewer as a parameter.
  //! \param[in] pViewer owning viewer.
  asiUI_BaseCurveCallback(asiUI_Viewer* pViewer);

  //! Dtor.
  ~asiUI_BaseCurveCallback();

};

#endif
