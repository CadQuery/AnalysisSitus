//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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
#include <asiVisu_MeshNScalarPipeline.h>

// Visualization includes
#include <asiVisu_MeshNScalarFilter.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_MeshSource.h>
#include <asiVisu_MeshUtils.h>
#include <asiVisu_NodeInfo.h>

// Active Data includes
#include <ActData_MeshParameter.h>
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
asiVisu_MeshNScalarPipeline::asiVisu_MeshNScalarPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating scalar arrays
  vtkSmartPointer<asiVisu_MeshNScalarFilter>
    aScFilter = vtkSmartPointer<asiVisu_MeshNScalarFilter>::New();

  // Allocate Normals Calculation filter
  vtkSmartPointer<vtkPolyDataNormals>
    aNormalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_NScalar, aScFilter);
  m_filterMap.Bind(Filter_Normals, aNormalsFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_NScalar) );
  this->append( m_filterMap.Find(Filter_Normals) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void asiVisu_MeshNScalarPipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_MeshNScalarDataProvider)
    aMeshPrv = Handle(asiVisu_MeshNScalarDataProvider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    asiVisu_MeshNScalarFilter*
      aScFilter = asiVisu_MeshNScalarFilter::SafeDownCast( m_filterMap.Find(Filter_NScalar) );

    aScFilter->SetScalarArrays( aMeshPrv->GetNodeIDs(),
                                aMeshPrv->GetNodeScalars() );

    // Complete pipeline
    this->SetInputConnection( aMeshSource->GetOutputPort() );

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    asiVisu_NodeInfo::Store( aMeshPrv->GetNodeID(), this->Actor() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_MeshNScalarPipeline::addToRendererCallback(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshNScalarPipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshNScalarPipeline::updateCallback()
{
  asiVisu_MeshNScalarFilter*
    aScFilter = asiVisu_MeshNScalarFilter::SafeDownCast( m_filterMap.Find(Filter_NScalar) );
  aScFilter->Update();
  double aMinScalar = aScFilter->GetMinScalar(),
                aMaxScalar = aScFilter->GetMaxScalar();

  if ( Abs(aMinScalar) == VTK_FLOAT_MAX && Abs(aMaxScalar) == VTK_FLOAT_MAX )
    aMinScalar = aMaxScalar = 0.0;

  bool doScalarInterpolation;
  if ( Abs(aMaxScalar - aMinScalar) < Precision::Confusion() )
    doScalarInterpolation = false;
  else
    doScalarInterpolation = true;

  asiVisu_MeshResultUtils::InitPointScalarMapper(m_mapper, ARRNAME_MESH_N_SCALARS,
                                                aMinScalar, aMaxScalar,
                                                doScalarInterpolation);
}
