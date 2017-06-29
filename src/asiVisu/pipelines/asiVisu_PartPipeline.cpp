//-----------------------------------------------------------------------------
// Created on: 30 November 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPipeline.h>

// Visualization includes
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkProperty.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_PartPipeline::asiVisu_PartPipeline() : asiVisu_PartPipelineBase(NULL)
{
  m_dmFilter->SetDisplayMode(DisplayMode_Shaded);

  // Apply lightning rules
  asiVisu_Utils::ApplyLightingRules( this->Actor() );
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param dataProvider [in] Data Provider.
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

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Clear cached data which is by design actual for the current state of
    // source only. The source changes, so the cache needs nullification
    this->clearCache();

    // Configure data source
    m_source->SetAAG( DP->GetAAG() );
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
