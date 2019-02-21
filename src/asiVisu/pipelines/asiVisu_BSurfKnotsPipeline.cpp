//-----------------------------------------------------------------------------
// Created on: 13 August 2018
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
#include <asiVisu_BSurfKnotsPipeline.h>

// asiVisu includes
#include <asiVisu_BSurfKnotsSource.h>
#include <asiVisu_SurfaceDataProvider.h>
#include <asiVisu_NodeInfo.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_BSurfKnotsPipeline::asiVisu_BSurfKnotsPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet ( false )
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param[in] DP Data Provider.
void asiVisu_BSurfKnotsPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_SurfaceDataProvider)
    dp = Handle(asiVisu_SurfaceDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(Standard_Type) surfType = dp->GetSurfaceType();
  //
  if ( surfType.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( dp->MustExecute( this->GetMTime() ) )
  {
    // Bind to Node
    asiVisu_NodeInfo::Store( dp->GetNodeID(), this->Actor() );

    // Access surface
    double uMin, uMax, vMin, vMax;
    Handle(Geom_Surface) surf = dp->GetSurface(uMin, uMax, vMin, vMax);

    // B-surface knots isos
    if ( !surf.IsNull() && surf->IsKind( STANDARD_TYPE(Geom_BSplineSurface) ) )
    {
      Handle(Geom_BSplineSurface) bsurf = Handle(Geom_BSplineSurface)::DownCast(surf);
      //
      vtkSmartPointer<asiVisu_BSurfKnotsSource>
        bisosSrc = vtkSmartPointer<asiVisu_BSurfKnotsSource>::New();
      //
      bisosSrc->SetInputSurface(bsurf);

      // Chain pipeline
      this->SetInputConnection( bisosSrc->GetOutputPort() );
    }
    else
    {
      // Pass empty data set in order to have valid pipeline
      vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
      this->SetInputData(dummyDS);
    }
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_BSurfKnotsPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_BSurfKnotsPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_BSurfKnotsPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = asiVisu_Utils::InitKnotsIsosLookupTable();
    asiVisu_Utils::InitMapper(m_mapper, aLookup, ARRNAME_ISOSMULTS_SCALARS);
    m_bMapperColorsSet = true;
  }
}
