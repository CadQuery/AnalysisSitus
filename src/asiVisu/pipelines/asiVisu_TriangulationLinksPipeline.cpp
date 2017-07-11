//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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
#include <asiVisu_TriangulationLinksPipeline.h>

// asiVisu includes
#include <asiVisu_TriangulationDataProvider.h>
#include <asiVisu_TriangulationNodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

asiVisu_TriangulationLinksPipeline::asiVisu_TriangulationLinksPipeline(const vtkSmartPointer<asiVisu_TriangulationSource>& source)
//
: asiVisu_TriangulationPipelineBase(source)
{
  m_dmFilter->SetDisplayMode(MeshDisplayMode_Wireframe);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param dataProvider [in] Data Provider.
void asiVisu_TriangulationLinksPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_TriangulationDataProvider)
    DP = Handle(asiVisu_TriangulationDataProvider)::DownCast(dataProvider);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(Poly_Triangulation) triangulation = DP->GetTriangulation();
  //
  if ( triangulation.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> dummyData = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyData);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Clear cached data which is by design actual for the current state of
    // source only. The source changes, so the cache needs nullification
    this->clearCache();

    // Bind to a Data Node using information key
    asiVisu_TriangulationNodeInfo::Store( DP->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}
