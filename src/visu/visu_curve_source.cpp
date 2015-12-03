//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
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

//! Sets the Data Source input arrays.
//! \param theXCoords [in] X coordinates.
//! \param theYCoords [in] Y coordinates.
//! \param theZCoords [in] Z coordinates.
//! \param theOri     [in] orientation.
void visu_curve_source::SetInputArrays(const Handle(HRealArray)& theXCoords,
                                       const Handle(HRealArray)& theYCoords,
                                       const Handle(HRealArray)& theZCoords,
                                       const visu_orientation    theOri)
{
  if ( theXCoords->Length() != theZCoords->Length() )
  {
    vtkErrorMacro( << "Invalid domain: faulty number of X coordinates" );
    return;
  }

  if ( theYCoords->Length() != theZCoords->Length() )
  {
    vtkErrorMacro( << "Invalid domain: faulty number of Y coordinates" );
  }

  m_XCoords = theXCoords;
  m_YCoords = theYCoords;
  m_ZCoords = theZCoords;
  m_ori     = theOri;
}

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param theRequest      [in] information about data object.
//! \param theInputVector  [in] the input data. As a data source is the start
//!                             stage of the VTK pipeline, theInputVector is
//!                             empty and not used (no input port).
//! \param theOutputVector [in] the pointer to output data, that is filled
//!                             in this method.
//! \return status.
int visu_curve_source::RequestData(vtkInformation*        theRequest,
                                   vtkInformationVector** theInputVector,
                                   vtkInformationVector*  theOutputVector)
{
  if ( m_XCoords.IsNull() || m_YCoords.IsNull() || m_ZCoords.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL coordinate arrays" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* aPolyOutput = vtkPolyData::GetData(theOutputVector);
  aPolyOutput->Allocate();

  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  // Access cell data
  vtkCellData* aCD = aPolyOutput->GetCellData();

  // Array for orientation types
  vtkSmartPointer<vtkIntArray>
    aTypeArr = visu_utils::InitIntArray(ARRNAME_ORIENT_SCALARS);
  aCD->AddArray(aTypeArr);

  /* =====================================================================
   *  Assuming the input arrays ordered, we build series of line segments
   *  representing our curve in a tabular form. Each segment is
   *  defined as the following pair:
   *
   *   S = ( Z(x[i], y[i]), Z(x[i+1], y[i+1]) )
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

  return Superclass::RequestData(theRequest, theInputVector, theOutputVector);
}

//! Adds the given point to the passed polygonal data set.
//! \param thePoint    [in] point to add.
//! \param thePolyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_curve_source::registerGridPoint(const gp_Pnt& thePoint,
                                               vtkPolyData*  thePolyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = thePolyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType aPid = aPoints->InsertNextPoint( thePoint.X(),
                                             thePoint.Y(),
                                             thePoint.Z() );

  return aPid;
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param theIndex    [in]     index of (X, Y, Z) coordinate triple.
//! \param thePolyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_curve_source::registerGridPoint(const int    theIndex,
                                               vtkPolyData* thePolyData)
{
  return this->registerGridPoint( gp_Pnt( m_XCoords->Value(theIndex),
                                          m_YCoords->Value(theIndex),
                                          m_ZCoords->Value(theIndex) ),
                                  thePolyData );

}

//! Adds a line cell into the polygonal data set.
//! \param thePointStart [in]     first point.
//! \param thePointEnd   [in]     second point.
//! \param thePolyData   [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerLine(const gp_Pnt& thePointStart,
                                          const gp_Pnt& thePointEnd,
                                          vtkPolyData*  thePolyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(thePointStart, thePolyData) );
  aPids.push_back( this->registerGridPoint(thePointEnd,   thePolyData) );

  vtkIdType aCellID =
    thePolyData->InsertNextCell( VTK_LINE, (int) aPids.size(), &aPids[0] );

  // Set orientation scalar
  if ( m_ori > VisuOri_Undefined )
  {
    vtkIntArray* aTypeArr =
      vtkIntArray::SafeDownCast( thePolyData->GetCellData()->GetArray(ARRNAME_ORIENT_SCALARS) );
    aTypeArr->InsertNextValue(m_ori);
  }

  return aCellID;
}

//! Adds a line cell into the polygonal data set.
//! \param theIndex    [in]     index of (X, Y, Z) coordinate triple.
//! \param thePolyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerLine(const int    theIndex,
                                          vtkPolyData* thePolyData)
{
  gp_Pnt P1( m_XCoords->Value(theIndex),
             m_YCoords->Value(theIndex),
             m_ZCoords->Value(theIndex) );
  gp_Pnt P2( m_XCoords->Value(theIndex + 1),
             m_YCoords->Value(theIndex + 1),
             m_ZCoords->Value(theIndex + 1) );

  return this->registerLine(P1, P2, thePolyData);
}

//! Adds a vertex cell into the polygonal data set.
//! \param theIndex    [in]     index of (X, Y, Z) coordinate triple.
//! \param thePolyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerVertex(const int    theIndex,
                                            vtkPolyData* thePolyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(theIndex, thePolyData) );

  vtkIdType aCellID =
    thePolyData->InsertNextCell( VTK_VERTEX, (int) aPids.size(), &aPids[0] );

  return aCellID;
}
