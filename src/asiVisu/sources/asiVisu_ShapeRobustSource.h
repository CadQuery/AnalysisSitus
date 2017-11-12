//-----------------------------------------------------------------------------
// Created on: 21 March 2017
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

#ifndef asiVisu_ShapeRobustSource_h
#define asiVisu_ShapeRobustSource_h

// asiVisu includes
#include <asiVisu_ShapeData.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkInformationIdTypeKey.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkType.h>
#include <vtkSmartPointer.h>

//! Source of polygonal VTK data representing OCCT shape.
class asiVisu_ShapeRobustSource : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_ShapeRobustSource, vtkPolyDataAlgorithm);

  asiVisu_EXPORT static asiVisu_ShapeRobustSource*
    New();

public:

  asiVisu_EXPORT void
    SetAAG(const Handle(asiAlgo_AAG)& aag);

  asiVisu_EXPORT const Handle(asiAlgo_AAG)&
    GetAAG() const;

  asiVisu_EXPORT void
    SetShape(const TopoDS_Shape& shape);

  asiVisu_EXPORT const TopoDS_Shape&
    GetShape() const;

  asiVisu_EXPORT void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

  asiVisu_EXPORT void
    SetTessellationParams(const double linDefl,
                          const double angDefl);

protected:

  //! Builds output polygonal data set from the B-Rep shape.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        pInfo,
                vtkInformationVector** pInputVector,
                vtkInformationVector*  pOutputVector);

protected:

  asiVisu_EXPORT
    asiVisu_ShapeRobustSource();

  asiVisu_EXPORT
    ~asiVisu_ShapeRobustSource();

private:

  asiVisu_EXPORT
    asiVisu_ShapeRobustSource(const asiVisu_ShapeRobustSource&);

  asiVisu_EXPORT asiVisu_ShapeRobustSource&
    operator=(const asiVisu_ShapeRobustSource&);

protected:

  Handle(asiAlgo_AAG)       m_aag;               //!< (Optional) AAG of the original B-Rep shape.
  TopoDS_Shape              m_shape;             //!< Original B-Rep shape.
  Handle(asiVisu_ShapeData) m_shapeData;         //!< Polygonal data container.
  double                    m_fLinDeflection;    //!< Linear deflection.
  double                    m_fAngDeflectionDeg; //!< Angular deflection.
  ActAPI_ProgressEntry      m_progress;          //!< Progress notifier.
  ActAPI_PlotterEntry       m_plotter;           //!< Imperative plotter.

};

#endif
