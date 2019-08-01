//-----------------------------------------------------------------------------
// Created on: 02 December 2015
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
#include <asiVisu_CurveSource.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_TrimmedCurve.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_CurveSource);

//-----------------------------------------------------------------------------

asiVisu_CurveSource::asiVisu_CurveSource() : asiVisu_CurveSourceBase()
{}

//-----------------------------------------------------------------------------

asiVisu_CurveSource::~asiVisu_CurveSource()
{}

//-----------------------------------------------------------------------------

//! Initialize data source from a topological edge.
//! \param edge [in] edge to extract the spatial curve from.
//! \return true if poly data has been produced.
bool asiVisu_CurveSource::SetInputEdge(const TopoDS_Edge& edge)
{
  if ( BRep_Tool::Degenerated(edge) )
    return false;

  double f, l;
  m_curve3d = BRep_Tool::Curve(edge, f, l);
  //
  if ( m_curve3d.IsNull() )
    return false;

  // Build a trimmed curve in order to put trimming parameters of a curve
  // right into it
  Handle(Geom_TrimmedCurve) tcrv = new Geom_TrimmedCurve(m_curve3d, f, l);

  // Resolve orientation
  asiVisu_Orientation ori = VisuOri_Undefined;
  switch ( edge.Orientation() )
  {
    case TopAbs_FORWARD  : ori = VisuOri_Forward;  break;
    case TopAbs_REVERSED : ori = VisuOri_Reversed; break;
    case TopAbs_INTERNAL : ori = VisuOri_Internal; break;
    case TopAbs_EXTERNAL : ori = VisuOri_External; break;
    default: break;
  }

  // Initialize with a curve
  return this->SetInputCurve(tcrv, f, l, ori);
}

//-----------------------------------------------------------------------------

//! Initialize data source from a parametric curve.
//! \param curve [in] curve to discretize.
//! \param first [in] first parameter.
//! \param last  [in] last parameter.
//! \param ori   [in] orientation.
//! \return true if poly data has been produced.
bool asiVisu_CurveSource::SetInputCurve(const Handle(Geom_Curve)& curve,
                                        const double              first,
                                        const double              last,
                                        const asiVisu_Orientation ori)
{
  m_curve3d = curve;

  // Discretize curve.
  Handle(HRealArray) xCoords, yCoords, zCoords;
  //
  if ( !this->FillArrays(curve, first, last, xCoords, yCoords, zCoords) )
    return false;

  // Pass arrays as inputs.
  this->SetInputArrays(xCoords, yCoords, zCoords, ori);
  return true;
}

//-----------------------------------------------------------------------------

//! Initialize data source from a two-dimensional parametric curve.
//! \param curve [in] curve to discretize.
//! \param first [in] first parameter.
//! \param last  [in] last parameter.
//! \param ori   [in] orientation.
//! \return true if poly data has been produced.
bool asiVisu_CurveSource::SetInputCurve2d(const Handle(Geom2d_Curve)& curve,
                                          const double                first,
                                          const double                last,
                                          const asiVisu_Orientation   ori)
{
  // Discretize curve.
  Handle(HRealArray) xCoords, yCoords, zCoords;
  //
  if ( !this->FillArrays(curve, first, last, xCoords, yCoords, zCoords) )
    return false;

  // Pass arrays as inputs
  this->SetInputArrays(xCoords, yCoords, zCoords, ori);
  return true;
}

//-----------------------------------------------------------------------------

//! Sets the Data Source input arrays. These arrays represent a curve
//! explicitly as series of points. Of course, to have these points you need
//! to sample your curve beforehand.
//!
//! \param xCoords [in] X coordinates.
//! \param yCoords [in] Y coordinates.
//! \param zCoords [in] Z coordinates.
//! \param ori     [in] orientation.
void asiVisu_CurveSource::SetInputArrays(const Handle(HRealArray)& xCoords,
                                         const Handle(HRealArray)& yCoords,
                                         const Handle(HRealArray)& zCoords,
                                         const asiVisu_Orientation ori)
{
  if ( xCoords->Length() != zCoords->Length() )
  {
    vtkErrorMacro( << "Invalid domain: faulty number of X coordinates" );
    return;
  }

  if ( yCoords->Length() != zCoords->Length() )
  {
    vtkErrorMacro( << "Invalid domain: faulty number of Y coordinates" );
  }

  m_XCoords = xCoords;
  m_YCoords = yCoords;
  m_ZCoords = zCoords;
  m_ori     = ori;
}

//-----------------------------------------------------------------------------

//! Returns input arrays.
//! \param xCoords [out] x coordinates.
//! \param yCoords [out] y coordinates.
//! \param zCoords [out] z coordinates.
//! \param ori     [out] orientation.
void asiVisu_CurveSource::GetInputArrays(Handle(HRealArray)&  xCoords,
                                         Handle(HRealArray)&  yCoords,
                                         Handle(HRealArray)&  zCoords,
                                         asiVisu_Orientation& ori) const
{
  xCoords = m_XCoords;
  yCoords = m_YCoords;
  zCoords = m_ZCoords;
  ori     = m_ori;
}

//-----------------------------------------------------------------------------

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_CurveSource::RequestData(vtkInformation*        request,
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector*  outputVector)
{
  if ( m_XCoords.IsNull() || m_YCoords.IsNull() || m_ZCoords.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL coordinate arrays" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  // Access cell data
  vtkCellData* CD = polyOutput->GetCellData();

  // Array for orientation types
  vtkSmartPointer<vtkIntArray> typeArr = asiVisu_Utils::InitIntArray(ARRNAME_ORIENT_SCALARS);
  CD->AddArray(typeArr);

  // Pedigree IDs for selection
  vtkSmartPointer<vtkIdTypeArray> PIDsArray = vtkSmartPointer<vtkIdTypeArray>::New();
  PIDsArray->SetNumberOfComponents(1);
  CD->SetPedigreeIds(PIDsArray);

  /* ===================
   *  Populate polydata
   * =================== */

  if ( !this->FillPolydata(polyOutput) )
  {
    vtkErrorMacro( << "Cannot populate polygonal data set" );
    return 0;
  }

  /* ===================
   *  Continue pipeline
   * =================== */

  return Superclass::RequestData(request, inputVector, outputVector);
}
