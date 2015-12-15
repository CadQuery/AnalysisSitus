//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_curve_source.h>

// Visualization includes
#include <visu_utils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OCCT includes
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(visu_curve_source);

//! Default constructor.
visu_curve_source::visu_curve_source() : vtkPolyDataAlgorithm(), m_fOriTipSize(0.0)
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
visu_curve_source::~visu_curve_source()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a parametric curve.
//! \param curve [in] curve to discretize.
//! \param ori   [in] orientation.
//! \return true if poly data has been produced.
bool visu_curve_source::SetInputCurve(const Handle(Geom_Curve)& curve,
                                      const visu_orientation    ori)
{
  const double f = visu_utils::TrimInf( curve->FirstParameter() ),
               l = visu_utils::TrimInf( curve->LastParameter() );

  // Discretize
  GeomAdaptor_Curve gac(curve, f, l);
  GCPnts_QuasiUniformDeflection Defl(gac, 0.0001);
  if ( !Defl.IsDone() )
  {
    vtkErrorMacro( << "Cannot discretize curve" );
    return false;
  }
  const int nPts = Defl.NbPoints();
  if ( !nPts )
    return false;

  // Allocate arrays
  Handle(HRealArray) xCoords = new HRealArray(0, nPts - 1, 0.0),
                     yCoords = new HRealArray(0, nPts - 1, 0.0),
                     zCoords = new HRealArray(0, nPts - 1, 0.0);

  for ( int k = 1; k <= nPts; ++k )
  {
    gp_Pnt P = Defl.Value(k);
    //
    xCoords->ChangeValue(k - 1) = P.X();
    yCoords->ChangeValue(k - 1) = P.Y();
    zCoords->ChangeValue(k - 1) = P.Z();
  }

  // Pass arrays as inputs
  this->SetInputArrays(xCoords, yCoords, zCoords, ori);
  return true;
}

//! Sets the Data Source input arrays. These arrays represent a curve
//! explicitly as series of points. Of course, to have these points you need
//! to sample your curve beforehand.
//!
//! \param xCoords [in] X coordinates.
//! \param yCoords [in] Y coordinates.
//! \param zCoords [in] Z coordinates.
//! \param ori     [in] orientation.
void visu_curve_source::SetInputArrays(const Handle(HRealArray)& xCoords,
                                       const Handle(HRealArray)& yCoords,
                                       const Handle(HRealArray)& zCoords,
                                       const visu_orientation    ori)
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

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, theInputVector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
int visu_curve_source::RequestData(vtkInformation*        request,
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

  vtkPolyData* aPolyOutput = vtkPolyData::GetData(outputVector);
  aPolyOutput->Allocate();

  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  // Access cell data
  vtkCellData* aCD = aPolyOutput->GetCellData();

  // Array for orientation types
  vtkSmartPointer<vtkIntArray> aTypeArr = visu_utils::InitIntArray(ARRNAME_ORIENT_SCALARS);
  aCD->AddArray(aTypeArr);

  /* =====================================================================
   *  Assuming the input arrays ordered, we build series of line segments
   *  representing our curve in a tabular form. Each segment is
   *  defined as the following pair:
   *
   *   seg = (x[i], y[i], z[i]), (x[i+1], y[i+1], z[i+1])
   *
   *  Take care of degenerated case: single point
   * ===================================================================== */

  const int nbX = m_XCoords->Length(),
            nbY = m_YCoords->Length();

  if ( nbX == 0 || nbY == 0 )
    return 0;

  if ( nbX == 1 && nbY == 1 )
  {
    this->registerVertex(0, aPolyOutput);
  }
  else
  {
    for ( int i = 0; i < nbX - 1; i++ )
      this->registerLine(i, aPolyOutput);
  }

  /* =====================
   *  Add orientation tip
   * ===================== */

  if ( m_fOriTipSize > RealEpsilon() )
  {
    // Derive tangent
    if ( m_oriN.Magnitude() < RealEpsilon() )
      m_oriT = gp_XYZ( m_XCoords->Value( m_XCoords->Upper() ),
                       m_YCoords->Value( m_YCoords->Upper() ),
                       m_ZCoords->Value( m_ZCoords->Upper() ) )
             - gp_XYZ( m_XCoords->Value( m_XCoords->Upper() - 1 ),
                       m_YCoords->Value( m_YCoords->Upper() - 1 ),
                       m_ZCoords->Value( m_ZCoords->Upper() - 1 ) );

    // Normalize tangent
    m_oriT.Normalize();

    // Derive normal
    if ( m_oriN.Magnitude() > RealEpsilon() )
      m_oriN.Normalize();
    else
      m_oriN = m_oriT.Rotated( gp_Ax1( gp::Origin(), gp::DZ() ), M_PI/2); // Lets take any orthogonal vector

    // Build tip
    if ( m_oriT.Magnitude() > RealEpsilon() &&
         m_oriN.Magnitude() > RealEpsilon() )
    {
      gp_Pnt P1( m_XCoords->Value( m_XCoords->Upper() ),
                 m_YCoords->Value( m_YCoords->Upper() ),
                 m_ZCoords->Value( m_ZCoords->Upper() ) );

      const double tip_angle = 3*M_PI/4;
      gp_Pnt P2( P1.XYZ() + m_oriT.XYZ()*m_fOriTipSize*Cos(tip_angle) + m_oriN.XYZ()*m_fOriTipSize*Sin(tip_angle) );
      gp_Pnt P3( P1.XYZ() + m_oriT.XYZ()*m_fOriTipSize*Cos(tip_angle) - m_oriN.XYZ()*m_fOriTipSize*Sin(tip_angle) );

      this->registerLine(P1, P2, aPolyOutput);
      this->registerLine(P1, P3, aPolyOutput);
    }
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the given point to the passed polygonal data set.
//! \param point    [in] point to add.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_curve_source::registerGridPoint(const gp_Pnt& point,
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

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_curve_source::registerGridPoint(const int    index,
                                               vtkPolyData* polyData)
{
  return this->registerGridPoint( gp_Pnt( m_XCoords->Value(index),
                                          m_YCoords->Value(index),
                                          m_ZCoords->Value(index) ),
                                  polyData );

}

//! Adds a line cell into the polygonal data set.
//! \param pointStart [in]     first point.
//! \param pointEnd   [in]     second point.
//! \param polyData   [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerLine(const gp_Pnt& pointStart,
                                          const gp_Pnt& pointEnd,
                                          vtkPolyData*  polyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(pointStart, polyData) );
  aPids.push_back( this->registerGridPoint(pointEnd,   polyData) );

  vtkIdType aCellID =
    polyData->InsertNextCell( VTK_LINE, (int) aPids.size(), &aPids[0] );

  // Set orientation scalar
  if ( m_ori > VisuOri_Undefined )
  {
    vtkIntArray* aTypeArr =
      vtkIntArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_ORIENT_SCALARS) );
    aTypeArr->InsertNextValue(m_ori);
  }

  return aCellID;
}

//! Adds a line cell into the polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerLine(const int    index,
                                          vtkPolyData* polyData)
{
  gp_Pnt P1( m_XCoords->Value(index),
             m_YCoords->Value(index),
             m_ZCoords->Value(index) );
  gp_Pnt P2( m_XCoords->Value(index + 1),
             m_YCoords->Value(index + 1),
             m_ZCoords->Value(index + 1) );

  return this->registerLine(P1, P2, polyData);
}

//! Adds a vertex cell into the polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerVertex(const int    index,
                                            vtkPolyData* polyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(index, polyData) );

  vtkIdType aCellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) aPids.size(), &aPids[0] );

  return aCellID;
}
