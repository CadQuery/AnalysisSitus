//-----------------------------------------------------------------------------
// Created on: 06 April 2017
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
#include <asiVisu_PartEdgesPipeline.h>

// asiVisu includes
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

asiVisu_PartEdgesPipeline::asiVisu_PartEdgesPipeline(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source)
//
: asiVisu_PartPipelineBase (source),
  m_fPartRed               (0.),
  m_fPartGreen             (0.),
  m_fPartBlue              (0.)
{
  m_dmFilter->SetDisplayMode(ShapeDisplayMode_Wireframe);

  // Transformation filter.
  m_tranformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  //
  this->append(m_tranformFilter);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param dataProvider [in] Data Provider.
void asiVisu_PartEdgesPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_PartDataProvider)
    DP = Handle(asiVisu_PartDataProvider)::DownCast(dataProvider);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape shape = DP->GetShape();
  if ( shape.IsNull() )
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

  // Set transformation from the data provider.
  m_tranformFilter->SetTransform( DP->GetTransformation() );

  // Pass color source.
  m_source->SetColorSource( DP->PrepareColorSource(), false );

  // Update part-wise colors.
  DP->GetColor(m_fPartRed, m_fPartGreen, m_fPartBlue);

  // Update use of scalars flag.
  m_bScalarsOn = DP->HasScalars();

  // Lazy update.
  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Clear cached data which is by design actual for the current state of
    // source only. The source changes, so the cache needs nullification
    this->clearCache();

    // Bind to a Data Node using information key
    asiVisu_PartNodeInfo::Store( DP->GetNodeID(), this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_PartEdgesPipeline::callback_update()
{
  if ( m_bScalarsOn )
  {
    this->updateColors();

    // Get extra scalars.
    NCollection_DataMap<int, int> extraScalars;
    m_source->GetExtraColorsScalars(extraScalars);
    //
    const int lastUnusedScalar = m_source->GetLastUnusedScalar();

    // Initialize mapper and a lookup table to have not only the default
    // colors for boundary elements, but also the custom colors. The part
    // color is passed as the reference one for choosing scalar colors.
    asiVisu_Utils::InitShapeMapper(m_mapper.Get(),
                                   m_fPartRed,
                                   m_fPartGreen,
                                   m_fPartBlue,
                                   extraScalars,
                                   lastUnusedScalar);

    if ( !m_bMapperColorsSet )
      m_bMapperColorsSet = true;
  }
  else
  {
    m_mapper->ScalarVisibilityOff();
    m_mapper->Update();
  }
}
