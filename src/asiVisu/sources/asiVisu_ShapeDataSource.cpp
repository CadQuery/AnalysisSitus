//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapeDataSource.h>

// A-Situs (visualization) includes
#include <asiVisu_ShapeMesher.h>
#include <asiVisu_ShapeRobustTessellator.h>

// OCCT includes
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

vtkStandardNewMacro(asiVisu_ShapeDataSource);

//-----------------------------------------------------------------------------

asiVisu_ShapeDataSource::asiVisu_ShapeDataSource()
: IVtkTools_ShapeDataSource(), m_faceterType(Faceter_Standard)
{}

//-----------------------------------------------------------------------------

asiVisu_ShapeDataSource::~asiVisu_ShapeDataSource()
{}

//-----------------------------------------------------------------------------

int asiVisu_ShapeDataSource::RequestData(vtkInformation*        theRequest,
                                         vtkInformationVector** theInputVector,
                                         vtkInformationVector*  theOutputVector)
{
  vtkPolyData* aPolyData = vtkPolyData::GetData (theOutputVector);
  aPolyData->Allocate();
  vtkPoints* aPts = vtkPoints::New();
  aPolyData->SetPoints(aPts);
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

    /* =============
     *  Run faceter
     * ============= */

    myPolyData = new IVtkVTK_ShapeData;
    //
    if ( m_faceterType == Faceter_Standard )
    {
      asiVisu_ShapeMesher::Handle aMesher = new asiVisu_ShapeMesher(0.001, 5.0*M_PI/180.0, 0, 0);
      aMesher->Build(aShapeWrapperCopy, myPolyData);
    }
    else if ( m_faceterType == Faceter_Robust )
    {
      asiVisu_ShapeRobustTessellator::Handle aMesher = new asiVisu_ShapeRobustTessellator();
      aMesher->Build(aShapeWrapperCopy, myPolyData);
    }
    //
    vtkPolyData* aMeshData = myPolyData->getVtkPolyData();

    /* ==============
     *  Post-process
     * ============== */

    if ( myIsFastTransformMode && !aShapeLoc.IsIdentity() )
      aTransformedData = this->transform(aMeshData, aShapeLoc);
    else
      aTransformedData = aMeshData;
  }

  aPolyData->CopyStructure(aTransformedData);  // Copy points and cells
  aPolyData->CopyAttributes(aTransformedData); // Copy data arrays (sub-shapes IDs)

  // We store the OccShape instance in a IVtkTools_ShapeObject
  // wrapper in vtkInformation object of vtkDataObject, then pass it
  // to the actors through pipelines, so selection logic can access
  // OccShape easily given the actor instance.
  IVtkTools_ShapeObject::SetShapeSource(this, aPolyData);
  aPolyData->GetAttributes(vtkDataObject::CELL)->SetPedigreeIds( SubShapeIDs() );

  return vtkPolyDataAlgorithm::RequestData (theRequest, theInputVector, theOutputVector);
}
