//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_shape_data_source.h>

// OCCT includes
#include <IVtkOCC_ShapeMesher.hxx>
#include <IVtkTools_ShapeObject.hxx>

// VTK includes
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

vtkStandardNewMacro(visu_shape_data_source);

//================================================================
// Function : Constructor
// Purpose  : 
//================================================================
visu_shape_data_source::visu_shape_data_source() : IVtkTools_ShapeDataSource()
{}

//================================================================
// Function : Destructor
// Purpose  : 
//================================================================
visu_shape_data_source::~visu_shape_data_source()
{}

//================================================================
// Function : RequestData
// Purpose  : 
//================================================================
int visu_shape_data_source::RequestData (vtkInformation* theRequest,
                                         vtkInformationVector** theInputVector,
                                         vtkInformationVector* theOutputVector)
{
  vtkPolyData* aPolyData = vtkPolyData::GetData (theOutputVector);
  aPolyData->Allocate();
  vtkPoints* aPts = vtkPoints::New();
  aPolyData->SetPoints (aPts);
  aPts->Delete();

  vtkSmartPointer<vtkPolyData> aTransformedData;
  TopoDS_Shape aShape = myOccShape->GetShape();
  TopLoc_Location aShapeLoc = aShape.Location();

  if (myIsTransformOnly)
  {
    vtkPolyData* aPrevData = myPolyData->getVtkPolyData();
    if (!aShapeLoc.IsIdentity() )
    {
      aTransformedData = this->transform (aPrevData, aShapeLoc);
    }
    else
    {
      aTransformedData = aPrevData;
    }
  }
  else
  {
    IVtkOCC_Shape::Handle aShapeWrapperCopy;
    if (myIsFastTransformMode && !aShapeLoc.IsIdentity() )
    {
      // Reset location before meshing
      aShape.Location (TopLoc_Location() );
      aShapeWrapperCopy = new IVtkOCC_Shape (aShape);
      aShapeWrapperCopy->SetId( myOccShape->GetId() );
    }
    else
    {
      aShapeWrapperCopy = myOccShape;
    }

    myPolyData = new IVtkVTK_ShapeData;
    IVtkOCC_ShapeMesher::Handle aMesher = new IVtkOCC_ShapeMesher(0.1, 15.0*M_PI/180.0);
    aMesher->Build (aShapeWrapperCopy, myPolyData);
    vtkPolyData* aMeshData = myPolyData->getVtkPolyData();

    if (myIsFastTransformMode && !aShapeLoc.IsIdentity() )
    {
      aTransformedData = this->transform(aMeshData, aShapeLoc);
    }
    else
    {
      aTransformedData = aMeshData;
    }
  }

  aPolyData->CopyStructure (aTransformedData);  // Copy points and cells
  aPolyData->CopyAttributes (aTransformedData); // Copy data arrays (sub-shapes IDs)

  // We store the OccShape instance in a IVtkTools_ShapeObject
  // wrapper in vtkInformation object of vtkDataObject, then pass it
  // to the actors through pipelines, so selection logic can access
  // OccShape easily given the actor instance.
  IVtkTools_ShapeObject::SetShapeSource(this, aPolyData);
  aPolyData->GetAttributes (vtkDataObject::CELL)->SetPedigreeIds (SubShapeIDs() );

  return Superclass::RequestData (theRequest, theInputVector, theOutputVector);
}
