//-----------------------------------------------------------------------------
// Created on: 27 December 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiVisu_BCurveRepersPipeline.h>

// asiVisu includes
#include <asiVisu_BCurveRepersSource.h>
#include <asiVisu_CurveDataProvider.h>
#include <asiVisu_NodeInfo.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <Geom_BSplineCurve.hxx>

//-----------------------------------------------------------------------------

asiVisu_BCurveRepersPipeline::asiVisu_BCurveRepersPipeline(const bool allowSelection)
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() )
{
  m_iForcedActiveReper = -1;
  m_bAllowSelection    = allowSelection;

  this->Actor()->GetProperty()->SetPointSize(8.0f);
}

//-----------------------------------------------------------------------------

void asiVisu_BCurveRepersPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_CurveDataProvider)
    dp = Handle(asiVisu_CurveDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  // Get reper points.
  std::vector<gp_XYZ> pts;
  dp->GetReperPoints(pts);

  // Do nothing for empty input.
  if ( !pts.size() )
  {
    // Pass empty data set in order to have valid pipeline.
    vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp.
    return; // Do nothing.
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  // Forced activation is allowed.
  if ( dp->MustExecute( this->GetMTime() ) || (m_iForcedActiveReper != -1) )
  {
    // Bind to a Data Node using information key.
    asiVisu_NodeInfo::Store( dp->GetNodeID(), this->Actor() );

    // B-curve repers.
    vtkSmartPointer<asiVisu_BCurveRepersSource>
      bRepersSrc = vtkSmartPointer<asiVisu_BCurveRepersSource>::New();
    //
    bRepersSrc->SetInputPoints(pts);

    // Set active reper ID if any.
    if ( m_bAllowSelection )
      if ( m_iForcedActiveReper != -1 )
        bRepersSrc->SetActiveReper( m_iForcedActiveReper );
      else
        bRepersSrc->SetActiveReper( dp->GetActiveReper() );

    // Connect data source to the pipeline.
    this->SetInputConnection( bRepersSrc->GetOutputPort() );
  }

  // Update modification timestamp.
  this->Modified();
}
