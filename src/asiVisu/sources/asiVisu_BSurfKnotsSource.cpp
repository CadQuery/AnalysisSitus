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

// Own include
#include <asiVisu_BSurfKnotsSource.h>

// asiVisu includes
#include <asiVisu_GeomUtils.h>

// OCCT includes
#include <GeomAdaptor_Curve.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkIdTypeArray.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_BSurfKnotsSource);

//-----------------------------------------------------------------------------

//! Default constructor.
asiVisu_BSurfKnotsSource::asiVisu_BSurfKnotsSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//-----------------------------------------------------------------------------

//! Destructor.
asiVisu_BSurfKnotsSource::~asiVisu_BSurfKnotsSource()
{}

//-----------------------------------------------------------------------------

//! Initialize data source from a b-surface.
//! \param[in] bsurf B-spline surface to extract a knots isos from.
//! \return true if poly data has been produced.
bool asiVisu_BSurfKnotsSource::SetInputSurface(const Handle(Geom_BSplineSurface)& bsurf)
{
  if ( bsurf.IsNull() )
    return false;

  m_surf = bsurf;
  this->Modified();
  return true;
}

//-----------------------------------------------------------------------------

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param[in]  request      describes "what" algorithm should do. This is
//!                          typically just one key such as REQUEST_INFORMATION.
//! \param[in]  inputVector  inputs of the algorithm.
//! \param[out] outputVector outputs of the algorithm.
int asiVisu_BSurfKnotsSource::RequestData(vtkInformation*        request,
                                          vtkInformationVector** inputVector,
                                          vtkInformationVector*  outputVector)
{
  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  // Add array for scalars.
  vtkCellData* cellData = polyOutput->GetCellData();
  //
  vtkSmartPointer<vtkIntArray> typeArr = asiVisu_Utils::InitIntArray(ARRNAME_ISOSMULTS_SCALARS);
  cellData->AddArray(typeArr);

  /* =======================
   *  Populate VTK data set
   * ======================= */

  const TColStd_Array1OfReal&    uKnots = m_surf->UKnots();
  const TColStd_Array1OfInteger& uMults = m_surf->UMultiplicities();
  const TColStd_Array1OfReal&    vKnots = m_surf->VKnots();
  const TColStd_Array1OfInteger& vMults = m_surf->VMultiplicities();

  // Register U-isos.
  for ( int k = uKnots.Lower(); k <= uKnots.Upper(); ++k )
  {
    // We use try-catch because OpenCascade likes to crash even on such basic stuff...
    Handle(Geom_BSplineCurve) uIso;
    //
    try
    {
      uIso = Handle(Geom_BSplineCurve)::DownCast( m_surf->UIso( uKnots(k) ) );
    }
    catch ( ... )
    {
      std::cout << "Crash on constructing u-isoline" << std::endl;
    }
    //
    if ( uIso.IsNull() )
      continue;

    // Register curve.
    this->registerCurve(uIso, uMults(k), polyOutput);
  }

  // Register V-isos.
  for ( int k = vKnots.Lower(); k <= vKnots.Upper(); ++k )
  {
    // We use try-catch because OpenCascade likes to crash even on such basic stuff...
    Handle(Geom_BSplineCurve) vIso;
    //
    try
    {
      vIso = Handle(Geom_BSplineCurve)::DownCast( m_surf->VIso( vKnots(k) ) );
    }
    catch ( ... )
    {
      std::cout << "Crash on constructing v-isoline" << std::endl;
    }
    //
    if ( vIso.IsNull() )
      continue;

    // Register curve.
    this->registerCurve(vIso, vMults(k), polyOutput);
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

//! Adds the given point to the passed polygonal data set.
//! \param[in]     P        point to add.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_BSurfKnotsSource::registerPoint(const gp_Pnt& P,
                                                  vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType pid = points->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

vtkIdType
  asiVisu_BSurfKnotsSource::registerCurve(const Handle(Geom_BSplineCurve)& curve,
                                          const int                        mult,
                                          vtkPolyData*                     polyData)
{
  const double f = asiVisu_Utils::TrimInf( curve->FirstParameter() ),
               l = asiVisu_Utils::TrimInf( curve->LastParameter() );

  asiVisu_IsosCont cont = VisuIsosCont_Undefined;
  //
  const int contNum = curve->Degree() - mult;
  //
  if ( contNum <= 0 )
    cont = VisuIsosCont_C0;
  else if ( contNum == 1 )
    cont = VisuIsosCont_C1;
  else if ( contNum == 2 )
    cont = VisuIsosCont_C2;
  else
    cont = VisuIsosCont_CN;

  // Discretize curve.
  GeomAdaptor_Curve gac(curve, f, l);
  //
  std::vector<gp_Pnt> curvePts;
  asiVisu_GeomUtils::DiscretizeCurve(gac, 100, curvePts);

  const int nPts = int ( curvePts.size() );
  if ( !nPts )
    return VTK_BAD_ID;

  // Register points.
  std::vector<vtkIdType> pids;
  for ( int k = 0; k < nPts; ++k )
  {
    pids.push_back( this->registerPoint(curvePts[k], polyData) );
  }

  // Add cell.
  vtkIdType cellID =
    polyData->InsertNextCell( VTK_POLY_LINE, (int) pids.size(), &pids[0] );

  // Set element type.
  vtkIntArray*
    typeArr = vtkIntArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_ISOSMULTS_SCALARS) );
  //
  typeArr->InsertNextValue(cont);

  return cellID;
}
