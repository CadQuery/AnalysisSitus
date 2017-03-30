//-----------------------------------------------------------------------------
// Created on: 03 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapeRobustSource.h>

// asiVisu includes
#include <asiVisu_ShapeRobustTessellator.h>

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

vtkStandardNewMacro(asiVisu_ShapeRobustSource);

//-----------------------------------------------------------------------------

asiVisu_ShapeRobustSource::asiVisu_ShapeRobustSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Input is passed from application data model
}

//-----------------------------------------------------------------------------

asiVisu_ShapeRobustSource::~asiVisu_ShapeRobustSource()
{}

//-----------------------------------------------------------------------------

void asiVisu_ShapeRobustSource::SetAAG(const Handle(asiAlgo_AAG)& aag)
{
  m_aag = aag;
}

//-----------------------------------------------------------------------------

const Handle(asiAlgo_AAG)& asiVisu_ShapeRobustSource::GetAAG() const
{
  return m_aag;
}

//-----------------------------------------------------------------------------

int asiVisu_ShapeRobustSource::RequestData(vtkInformation*        pInfo,
                                           vtkInformationVector** pInputVector,
                                           vtkInformationVector*  pOutputVector)
{
  vtkPolyData* pOutputPolyData = vtkPolyData::GetData(pOutputVector);
  pOutputPolyData->Allocate();
  //
  vtkSmartPointer<vtkPoints> pOutputPts = vtkSmartPointer<vtkPoints>::New();
  pOutputPolyData->SetPoints(pOutputPts);

  /* =============
   *  Run faceter
   * ============= */

  m_shapeData = new asiVisu_ShapeData;
  //
  vtkSmartPointer<asiVisu_ShapeRobustTessellator>
    tessGen = vtkSmartPointer<asiVisu_ShapeRobustTessellator>::New();
  //
  tessGen->Initialize(m_aag, 0.1, 1.0);
  tessGen->Build();
  //
  const Handle(asiVisu_ShapeData)&    tessResult         = tessGen->GetResult();
  const vtkSmartPointer<vtkPolyData>& tessResultPolyData = tessResult->GetPolyData();

  /* ==========
   *  Finalize
   * ========== */

  pOutputPolyData->CopyStructure(tessResultPolyData);  // Copy points and cells
  pOutputPolyData->CopyAttributes(tessResultPolyData); // Copy data arrays

  return vtkPolyDataAlgorithm::RequestData(pInfo, pInputVector, pOutputVector);
}
