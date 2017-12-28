//-----------------------------------------------------------------------------
// Created on: 28 December 2017
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
#include <asiVisu_BSurfPolesSource.h>

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

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_BSurfPolesSource);

//! Default constructor.
asiVisu_BSurfPolesSource::asiVisu_BSurfPolesSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_BSurfPolesSource::~asiVisu_BSurfPolesSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a b-surface.
//! \param[in] bsurf B-spline surface to extract a control net from.
//! \return true if poly data has been produced.
bool asiVisu_BSurfPolesSource::SetInputSurface(const Handle(Geom_BSplineSurface)& bsurf)
{
  if ( bsurf.IsNull() )
    return false;

  const TColgp_Array2OfPnt& poles = bsurf->Poles();

  // For closed surfaces we have to repeat the first pole at the end.
  int nPolesU, nPolesV;
  //
  if ( bsurf->IsUClosed() )
    nPolesU = poles.RowLength() + 1;
  else
    nPolesU = poles.RowLength();
  //
  if ( bsurf->IsVClosed() )
    nPolesV = poles.ColLength() + 1;
  else
    nPolesV = poles.ColLength();

  // Create a shared array of poles: notice that indexation is 0-based (!).
  m_poles = new TColgp_HArray2OfPnt(0, nPolesV - 1,
                                    0, nPolesU - 1);
  //
  for ( int r = 0; r < poles.ColLength(); ++r )
    for ( int c = 0; c < poles.RowLength(); ++c )
      m_poles->ChangeValue(r, c) = poles(r + 1, c + 1);

  // Add trailing pole for a closed surface.
  if ( bsurf->IsUClosed() )
    for ( int r = 0; r < poles.ColLength(); ++r )
      m_poles->ChangeValue(r, nPolesU - 1) = m_poles->Value(r, 0);
  //
  if ( bsurf->IsVClosed() )
    for ( int c = 0; c < poles.RowLength(); ++c )
      m_poles->ChangeValue(nPolesV - 1, c) = m_poles->Value(0, c);

  return true;
}

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param[in]  request      describes "what" algorithm should do. This is
//!                          typically just one key such as REQUEST_INFORMATION.
//! \param[in]  inputVector  inputs of the algorithm.
//! \param[out] outputVector outputs of the algorithm.
int asiVisu_BSurfPolesSource::RequestData(vtkInformation*        request,
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

  const int nPolesU = m_poles->RowLength();
  const int nPolesV = m_poles->ColLength();
  //
  if ( !nPolesU || !nPolesV )
    return 0;

  // Register all points once.
  std::vector< std::vector<vtkIdType> > pids;
  for ( int r = 0; r < nPolesV; ++r )
  {
    std::vector<vtkIdType> rowPids;

    for ( int c = 0; c < nPolesU; ++c )
    {
      rowPids.push_back( this->registerGridPoint(r, c, aPolyOutput) );
    }

    pids.push_back(rowPids);
  }

  if ( nPolesU == 1 && nPolesV == 1 ) // That would be strange!
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
    for ( int r = 0; r < nPolesV; ++r )
      for ( int c = 0; c < nPolesU; ++c )
      {
        this->registerVertex(pids[r][c], aPolyOutput);
      }
    // ... and only then with lines
    for ( int r = 0; r < nPolesV; ++r )
      for ( int c = 0; c < nPolesU - 1; ++c )
      {
        this->registerLine(pids[r][c], pids[r][c + 1], aPolyOutput);
      }
    //
    for ( int c = 0; c < nPolesU; ++c )
      for ( int r = 0; r < nPolesV - 1; ++r )
      {
        this->registerLine(pids[r][c], pids[r + 1][c], aPolyOutput);
      }
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the grid point with the given indices to the passed polygonal data set.
//! \param[in]     row      row index of (X, Y, Z) coordinate triple.
//! \param[in]     col      column index of (X, Y, Z) coordinate triple.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_BSurfPolesSource::registerGridPoint(const int    row,
                                                      const int    col,
                                                      vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = polyData->GetPoints();

  // Push the point into VTK data set
  const gp_Pnt& P = m_poles->Value(row, col);
  //
  vtkIdType aPid = aPoints->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return aPid;
}

//! Adds a line cell into the polygonal data set.
//! \param[in]     pid0     point 1.
//! \param[in]     pid1     point 2.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_BSurfPolesSource::registerLine(const vtkIdType pid0,
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
//! \param[in]     pid      point index.
//! \param[in,out] polyData polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_BSurfPolesSource::registerVertex(const vtkIdType pid,
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
