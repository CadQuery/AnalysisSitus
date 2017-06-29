//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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
#include <asiVisu_MeshPipeline.h>

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshSource.h>
#include <asiVisu_MeshUtils.h>
#include <asiVisu_NodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Mesh Pipeline
//-----------------------------------------------------------------------------

//! Creates new Mesh Pipeline instance.
asiVisu_MeshPipeline::asiVisu_MeshPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() )
{
  this->EmptyGroupForAllModeOn();

  /* ===========================
   *  Initialize custom filters
   * =========================== */

  asiVisu_Utils::ApplyLightingRules( this->Actor() );
}

//! Sets input data for the pipeline.
//! This method performs translation of DOMAIN data to VTK polygonal data.
//! \param dataProvider [in] Data Provider.
void asiVisu_MeshPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_MeshDataProvider)
    aMeshPrv = Handle(asiVisu_MeshDataProvider)::DownCast(dataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );
    if ( m_bIsEmptyGroupForAll )
      aMeshSource->EmptyGroupForAllModeOn();
    else
      aMeshSource->EmptyGroupForAllModeOff();

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    asiVisu_NodeInfo::Store( aMeshPrv->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( aMeshSource->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Switches ON inverse mode of filtering.
void asiVisu_MeshPipeline::EmptyGroupForAllModeOn()
{
  m_bIsEmptyGroupForAll = true;
}

//! Switches OFF inverse mode of filtering.
void asiVisu_MeshPipeline::EmptyGroupForAllModeOff()
{
  m_bIsEmptyGroupForAll = false;
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_MeshPipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetPointSize( asiVisu_MeshUtils::DefaultPointSize() );
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshPipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    asiVisu_MeshUtils::InitMapper(m_mapper, ARRNAME_MESH_ITEM_TYPE);
    m_bMapperColorsSet = true;
  }
}
