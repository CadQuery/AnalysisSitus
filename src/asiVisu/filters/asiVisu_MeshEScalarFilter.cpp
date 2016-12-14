//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshEScalarFilter.h>

// Visualization includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkMath.h>

vtkStandardNewMacro(asiVisu_MeshEScalarFilter);

//! Default constructor.
asiVisu_MeshEScalarFilter::asiVisu_MeshEScalarFilter()
{
  m_scalarMap = new HScalarMap(100);

  m_fMinScalar =  VTK_FLOAT_MAX;
  m_fMaxScalar = -VTK_FLOAT_MAX;

  //this->GetInformation()->Set(vtkAlgorithm::PRESERVES_RANGES(), 1);
  //this->GetInformation()->Set(vtkAlgorithm::PRESERVES_BOUNDS(), 1);
}

//! Destructor.
asiVisu_MeshEScalarFilter::~asiVisu_MeshEScalarFilter()
{
}

//! Specifies the applicable input types.
//! \param port [not used] connection port.
//! \param info [in/out]   information object.
//! \return result code.
int asiVisu_MeshEScalarFilter::FillInputPortInformation(int /*port*/, vtkInformation* info)
{
  // This filter utilizes vtkDataSet cell traversal so it
  // supports any data set type as input
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//! Main processing routine.
//! \param theInfo   [not used] information object.
//! \param theInput  [in]       input data.
//! \param theOutput [out]      output data.
//! \return result code.
int asiVisu_MeshEScalarFilter::RequestData(vtkInformation*,
                                           vtkInformationVector** theInputVector,
                                           vtkInformationVector*  theOutputVector)
{
  // Get input and output data sets
  vtkDataSet*  anInput  = vtkDataSet::GetData(theInputVector[0]);
  vtkPolyData* anOutput = vtkPolyData::GetData(theOutputVector);

  vtkIdType aNbCells  = anInput->GetNumberOfCells();
  vtkIdType aNbPoints = anInput->GetNumberOfPoints();

  // Skip execution if there are no input geometry
  if ( aNbCells < 1 || aNbPoints < 1 )
  {
    vtkDebugMacro("No data to process!");
    return 1;
  }

  // Skip execution if there is no input scalars
  if ( m_scalarMap->IsEmpty() )
  {
    m_fMaxScalar = m_fMinScalar = 0.0;
    vtkDebugMacro("No scalars to process!");
    return 1;
  }

  // Reset min & max scalar values
  m_fMaxScalar = -VTK_FLOAT_MAX;
  m_fMinScalar =  VTK_FLOAT_MAX;

  // Allocate working space for the new and old cell point lists
  vtkSmartPointer<vtkIdList> anOldPointIds = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkIdList> aNewPointIds = vtkSmartPointer<vtkIdList>::New();
  anOldPointIds->Allocate(VTK_CELL_SIZE);
  aNewPointIds->Allocate(VTK_CELL_SIZE);

  // Allocate roughly the space needed for the output cells
  anOutput->Allocate(aNbCells);

  // Allocate roughly the space for the new collection of points
  vtkSmartPointer<vtkPoints> aNewPoints = vtkSmartPointer<vtkPoints>::New();
  aNewPoints->Allocate(aNbPoints, aNbPoints);

  // Get arrays of IDs
  vtkIntArray* aCellIDs =
    vtkIntArray::SafeDownCast( anInput->GetCellData()->GetArray(ARRNAME_MESH_ELEM_IDS) );

  // Allocate space for data associated with the new set of points
  vtkPointData* anInputPD  = anInput->GetPointData();
  vtkPointData* anOutputPD = anOutput->GetPointData();

  // Allocate space for data associated with the new set of cells
  vtkCellData* anInputCD  = anInput->GetCellData();
  vtkCellData* anOutputCD = anOutput->GetCellData();
  anOutputCD->CopyAllocate(anInputCD, aNbCells, aNbCells);

  // Allocate an array for cell scalars
  vtkSmartPointer<vtkDoubleArray>
    aNewCellScalars = asiVisu_Utils::InitDoubleArray(ARRNAME_MESH_N_SCALARS);

  // Map for shared VTK points
  NCollection_DataMap<int, vtkIdType> aRegPoints;

  // Traverse all cells
  for ( vtkIdType aCellId = 0; aCellId < aNbCells; ++aCellId )
  {
    // Get the list of points for this cell
    anInput->GetCellPoints(aCellId, anOldPointIds);
    vtkIdType aNbCellPoints = anOldPointIds->GetNumberOfIds();

    // Create new points for this cell
    aNewPointIds->Reset();

    // Get ID of the mesh element
    int anElemID = aCellIDs->GetValue(aCellId);

    // Access scalar value
    double aScalar = 0.0;
    bool isCellScalared = this->scalarForElem(anElemID, aScalar);
    this->adjustMinMax(aScalar);

    if ( !isCellScalared )
      continue;

    for ( vtkIdType aNodeLocalID = 0; aNodeLocalID < aNbCellPoints; ++aNodeLocalID )
    {
      vtkIdType anOldPid = anOldPointIds->GetId(aNodeLocalID);

      // Create the new point for this cell
      vtkIdType aNewPid;
      if ( !aRegPoints.IsBound( (int) anOldPid ) )
      {
        double aCoords[3];
        anInput->GetPoint(anOldPid, aCoords);
        aNewPid = aNewPoints->InsertNextPoint(aCoords);
        aRegPoints.Bind( (int) anOldPid, aNewPid );
      }
      else
        aNewPid = aRegPoints.Find( (int) anOldPid );

      aNewPointIds->InsertId(aNodeLocalID, aNewPid);
      anOutputPD->CopyData(anInputPD, anOldPid, aNewPid);
    }

    // Store the new cell in the output
    vtkIdType aNewCellPid = anOutput->InsertNextCell(anInput->GetCellType(aCellId), aNewPointIds);

    // Associate scalar with cell data
    double aScalarTuple[1] = {aScalar};
    aNewCellScalars->InsertTypedTuple(aNewCellPid, aScalarTuple);
  }

  // Set scalars to cell data
  anOutputCD->SetScalars(aNewCellScalars);

  // Store the new set of points in the output
  anOutput->SetPoints(aNewPoints);

  // Avoid keeping extra memory around
  anOutput->Squeeze();

  return 1;
}

//! Retrieves scalar value for the element with the given ID.
//! \param theElemID [in]  ID of the mesh element to access scalar value for.
//! \param theScalar [out] requested scalar if any.
//! \return true if scalar value has been found, false -- otherwise.
bool asiVisu_MeshEScalarFilter::scalarForElem(const int theElemID,
                                              double&   theScalar)
{
  if ( !m_scalarMap->IsBound(theElemID) )
    return false;

  theScalar = m_scalarMap->Find(theElemID);
  return true;
}

//! Adjusts min & max scalar values against the passed scalar value.
//! \param theScalar [in] scalar value to adjust the min & max
//!        values against.
void asiVisu_MeshEScalarFilter::adjustMinMax(double theScalar)
{
  if ( theScalar > m_fMaxScalar )
    m_fMaxScalar = theScalar;
  if ( theScalar < m_fMinScalar )
    m_fMinScalar = theScalar;
}
