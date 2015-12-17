//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_bcurve_poles_source.h>

// Visualization includes
#include <visu_utils.h>

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

vtkStandardNewMacro(visu_bcurve_poles_source);

//! Default constructor.
visu_bcurve_poles_source::visu_bcurve_poles_source() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
visu_bcurve_poles_source::~visu_bcurve_poles_source()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a b-curve.
//! \param curve [in] curve to extract a control polygon from.
//! \return true if poly data has been produced.
bool visu_bcurve_poles_source::SetInputCurve(const Handle(Geom_BSplineCurve)& bcurve)
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

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, theInputVector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
int visu_bcurve_poles_source::RequestData(vtkInformation*        request,
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
  for ( int i = 0; i < nPoles - 1; i++ )
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
    for ( int i = 0; i < nPoles - 1; i++ )
    {
      this->registerVertex(pids[i], aPolyOutput);
    }
    // ... and only then with lines
    for ( int i = 0; i < nPoles - 1; i++ )
    {
      this->registerLine(pids[i], ( (i == nPoles - 2) ? pids[0] : pids[i + 1] ), aPolyOutput);
    }
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_bcurve_poles_source::registerGridPoint(const int    index,
                                                      vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType aPid = aPoints->InsertNextPoint( m_poles->Value(index).X(),
                                             m_poles->Value(index).Y(),
                                             m_poles->Value(index).Z() );

  return aPid;
}

//! Adds a line cell into the polygonal data set.
//! \param pid0     [in]     point 1.
//! \param pid1     [in]     point 2.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_bcurve_poles_source::registerLine(const vtkIdType pid0,
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
vtkIdType visu_bcurve_poles_source::registerVertex(const vtkIdType pid,
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
