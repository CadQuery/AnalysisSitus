//-----------------------------------------------------------------------------
// Created on: 15 April 2017
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
#include <asiVisu_ShapePipeline.h>

// asiVisu includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_ShapePipeline::asiVisu_ShapePipeline(bool isScalarMode)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bIsScalarModeOn  ( isScalarMode ),
  m_bMapperColorsSet ( false )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Initialize Data Source.
  m_source = vtkSmartPointer<asiVisu_ShapeRobustSource>::New();

  // Filter for normals.
  m_normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  // Display mode filter.
  m_dmFilter = vtkSmartPointer<asiVisu_ShapeDisplayModeFilter>::New();
  m_dmFilter->SetDisplayMode(ShapeDisplayMode_Shaded);

  // Set line width.
  this->Actor()->GetProperty()->SetLineWidth(1);
  this->Actor()->GetProperty()->SetPointSize(8);
  //
  asiVisu_Utils::ApplyLightingRules( this->Actor() );

  // Compose pipeline.
  this->append(m_dmFilter);
  this->append(m_normalsFilter);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapePipeline::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                               ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;
  //
  m_source->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param[in] dataProvider Data Provider.
void asiVisu_ShapePipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_ShapeDataProvider)
    DP = Handle(asiVisu_ShapeDataProvider)::DownCast(dataProvider);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape shape = DP->GetShape();
  if ( shape.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline.
    vtkSmartPointer<vtkPolyData> dummyData = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyData);
    this->Modified(); // Update modification timestamp.
    return; // Do nothing.
  }

  /* ====================
   *  Configure pipeline
   * ==================== */

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Get deflection values.
    const double linDefl = DP->GetLinearDeflection();
    const double angDefl = DP->GetAngularDeflection();

    // Configure data source.
    m_source->SetShape(shape);
    m_source->SetTessellationParams(linDefl, angDefl);

    // Bind to a Data Node using information key.
    asiVisu_NodeInfo::Store( DP->GetNodeID(), this->Actor() );

    // Initialize pipeline.
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp.
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_ShapePipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_ShapePipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_ShapePipeline::callback_update()
{
  if ( m_bIsScalarModeOn && !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = true;
  }

  if ( m_bIsScalarModeOn )
    this->Mapper()->ScalarVisibilityOn();
  else
    this->Mapper()->ScalarVisibilityOff();
}
