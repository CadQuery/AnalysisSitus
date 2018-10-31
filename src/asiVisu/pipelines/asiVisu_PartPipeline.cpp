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
//    * Neither the name of Sergey Slyadnev nor the
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
  m_dmFilter->SetDisplayMode(ShapeDisplayMode_Shaded);

  // Apply lightning rules
  asiVisu_Utils::ApplyLightingRules( this->Actor() );
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

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Clear cached data which is by design actual for the current state of
    // source only. The source changes, so the cache needs nullification
    this->clearCache();

    // Configure data source
    if ( !DP->GetAAG().IsNull() )
    {
      m_source->SetAAG( DP->GetAAG() );
    }
    else
    {
      m_source->SetShape( DP->GetShape() );
    }
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
