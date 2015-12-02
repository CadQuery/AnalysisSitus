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

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(visu_curve_source);

//! Default constructor.
visu_curve_source::visu_curve_source()
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

  return Superclass::RequestData(theRequest, theInputVector, theOutputVector);
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param thePairIndex [in]     index of (X, Y) coordinate pair.
//! \param thePolyData  [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType visu_curve_source::registerGridPoint(const int    thePairIndex,
                                               vtkPolyData* thePolyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = thePolyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType aPid = aPoints->InsertNextPoint( m_XCoords->Value(thePairIndex),
                                             m_YCoords->Value(thePairIndex),
                                             m_ZCoords->Value(thePairIndex) );

  return aPid;
}

//! Adds a line cell into the polygonal data set.
//! \param thePairIndex [in]     index of (X, Y) coordinate pair.
//! \param thePolyData  [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerLine(const int    thePairIndex,
                                          vtkPolyData* thePolyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(thePairIndex,     thePolyData) );
  aPids.push_back( this->registerGridPoint(thePairIndex + 1, thePolyData) );

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

//! Adds a vertex cell into the polygonal data set.
//! \param thePairIndex [in]     index of (X, Y) coordinate pair.
//! \param thePolyData  [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType visu_curve_source::registerVertex(const int    thePairIndex,
                                            vtkPolyData* thePolyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(thePairIndex, thePolyData) );

  vtkIdType aCellID =
    thePolyData->InsertNextCell( VTK_VERTEX, (int) aPids.size(), &aPids[0] );

  return aCellID;
}
