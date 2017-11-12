//-----------------------------------------------------------------------------
// Created on: 15 December 2015
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
#include <asiVisu_BCurvePolesSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkIdTypeArray.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OCCT includes
#include <TColgp_Array1OfPnt2d.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_BCurvePolesSource);

//! Default constructor.
asiVisu_BCurvePolesSource::asiVisu_BCurvePolesSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_BCurvePolesSource::~asiVisu_BCurvePolesSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a b-curve.
//! \param curve [in] curve to extract a control polygon from.
//! \return true if poly data has been produced.
bool asiVisu_BCurvePolesSource::SetInputCurve(const Handle(Geom_BSplineCurve)& bcurve)
{
  if ( bcurve.IsNull() )
    return false;

  const TColgp_Array1OfPnt& poles = bcurve->Poles();

  int nPoles;
  if ( bcurve->IsClosed() )
  {
    // For closed curves we have to repeat the first pole at the end
    nPoles = poles.Length() + 1;
  }
  else
    nPoles = poles.Length();

  // Create a shared array of poles: notice that indexation is 0-based (!)
  m_poles = new TColgp_HArray1OfPnt( 0, nPoles - 1 );
  //
  for ( int k = 0; k < poles.Length(); ++k )
    m_poles->ChangeValue(k) = poles(k + 1);

  // Add trailing pole for a closed curve
  if ( bcurve->IsClosed() )
    m_poles->ChangeLast() = m_poles->First();

  return true;
}

//! Initialize data source from a two-dimensional b-curve.
//! \param curve [in] curve to extract a control polygon from.
//! \return true if poly data has been produced.
bool asiVisu_BCurvePolesSource::SetInputCurve2d(const Handle(Geom2d_BSplineCurve)& bcurve)
{
  if ( bcurve.IsNull() )
    return false;

  const TColgp_Array1OfPnt2d& poles = bcurve->Poles();

  int nPoles;
  if ( bcurve->IsClosed() )
  {
    // For closed curves we have to repeat the first pole at the end
    nPoles = poles.Length() + 1;
  }
  else
    nPoles = poles.Length();

  // Create a shared array of poles: notice that indexation is 0-based (!)
  m_poles = new TColgp_HArray1OfPnt( 0, nPoles - 1 );
  //
  for ( int k = 0; k < poles.Length(); ++k )
  {
    const gp_Pnt2d& p2d = poles(k + 1);
    //
    m_poles->ChangeValue(k).SetX( p2d.X() );
    m_poles->ChangeValue(k).SetY( p2d.Y() );
    m_poles->ChangeValue(k).SetZ( 0.0 );
  }

  return true;
}

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_BCurvePolesSource::RequestData(vtkInformation*        request,
                                           vtkInformationVector** inputVector,
                                           vtkInformationVector*  outputVector)
{
  if ( m_poles.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: poles are not set" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* aPolyOutput = vtkPolyData::GetData(outputVector);
  aPolyOutput->Allocate();

  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  // Global IDs for selection
  vtkSmartPointer<vtkIdTypeArray> GIDsArray = vtkSmartPointer<vtkIdTypeArray>::New();
  GIDsArray->SetNumberOfComponents(1);
  aPolyOutput->GetCellData()->SetGlobalIds(GIDsArray);

  /* =====================================================================
   *  Assuming the input arrays ordered, we build series of line segments
   *  representing our polygon in a tabular form. Each segment is
   *  defined as the following pair:
   *
   *   seg = (x[i], y[i], z[i]), (x[i+1], y[i+1], z[i+1])
   *
   *  Take care of degenerated case: single point
   * ===================================================================== */

  const int nPoles = m_poles->Length();
  if ( !nPoles )
    return 0;

  // Register all points once
  std::vector<vtkIdType> pids;
  for ( int i = 0; i < nPoles; ++i )
  {
    pids.push_back( this->registerGridPoint(i, aPolyOutput) );
  }

  if ( nPoles == 1 )
  {
    this->registerVertex(0, aPolyOutput);
  }
  else
  {
    // CAUTION: do not "optimize" the following two loops into one. It is
    //          important that all vertices are registered BEFORE all lines
    //          as otherwise we have troubles during selection (picked
    //          IDs do not correspond to what we assign here: it looks
    //          like VTK reorders IDs).
    //
    // From VTK docs: because vtkPolyData is implemented with four separate
    //                instances of vtkCellArray to represent 0D vertices, 1D
    //                lines, 2D polygons, and 2D triangle strips, it is
    //                possible to create vtkPolyData instances that consist of
    //                a mixture of cell types. Because of the design of the
    //                class, there are certain limitations on how mixed cell
    //                types are inserted into the vtkPolyData, and in turn the
    //                order in which they are processed and rendered. To
    //                preserve the consistency of cell ids, and to insure that
    //                cells with cell data are rendered properly, users must
    //                insert mixed cells in the order of vertices (vtkVertex
    //                and vtkPolyVertex), lines (vtkLine and vtkPolyLine),
    //                polygons (vtkTriangle, vtkQuad, vtkPolygon), and triangle
    //                strips (vtkTriangleStrip).

    // So first we populate poly data with vertices...
    for ( int i = 0; i < nPoles; ++i )
    {
      this->registerVertex(pids[i], aPolyOutput);
    }
    // ... and only then with lines
    for ( int i = 0; i < nPoles - 1; ++i )
    {
      this->registerLine(pids[i], pids[i + 1], aPolyOutput);
    }
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_BCurvePolesSource::registerGridPoint(const int    index,
                                                       vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = polyData->GetPoints();

  // Push the point into VTK data set
  const gp_Pnt& P = m_poles->Value(index);
  //
  vtkIdType aPid = aPoints->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return aPid;
}

//! Adds a line cell into the polygonal data set.
//! \param pid0     [in]     point 1.
//! \param pid1     [in]     point 2.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_BCurvePolesSource::registerLine(const vtkIdType pid0,
                                                  const vtkIdType pid1,
                                                  vtkPolyData*    polyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back(pid0);
  aPids.push_back(pid1);

  vtkIdType aCellID =
    polyData->InsertNextCell( VTK_LINE, (int) aPids.size(), &aPids[0] );

  // WARNING: in VTK when assigning global IDs, make sure that all cells
  //          get some ID values. You cannot leave some cells without IDs
  //          even if you don't care of these cells. E.g. if you want
  //          to work with poles only, and you have nothing to do with
  //          lines, you will anyway need to bind some global IDs to these
  //          lines.

  // Assign a global ID
  vtkSmartPointer<vtkIdTypeArray>
    GIDsArray = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetGlobalIds() );
  //
  GIDsArray->InsertNextValue(aCellID);

  return aCellID;
}

//! Adds a vertex cell into the polygonal data set.
//! \param pid      [in]     point index.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_BCurvePolesSource::registerVertex(const vtkIdType pid,
                                                    vtkPolyData*    polyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back(pid);
  //
  vtkIdType aCellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) aPids.size(), &aPids[0] );

  // Assign a global ID
  vtkSmartPointer<vtkIdTypeArray>
    GIDsArray = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetGlobalIds() );
  //
  GIDsArray->InsertNextValue(aCellID);

  return aCellID;
}
