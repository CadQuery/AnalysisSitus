//-----------------------------------------------------------------------------
// Created on: 02 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_CurveSource_h
#define asiVisu_CurveSource_h

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
#include <Geom2d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Edge.hxx>

//! Source of polygonal data representing a 3D curve.
class asiVisu_CurveSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_CurveSource, vtkPolyDataAlgorithm);

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

public:

  void          SetTipSize    (const double size)       { m_fOriTipSize = size; }
  double        GetTipSize    ()                  const { return m_fOriTipSize; }

  void          SetTipTangent (const gp_Vec& vec)       { m_oriT = vec; }
  const gp_Vec& GetTipTangent ()                  const { return m_oriT; }

  void          SetTipNorm    (const gp_Vec& vec)       { m_oriN = vec; }
  const gp_Vec& GetTipNorm    ()                  const { return m_oriN; }

  void          SetPedigreeId (const int pid)           { m_iPedigreeId = pid; }
  int           GetPedigreeId ()                  const { return m_iPedigreeId; }

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
    registerLine(const int    index,
                 vtkPolyData* polyData);

  asiVisu_EXPORT vtkIdType
    registerVertex(const int    index,
                   vtkPolyData* polyData);

protected:

  asiVisu_EXPORT
    asiVisu_CurveSource();

  asiVisu_EXPORT
    ~asiVisu_CurveSource();

private:

  asiVisu_CurveSource(const asiVisu_CurveSource&);
  asiVisu_CurveSource& operator=(const asiVisu_CurveSource&);

private:

  Handle(HRealArray) m_XCoords;     //!< X coordinates.
  Handle(HRealArray) m_YCoords;     //!< Y coordinates.
  Handle(HRealArray) m_ZCoords;     //!< Z coordinates.
  int                m_iPedigreeId; //!< Pedigree ID.

// Orientation marker:
private:

  asiVisu_Orientation m_ori;         //!< Curve orientation.
  double              m_fOriTipSize; //!< Size of orientation tip (calculated externally).
  gp_Vec              m_oriT;        //!< Orientation vector at the end point.
  gp_Vec              m_oriN;        //!< Normal to the curve at the end point.

};

#endif
