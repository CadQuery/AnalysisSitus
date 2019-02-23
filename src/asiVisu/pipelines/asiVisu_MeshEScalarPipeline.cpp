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
#include <asiVisu_MeshEScalarPipeline.h>

// Visualization includes
#include <asiVisu_MeshEScalarFilter.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_MeshSource.h>
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
asiVisu_MeshEScalarPipeline::asiVisu_MeshEScalarPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                   vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating scalar arrays
  vtkSmartPointer<asiVisu_MeshEScalarFilter>
    aScFilter = vtkSmartPointer<asiVisu_MeshEScalarFilter>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_EScalar, aScFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_EScalar) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void asiVisu_MeshEScalarPipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_MeshEScalarDataProvider)
    aMeshPrv = Handle(asiVisu_MeshEScalarDataProvider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    asiVisu_MeshEScalarFilter*
      aScFilter = asiVisu_MeshEScalarFilter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );

    aScFilter->SetScalarArrays( aMeshPrv->GetElementIDs(),
                                aMeshPrv->GetElementScalars() );

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
void asiVisu_MeshEScalarPipeline::addToRendererCallback(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToFlat();
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshEScalarPipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshEScalarPipeline::updateCallback()
{
  asiVisu_MeshEScalarFilter*
    aScFilter = asiVisu_MeshEScalarFilter::SafeDownCast( m_filterMap.Find(Filter_EScalar) );

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

  asiVisu_MeshResultUtils::InitCellScalarMapper(m_mapper, ARRNAME_MESH_E_SCALARS,
                                               aMinScalar, aMaxScalar,
                                               doScalarInterpolation);
}
