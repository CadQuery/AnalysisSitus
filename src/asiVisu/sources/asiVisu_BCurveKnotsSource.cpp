//-----------------------------------------------------------------------------
// Created on: 02 September 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_BCurveKnotsSource.h>

// Visualization includes
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

vtkStandardNewMacro(asiVisu_BCurveKnotsSource);

//! Default constructor.
asiVisu_BCurveKnotsSource::asiVisu_BCurveKnotsSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_BCurveKnotsSource::~asiVisu_BCurveKnotsSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a b-curve.
//! \param curve [in] curve to extract a knot sequence from.
//! \return true if poly data has been produced.
bool asiVisu_BCurveKnotsSource::SetInputCurve(const Handle(Geom_BSplineCurve)& bcurve)
{
  if ( bcurve.IsNull() )
    return false;

  m_curve = bcurve;
  return true;
}

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, inputVector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
int asiVisu_BCurveKnotsSource::RequestData(vtkInformation*        request,
                                          vtkInformationVector** inputVector,
                                          vtkInformationVector*  outputVector)
{
  if ( m_curve.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: b-curve is not set" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  const int nKnots = m_curve->Knots().Length();
  if ( !nKnots )
    return 0;

  // Populate poly data with vertices
  for ( int i = 0; i < nKnots; ++i )
  {
    // Register geometry (point) and topology (vertex)
    const vtkIdType pid = this->registerGridPoint(i, polyOutput);
    this->registerVertex(pid, polyOutput);
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_BCurveKnotsSource::registerGridPoint(const int    index,
                                                      vtkPolyData* polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  gp_Pnt P = m_curve->Value( m_curve->Knots()(index + 1) );
  //
  vtkIdType pid = points->InsertNextPoint( P.X(), P.Y(), P.Z() );

  return pid;
}

//! Adds a vertex cell into the polygonal data set.
//! \param pid      [in]     point index.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_BCurveKnotsSource::registerVertex(const vtkIdType pid,
                                                   vtkPolyData*    polyData)
{
  std::vector<vtkIdType> pids;
  pids.push_back(pid);
  //
  vtkIdType cellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) pids.size(), &pids[0] );

  return cellID;
}
