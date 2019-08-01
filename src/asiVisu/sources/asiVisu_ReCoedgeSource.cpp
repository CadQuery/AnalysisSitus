//-----------------------------------------------------------------------------
// Created on: 21 July 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiVisu_ReCoedgeSource.h>

// asiAlgo includes
#include <asiAlgo_PatchJointAdaptor.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <ShapeAnalysis_Surface.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_ReCoedgeSource);

//-----------------------------------------------------------------------------

asiVisu_ReCoedgeSource::asiVisu_ReCoedgeSource()
: asiVisu_CurveSourceBase (),
  m_bSameSense            (true)
{}

//-----------------------------------------------------------------------------

asiVisu_ReCoedgeSource::~asiVisu_ReCoedgeSource()
{}

//-----------------------------------------------------------------------------

bool asiVisu_ReCoedgeSource::SetCurve(const Handle(Geom_Curve)& curve)
{
  m_curve3d = curve;

  // Update modification time for the source.
  this->Modified();
  return true;
}

//-----------------------------------------------------------------------------

bool asiVisu_ReCoedgeSource::SetSurface(const Handle(Geom_Surface)& surf)
{
  m_surf = surf;

  // Update modification time for the source.
  this->Modified();
  return true;
}

//-----------------------------------------------------------------------------

bool asiVisu_ReCoedgeSource::SetSameSense(const bool samesense)
{
  m_bSameSense = samesense;
  m_ori        = m_bSameSense ? VisuOri_Forward : VisuOri_Reversed;

  // Update modification time for the source.
  this->Modified();
  return true;
}

//-----------------------------------------------------------------------------

int asiVisu_ReCoedgeSource::RequestData(vtkInformation*        request,
                                        vtkInformationVector** inputVector,
                                        vtkInformationVector*  outputVector)
{
  if ( m_curve3d.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL curve" );
    return 0;
  }

  if ( m_surf.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL surface" );
    return false;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  // Access cell data.
  vtkCellData* cellData = polyOutput->GetCellData();

  // Array for element types.
  vtkSmartPointer<vtkIntArray> typeArr = asiVisu_Utils::InitIntArray(ARRNAME_ORIENT_SCALARS);
  cellData->AddArray(typeArr);

  /* ======================
   *  Build polygonal data
   * ====================== */

  // Analyze basic local properties at coedge.
  bool isSurfGoesU, isLeftBound;
  double uMin, uMax, vMin, vMax;
  //
  if ( !asiAlgo_PatchJointAdaptor::AnalyzeJoint(m_curve3d, m_surf,
                                                isSurfGoesU, isLeftBound,
                                                uMin, uMax, vMin, vMax) )
  {
    vtkErrorMacro( << "Failed to analyze joint" );
    return false;
  }

  // Apply small shift to have a nice visualization margin.
  const double uDelta = (uMax - uMin)*0.1;
  const double vDelta = (vMax - vMin)*0.1;
  const double delta  = isSurfGoesU ? vDelta : uDelta;

  // Get the corresponding isoline.
  Handle(Geom_Curve)
    iso = ( isSurfGoesU ? m_surf->VIso(isLeftBound ? vMin + delta : vMax - delta)
                        : m_surf->UIso(isLeftBound ? uMin + delta : uMax - delta) );

  // Fill array of coordinates.
  if ( !this->FillArrays(iso, iso->FirstParameter() + delta, iso->LastParameter() - delta,
                         m_XCoords, m_YCoords, m_ZCoords) )
  {
    vtkErrorMacro( << "Cannot discretize isoparametric curve" );
    return false;
  }

  // Fill polydata.
  if ( !this->FillPolydata(polyOutput) )
  {
    vtkErrorMacro( << "Cannot fill polydata" );
    return false;
  }

  /* ====================
   *  Continue execution
   * ==================== */

  return Superclass::RequestData(request, inputVector, outputVector);
}
