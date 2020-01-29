//-----------------------------------------------------------------------------
// Created on: 30 November 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPipeline.h>

// asiVisu includes
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkMapper.h>
#include <vtkMatrix4x4.h>
#include <vtkProperty.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_PartPipeline::asiVisu_PartPipeline()
//
: asiVisu_PartPipelineBase(nullptr), m_fPartRed(0.), m_fPartGreen(0.), m_fPartBlue(0.)
{
  m_dmFilter->SetDisplayMode(ShapeDisplayMode_Shaded);
  m_dmFilter->SetAllowExtraScalars(true);

  // Apply lightning rules.
  asiVisu_Utils::ApplyLightingRules( this->Actor() );

  // Transformation filter.
  m_tranformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  //
  this->append(m_tranformFilter);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param[in] dataProvider Data Provider.
void asiVisu_PartPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_PartDataProvider)
    DP = Handle(asiVisu_PartDataProvider)::DownCast(dataProvider);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape shape = DP->GetShape();
  if ( shape.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> dummyData = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyData);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ====================
   *  Configure pipeline
   * ==================== */

  // Set transformation from the data provider.
  m_tranformFilter->SetTransform( DP->GetTransformation() );

  // Pass color source.
  m_source->SetColorSource( DP->PrepareColorSource(), false );

  // Update part-wise colors.
  DP->GetColor(m_fPartRed, m_fPartGreen, m_fPartBlue);

  // Lazy update.
  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Clear cached data which is by design actual for the current state of
    // source only. The source changes, so the cache needs nullification
    this->clearCache();

    /* Configure data source */

    // Set AAG.
    if ( !DP->GetAAG().IsNull() )
      m_source->SetAAG( DP->GetAAG() );
    else
      m_source->SetShape( DP->GetShape() );

    // Pass tessellation parameters.
    m_source->SetTessellationParams( DP->GetLinearDeflection(),
                                     DP->GetAngularDeflection() );

    // Bind to a Data Node using information key
    asiVisu_PartNodeInfo::Store( DP->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

void asiVisu_PartPipeline::updateColors()
{
  Handle(asiVisu_ShapeColorSource) colorSource = m_source->GetColorSource();
  //
  if ( colorSource.IsNull() )
    return;

  // Initialize the scalar generator for faces.
  asiVisu_ShapeRobustTessellator::t_colorScalarGenerator*
    pScalarGen = m_source->GetScalarGenerator();
  //
  if ( !pScalarGen )
    return;

  // Get polygonal data.
  vtkPolyData*
    pData = vtkPolyData::SafeDownCast( this->Mapper()->GetInputDataObject(0, 0) );
  //
  if ( !pData )
    return;

  // Get cell data.
  vtkCellData* pCellData       = pData->GetCellData();
  const int    numOfInputCells = pCellData->GetNumberOfTuples();

  // Get array of pedigree IDs
  vtkIdTypeArray*
    pPedigreeArr = vtkIdTypeArray::SafeDownCast( pCellData->GetPedigreeIds() );

  // Get array of cell types.
  vtkIdTypeArray*
    pShapePrimArr = vtkIdTypeArray::SafeDownCast( pCellData->GetArray(ARRNAME_PART_CELL_TYPES) );

  // Mark cells
  for ( vtkIdType cellId = 0; cellId < numOfInputCells; ++cellId )
  {
    // Check pedigree ID of the cell.
    double pedigreeIdDbl;
    pPedigreeArr->GetTuple(cellId, &pedigreeIdDbl);
    const int pedigreeId = (int) pedigreeIdDbl;

    Handle(asiAlgo_AAG) aag = m_source->GetAAG();
    //
    if ( aag.IsNull() )
      continue;

    const TopoDS_Shape& sh = aag->RequestMapOfSubShapes().FindKey(pedigreeId);
    //
    if ( sh.ShapeType() != TopAbs_FACE )
      continue;

    const int fid   = aag->GetFaceId( TopoDS::Face(sh) );
    const int color = colorSource->GetFaceColor(fid);

    if ( color != -1 )
    {
      const int scalar = pScalarGen->GetScalar(color);

      // Set scalar value in array.
      pShapePrimArr->SetTuple1(cellId, scalar);

      // Update cached scalars.
      if ( m_detectedCells.IsBound(cellId) )
        m_detectedCells.ChangeFind(cellId) = scalar;
      //
      if ( m_selectedCells.IsBound(cellId) )
        m_selectedCells.ChangeFind(cellId) = scalar;
    }
  }
}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_PartPipeline::callback_update()
{
  this->updateColors();

  // Get extra scalars.
  NCollection_DataMap<int, int> extraScalars;
  m_source->GetExtraColorsScalars(extraScalars);
  //
  const int lastUnusedScalar = m_source->GetLastUnusedScalar();

  // Initialize mapper and a lookup table to have not only the default
  // colors for boundary elements, but also the custom colors. The part
  // color is passed as the reference one for choosing scalar colors.
  asiVisu_Utils::InitShapeMapper(m_mapper.Get(),
                                 m_fPartRed,
                                 m_fPartGreen,
                                 m_fPartBlue,
                                 extraScalars,
                                 lastUnusedScalar);

  if ( !m_bMapperColorsSet )
    m_bMapperColorsSet = true;
}
