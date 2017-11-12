//-----------------------------------------------------------------------------
// Created on: 05 September 2016
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
#include <asiVisu_FaceContourPipeline.h>

// asiVisu includes
#include <asiVisu_FaceContourDataProvider.h>
#include <asiVisu_PDomainSource.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <BRep_Tool.hxx>

// VTK includes
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Face Contour Pipeline initialized by default VTK mapper and actor.
asiVisu_FaceContourPipeline::asiVisu_FaceContourPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet(false)
{
  this->Actor()->GetProperty()->SetLineWidth(3.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_FaceContourPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_FaceContourDataProvider)
    faceProvider = Handle(asiVisu_FaceContourDataProvider)::DownCast(DP);

  /* =================
   *  Validate inputs
   * ================= */

  TopoDS_Face face = faceProvider->GetFace();
  if ( face.IsNull() )
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

  if ( faceProvider->MustExecute( this->GetMTime() ) )
  {
    // Append filter
    vtkSmartPointer<asiVisu_PDomainSource>
      source = vtkSmartPointer<asiVisu_PDomainSource>::New();
    //
    source->SetFace(face);
    source->Set3DCurveModeOn(); // Enable 3D mode

    // Get norm at the middle point
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

    // Get natural bounds
    double uMin, uMax, vMin, vMax;
    surf->Bounds(uMin, uMax, vMin, vMax);

    // Calculate norm
    gp_Pnt sP;
    gp_Vec sD1U, sD1V;
    surf->D1( (uMin + uMax)*0.5, (vMin + vMax)*0.5, sP, sD1U, sD1V );
    //
    gp_Vec snorm = sD1U^sD1V;

    // Set plane for tips
    source->SetTipNorm(snorm);

    // Chain pipeline
    this->SetInputConnection( source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_FaceContourPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_FaceContourPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_FaceContourPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> lookup = asiVisu_Utils::InitDomainLookupTable();
    asiVisu_Utils::InitMapper(m_mapper, lookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
