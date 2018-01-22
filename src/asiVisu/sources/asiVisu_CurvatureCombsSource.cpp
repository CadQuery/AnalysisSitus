//-----------------------------------------------------------------------------
// Created on: 16 January 2018
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

// Own include
#include <asiVisu_CurvatureCombsSource.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GeomAdaptor_Curve.hxx>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_CurvatureCombsSource);

//-----------------------------------------------------------------------------

//! Default constructor.
asiVisu_CurvatureCombsSource::asiVisu_CurvatureCombsSource()
  : vtkPolyDataAlgorithm (),
    m_iNumPoints         (10), // Default value.
    m_fFirst             (0.0),
    m_fLast              (0.0)
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//-----------------------------------------------------------------------------

//! Destructor.
asiVisu_CurvatureCombsSource::~asiVisu_CurvatureCombsSource()
{}

//-----------------------------------------------------------------------------

//! Initialize data source from a parametric curve.
//! \param[in] curve curve to discretize.
//! \param[in] first first parameter.
//! \param[in] last  last parameter.
//! \return true if poly data has been produced.
bool asiVisu_CurvatureCombsSource::SetInputCurve(const Handle(Geom_Curve)& curve,
                                                 const double              first,
                                                 const double              last)
{
  m_curve  = curve;
  m_fFirst = asiVisu_Utils::TrimInf(first),
  m_fLast  = asiVisu_Utils::TrimInf(last);

  // Update modification time for the source
  this->Modified();
  return true;
}

//-----------------------------------------------------------------------------

//! Sets the number of discretization points for a curvature comb.
//! \param[in] numPts number of discretization points to use.
void asiVisu_CurvatureCombsSource::SetNumOfPoints(const int numPts)
{
  m_iNumPoints = numPts;

  // Update modification time for the source
  this->Modified();
}

//-----------------------------------------------------------------------------

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param[in]  request      describes "what" algorithm should do. This is
//!                          typically just one key such as REQUEST_INFORMATION.
//! \param[in]  inputVector  inputs of the algorithm.
//! \param[out] outputVector outputs of the algorithm.
int asiVisu_CurvatureCombsSource::RequestData(vtkInformation*        request,
                                              vtkInformationVector** inputVector,
                                              vtkInformationVector*  outputVector)
{
  if ( m_curve.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL curve" );
    return 0;
  }

  // Discretize with a uniform curvilinear step
  GeomAdaptor_Curve gac(m_curve, m_fFirst, m_fLast);
  GCPnts_QuasiUniformAbscissa Defl(gac, m_iNumPoints);
  //
  if ( !Defl.IsDone() )
  {
    vtkErrorMacro( << "Cannot discretize curve" );
    return false;
  }

  // Allocate arrays
  Handle(HRealArray) xCoords = new HRealArray(0, m_iNumPoints - 1, 0.0),
                     yCoords = new HRealArray(0, m_iNumPoints - 1, 0.0),
                     zCoords = new HRealArray(0, m_iNumPoints - 1, 0.0);

  for ( int k = 1; k <= m_iNumPoints; ++k )
  {
    const int param = Defl.Parameter(k);
    gp_Pnt    P     = m_curve->Value(param);
    //
    xCoords->ChangeValue(k - 1) = P.X();
    yCoords->ChangeValue(k - 1) = P.Y();
    zCoords->ChangeValue(k - 1) = P.Z();
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  /* =====================================================================
   *  Assuming the input arrays ordered, we build series of line segments
   *  representing our curve in a tabular form. Each segment is
   *  defined as the following pair:
   *
   *   seg = (x[i], y[i], z[i]), (x[i+1], y[i+1], z[i+1])
   *
   *  Take care of degenerated case: single point
   * ===================================================================== */

  const int nbX = xCoords->Length(),
            nbY = yCoords->Length(),
            nbZ = zCoords->Length();

  if ( nbX == 0 || nbY == 0 || nbZ == 0 )
    return 0;

  if ( nbX == 1 && nbY == 1 && nbZ == 1 )
  {
    gp_Pnt P( xCoords->Value(0),
              yCoords->Value(0),
              zCoords->Value(0) );

    this->registerVertex(P, polyOutput);
  }
  else
  {
    for ( int i = 0; i < nbX - 1; i++ )
    {
      gp_Pnt P1( xCoords->Value(i),
                 yCoords->Value(i),
                 zCoords->Value(i) );
      //
      gp_Pnt P2( xCoords->Value(i + 1),
                 yCoords->Value(i + 1),
                 zCoords->Value(i + 1) );

      this->registerLine(P1, P2, polyOutput);
    }
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

//! Adds the given point to the passed polygonal data set.
//! \param[in]     point    point to add.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_CurvatureCombsSource::registerGridPoint(const gp_Pnt& point,
                                                          vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType pid = points->InsertNextPoint( point.X(),
                                           point.Y(),
                                           point.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

//! Adds a line cell into the polygonal data set.
//! \param[in]     pointStart first point.
//! \param[in]     pointEnd   second point.
//! \param[in,out] polyData   polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_CurvatureCombsSource::registerLine(const gp_Pnt& pointStart,
                                                     const gp_Pnt& pointEnd,
                                                     vtkPolyData*  polyData)
{
  std::vector<vtkIdType> pids;
  pids.push_back( this->registerGridPoint(pointStart, polyData) );
  pids.push_back( this->registerGridPoint(pointEnd,   polyData) );

  vtkIdType cellID =
    polyData->InsertNextCell( VTK_LINE, (int) pids.size(), &pids[0] );

  return cellID;
}

//-----------------------------------------------------------------------------

//! Adds a vertex cell into the polygonal data set.
//! \param[in]     point    point to add as a vertex cell.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_CurvatureCombsSource::registerVertex(const gp_Pnt& point,
                                                       vtkPolyData*  polyData)
{
  std::vector<vtkIdType> pids;
  pids.push_back( this->registerGridPoint(point, polyData) );

  vtkIdType cellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) pids.size(), &pids[0] );

  return cellID;
}
