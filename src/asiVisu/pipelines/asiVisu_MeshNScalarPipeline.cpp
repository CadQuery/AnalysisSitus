//-----------------------------------------------------------------------------
// Created on: 13 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
#include <asiVisu_MeshNScalarPipeline.h>

// asiVisu includes
#include <asiVisu_MeshNScalarFilter.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_MeshUtils.h>
#include <asiVisu_NodeInfo.h>
#include <asiVisu_TriangulationSource.h>

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

void getColorCorrespondingToValue(double val,
                                  double min,
                                  double max,
                                  double toler,
                                  double& r,
                                  double& g,
                                  double& b)

{
  static const int numColorNodes = 3;
  double color[numColorNodes][3] =
  {
    0.0, 0.0, 1.0, // blue
    1.0, 1.0, 1.0, // white
    1.0, 0.0, 0.0  // red
  };

  if ( val > -toler && val < toler )
  {
    // Use middle color for values in the safety range.
    r = color[1][0];
    g = color[1][1];
    b = color[1][2];
  }
  else if ( val < -toler )
  {
    double currFraction = (val - min) / (-toler - min);

    r = color[0][0] * (1.0 - currFraction) + color[1][0] * currFraction;
    g = color[0][1] * (1.0 - currFraction) + color[1][1] * currFraction;
    b = color[0][2] * (1.0 - currFraction) + color[1][2] * currFraction;
  }
  else if ( val > toler )
  {
    double currFraction = (val - toler) / (max - toler);

    r = color[1][0] * (1.0 - currFraction) + color[2][0] * currFraction;
    g = color[1][1] * (1.0 - currFraction) + color[2][1] * currFraction;
    b = color[1][2] * (1.0 - currFraction) + color[2][2] * currFraction;
  }
}

//-----------------------------------------------------------------------------

asiVisu_MeshNScalarPipeline::asiVisu_MeshNScalarPipeline()
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
  m_fToler(0.)
{
  /* Prepare custom filters */

  // Allocate filter populating scalar arrays.
  vtkSmartPointer<asiVisu_MeshNScalarFilter>
    scFilter = vtkSmartPointer<asiVisu_MeshNScalarFilter>::New();

  // Allocate Normals Calculation filter.
  vtkSmartPointer<vtkPolyDataNormals>
    normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  /* Register custom filters */

  m_filterMap.Bind(Filter_NScalar, scFilter);
  m_filterMap.Bind(Filter_Normals, normalsFilter);

  // Append custom filters to the pipeline.
  this->append( m_filterMap.Find(Filter_NScalar) );
  this->append( m_filterMap.Find(Filter_Normals) );
}

//-----------------------------------------------------------------------------

void asiVisu_MeshNScalarPipeline::SetInput(const Handle(asiVisu_DataProvider)& dp)
{
  Handle(asiVisu_MeshNScalarDataProvider)
    meshDp = Handle(asiVisu_MeshNScalarDataProvider)::DownCast(dp);

  // Lazy update.
  if ( meshDp->MustExecute( this->GetMTime() ) )
  {
    // Prepare source.
    vtkSmartPointer<asiVisu_TriangulationSource>
      trisSource = vtkSmartPointer<asiVisu_TriangulationSource>::New();
    //
    trisSource->CollectEdgesModeOff();
    trisSource->CollectVerticesModeOff();
    trisSource->SetInputTriangulation( meshDp->GetTriangulation() );

    // Initialize safety range.
    m_fToler = meshDp->GetTolerance();

    // Initialize scalars filter.
    asiVisu_MeshNScalarFilter*
      scFilter = asiVisu_MeshNScalarFilter::SafeDownCast( m_filterMap.Find(Filter_NScalar) );
    //
    scFilter->SetScalarArrays( meshDp->GetNodeIDs(),
                               meshDp->GetNodeScalars() );

    // Complete pipeline.
    this->SetInputConnection( trisSource->GetOutputPort() );

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object.
    asiVisu_NodeInfo::Store( meshDp->GetNodeID(), this->Actor() );
  }

  // Update modification timestamp.
  this->Modified();
}

//-----------------------------------------------------------------------------

void asiVisu_MeshNScalarPipeline::initLookupTable()
{
  // Get scalar filter to access the scalar range.
  asiVisu_MeshNScalarFilter*
    scFilter = asiVisu_MeshNScalarFilter::SafeDownCast( m_filterMap.Find(Filter_NScalar) );
  //
  scFilter->Update();

  // Get scalar range.
  const double minScalar = scFilter->GetMinScalar();
  const double maxScalar = scFilter->GetMaxScalar();
  const double range     = maxScalar - minScalar;

  // Extra variables.
  const int numColors = 256;

  // Build and initialize lookup table.
  m_lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  //
  m_lookupTable->SetTableRange(minScalar, maxScalar);
  m_lookupTable->SetScaleToLinear();
  m_lookupTable->SetNumberOfTableValues(numColors + 1);

  // Populate table.
  double r, g, b;
  for ( int i = 0; i < numColors + 1; i++ )
  {
    const double val = minScalar + ( (double) i / numColors ) * range;
    getColorCorrespondingToValue(val, minScalar, maxScalar, m_fToler, r, g, b);
    m_lookupTable->SetTableValue(i, r, g, b);
  }
}

//-----------------------------------------------------------------------------

void asiVisu_MeshNScalarPipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToGouraud();
}

//-----------------------------------------------------------------------------

void asiVisu_MeshNScalarPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

void asiVisu_MeshNScalarPipeline::callback_update()
{
  // Initialize lookup table.
  this->initLookupTable();

  // Bind lookup table to mapper.
  asiVisu_MeshResultUtils::InitPointScalarMapper(m_mapper, m_lookupTable,
                                                 ARRNAME_MESH_N_SCALARS,
                                                 true);
}
