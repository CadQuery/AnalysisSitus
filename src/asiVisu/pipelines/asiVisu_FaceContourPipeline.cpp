//-----------------------------------------------------------------------------
// Created on: 05 September 2016
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
