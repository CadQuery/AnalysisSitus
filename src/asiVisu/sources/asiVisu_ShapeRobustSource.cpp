//-----------------------------------------------------------------------------
// Created on: 03 March 2017
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

// Own include
#include <asiVisu_ShapeRobustSource.h>

// VTK includes
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

vtkStandardNewMacro(asiVisu_ShapeRobustSource);

//-----------------------------------------------------------------------------

asiVisu_ShapeRobustSource::asiVisu_ShapeRobustSource()
: vtkPolyDataAlgorithm(),
  m_fLinDeflection(0.0), // Zeroes lead to automatic selection of faceter parameters
  m_fAngDeflectionDeg(0.0)
{
  this->SetNumberOfInputPorts(0); // Input is passed from application data model
}

//-----------------------------------------------------------------------------

asiVisu_ShapeRobustSource::~asiVisu_ShapeRobustSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::SetAAG(const Handle(asiAlgo_AAG)& aag)
{
  m_aag   = aag;
  m_shape = m_aag->GetMasterCAD();
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

const Handle(asiAlgo_AAG)& asiVisu_ShapeRobustSource::GetAAG() const
{
  return m_aag;
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::SetColorSource(const Handle(asiVisu_ShapeColorSource)& colorSource)
{
  m_colorSource = colorSource;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

//! \return shape color source.
const Handle(asiVisu_ShapeColorSource)&
  asiVisu_ShapeRobustSource::GetColorSource() const
{
  return m_colorSource;
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::SetShape(const TopoDS_Shape& shape)
{
  m_aag.Nullify();
  m_shape = shape;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

const TopoDS_Shape& asiVisu_ShapeRobustSource::GetShape() const
{
  return m_shape;
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                                   ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::SetTessellationParams(const double linDefl,
                                                      const double angDefl)
{
  m_fLinDeflection    = linDefl;
  m_fAngDeflectionDeg = angDefl;

  // Mark as modified. Such marking will lead to invocation of RequestData()
  // method once the pipeline is updated with Update() method on any of
  // successor stages.
  this->Modified();
}

//-----------------------------------------------------------------------------

int asiVisu_ShapeRobustSource::GetLastUnusedScalar() const
{
  if ( !m_tessellator.Get() )
    return 0;

  return m_tessellator->GetLastUnusedScalar();
}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::GetExtraColorsScalars(NCollection_DataMap<int, int>& extraScalars) const
{
  if ( !m_tessellator.Get() )
    return;

  m_tessellator->GetExtraColorsScalars(extraScalars);
}

//-----------------------------------------------------------------------------

int asiVisu_ShapeRobustSource::RequestData(vtkInformation*        pInfo,
                                           vtkInformationVector** pInputVector,
                                           vtkInformationVector*  pOutputVector)
{
  vtkPolyData* pOutputPolyData = vtkPolyData::GetData(pOutputVector);
  pOutputPolyData->Allocate();
  //
  vtkSmartPointer<vtkPoints> pOutputPts = vtkSmartPointer<vtkPoints>::New();
  pOutputPolyData->SetPoints(pOutputPts);

  /* =============
   *  Run faceter
   * ============= */

  m_shapeData = new asiVisu_ShapeData;
  //
  m_tessellator = vtkSmartPointer<asiVisu_ShapeRobustTessellator>::New();
  //
  if ( m_aag.IsNull() )
    m_tessellator->Initialize(m_shape,
                              m_fLinDeflection,
                              m_fAngDeflectionDeg,
                              m_colorSource,
                              m_progress,
                              m_plotter);
  else
    m_tessellator->Initialize(m_aag,
                              m_fLinDeflection,
                              m_fAngDeflectionDeg,
                              m_colorSource,
                              m_progress,
                              m_plotter);
  m_tessellator->Build();
  //
  const Handle(asiVisu_ShapeData)&    tessResult         = m_tessellator->GetResult();
  const vtkSmartPointer<vtkPolyData>& tessResultPolyData = tessResult->GetPolyData();

  /* ==========
   *  Finalize
   * ========== */

  pOutputPolyData->CopyStructure(tessResultPolyData);  // Copy points and cells
  pOutputPolyData->CopyAttributes(tessResultPolyData); // Copy data arrays

  return vtkPolyDataAlgorithm::RequestData(pInfo, pInputVector, pOutputVector);
}
