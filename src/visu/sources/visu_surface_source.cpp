//-----------------------------------------------------------------------------
// Created on: 25 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_surface_source.h>

// Visualization includes
#include <visu_utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(visu_surface_source);

//! Default constructor.
visu_surface_source::visu_surface_source() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
visu_surface_source::~visu_surface_source()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source with a parametric surface.
//! \param surf [in] surface to visualize.
//! \return true if poly data has been produced.
bool visu_surface_source::SetInputSurface(const Handle(Geom_Surface)& surf)
{
  m_surf = surf;
  return false;
}

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, the Input Vector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
int visu_surface_source::RequestData(vtkInformation*        request,
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector*  outputVector)
{
  if ( m_surf.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL surface" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* aPolyOutput = vtkPolyData::GetData(outputVector);
  aPolyOutput->Allocate();

  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  // Access cell data
  vtkCellData* aCD = aPolyOutput->GetCellData();

  // Array for orientation types
  vtkSmartPointer<vtkIntArray>
    gaussCurvature = visu_utils::InitIntArray(ARRNAME_SURF_GAUSSIAN_CURVATURE);
  //
  aCD->AddArray(gaussCurvature);

  //---------------------------------------------------------------------------

  const int nSteps = 10;

  double uMin, uMax, vMin, vMax;
  m_surf->Bounds(uMin, uMax, vMin, vMax);

  uMin = visu_utils::TrimInf(uMin);
  uMax = visu_utils::TrimInf(uMax);
  vMin = visu_utils::TrimInf(vMin);
  vMax = visu_utils::TrimInf(vMax);

  const double uStep = (uMax - uMin) / nSteps;
  const double vStep = (vMax - vMin) / nSteps;

  //// Generate u-isos
  //double u     = uMin;
  //bool   uStop = false;
  //while ( !uStop )
  //{
  //  if ( u > uMax )
  //  {
  //    u     = uMax;
  //    uStop = true;
  //  }

  //  Handle(Geom_Curve) uIso = S->UIso(u);
  //  if ( uIso.IsNull() )
  //    continue;

  //  u += uStep;

  //  // Allocate Data Source
  //  vtkSmartPointer<visu_curve_source>
  //    curveSource = vtkSmartPointer<visu_curve_source>::New();

  //  // Set geometry to be converted to VTK polygonal DS
  //  if ( !curveSource->SetInputCurve(uIso) )
  //    continue; // No poly data produced

  //  // Append poly data
  //  appendFilter->AddInputConnection( curveSource->GetOutputPort() );
  //}

  //// Generate v-isos
  //double v     = vMin;
  //bool   vStop = false;
  //while ( !vStop )
  //{
  //  if ( v > vMax )
  //  {
  //    v     = vMax;
  //    vStop = true;
  //  }

  //  Handle(Geom_Curve) vIso = S->VIso(v);
  //  if ( vIso.IsNull() )
  //    continue;

  //  v += vStep;

  //  // Allocate Data Source
  //  vtkSmartPointer<visu_curve_source>
  //    curveSource = vtkSmartPointer<visu_curve_source>::New();

  //  // Set geometry to be converted to VTK polygonal DS
  //  if ( !curveSource->SetInputCurve(vIso) )
  //    continue; // No poly data produced

  //  // Append poly data
  //  appendFilter->AddInputConnection( curveSource->GetOutputPort() );
  //}

  //---------------------------------------------------------------------------

  // TODO

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the given point to the passed polygonal data set.
//! \param point    [in]     point to add.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_surface_source::registerGridPoint(const gp_Pnt& point,
                                                 vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType aPid = aPoints->InsertNextPoint( point.X(),
                                             point.Y(),
                                             point.Z() );

  return aPid;
}

//! Adds a line cell into the polygonal data set.
//! \param pointStart [in]     first point.
//! \param pointEnd   [in]     second point.
//! \param polyData   [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_surface_source::registerLine(const gp_Pnt& pointStart,
                                            const gp_Pnt& pointEnd,
                                            vtkPolyData*  polyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(pointStart, polyData) );
  aPids.push_back( this->registerGridPoint(pointEnd,   polyData) );

  vtkIdType aCellID =
    polyData->InsertNextCell( VTK_LINE, (int) aPids.size(), &aPids[0] );

  return aCellID;
}
