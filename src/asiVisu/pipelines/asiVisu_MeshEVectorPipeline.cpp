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
#include <asiVisu_MeshEVectorPipeline.h>

// Visualization includes
#include <asiVisu_MeshEVectorFilter.h>
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
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

//! Creates new Pipeline instance.
asiVisu_MeshEVectorPipeline::asiVisu_MeshEVectorPipeline()
  : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                      vtkSmartPointer<vtkActor>::New() )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate filter populating vector data
  vtkSmartPointer<asiVisu_MeshEVectorFilter>
    aVecFilter = vtkSmartPointer<asiVisu_MeshEVectorFilter>::New();

  // Allocate Transformation filter
  vtkSmartPointer<vtkTransformPolyDataFilter>
    aTrsfFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Allocate Glyph filter
  vtkSmartPointer<vtkGlyph3D>
    aGlyphFilter = vtkSmartPointer<vtkGlyph3D>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_EVector, aVecFilter);
  m_filterMap.Bind(Filter_Trsf,    aTrsfFilter);
  m_filterMap.Bind(Filter_Glyph3D, aGlyphFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_EVector) );
  this->append( m_filterMap.Find(Filter_Glyph3D) );
}

//! Sets input data for the pipeline accepting Active Data entities.
//! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
//! DATA.
//! \param theDataProvider [in] Data Provider.
void asiVisu_MeshEVectorPipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_MeshEVectorDataProvider)
    aMeshPrv = Handle(asiVisu_MeshEVectorDataProvider)::DownCast(theDataProvider);

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aMeshPrv->MustExecute( this->GetMTime() ) )
  {
    vtkSmartPointer<asiVisu_MeshSource> aMeshSource = vtkSmartPointer<asiVisu_MeshSource>::New();
    aMeshSource->SetInputMesh( aMeshPrv->GetMeshDS() );

    Handle(HIntArray) anElemIDs = aMeshPrv->GetElementIDs();
    Handle(HRealArray) anElemVectors = aMeshPrv->GetElementVectors();

    // Pass vector array
    asiVisu_MeshEVectorFilter*
      aVecFilter = asiVisu_MeshEVectorFilter::SafeDownCast( m_filterMap.Find(Filter_EVector) );
    aVecFilter->SetVectorArrays( anElemIDs,
                                 anElemVectors,
                                 aMeshPrv->GetMaxModulus() );

    // Transform glyphs
    vtkTransformPolyDataFilter*
      aTrsfFilter = vtkTransformPolyDataFilter::SafeDownCast( m_filterMap.Find(Filter_Trsf) );
    aTrsfFilter->SetInputConnection( asiVisu_MeshResultUtils::GetVectorGlyph()->GetOutputPort() );
    aTrsfFilter->SetTransform( asiVisu_MeshResultUtils::GetVectorGlyphTransform() );

    // Adjust glyph filter
    vtkGlyph3D* aGlyphFilter = vtkGlyph3D::SafeDownCast( m_filterMap.Find(Filter_Glyph3D) );
    aGlyphFilter->SetSourceConnection( aTrsfFilter->GetOutputPort() );
    aGlyphFilter->SetVectorModeToUseVector();
    aGlyphFilter->SetScaleModeToScaleByVector();
    aGlyphFilter->SetColorModeToColorByScale();
    aGlyphFilter->SetInputArrayToProcess(1, 0, 0, 0, ARRNAME_MESH_E_VECTORS);
    aGlyphFilter->SetScaleFactor(1.0); // No global scaling

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
void asiVisu_MeshEVectorPipeline::addToRendererCallback(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer base routine.
void asiVisu_MeshEVectorPipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void asiVisu_MeshEVectorPipeline::updateCallback()
{
  m_mapper->ScalarVisibilityOff();
}
