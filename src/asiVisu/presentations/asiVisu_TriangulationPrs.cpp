//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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
#include <asiVisu_TriangulationPrs.h>

// asiVisu includes
#include <asiVisu_MeshDisplayMode.h>
#include <asiVisu_TriangulationDataProvider.h>
#include <asiVisu_TriangulationLinksPipeline.h>
#include <asiVisu_TriangulationPipeline.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkCellTreeLocator.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Triangulation Node.
//! \param[in] N Triangulation Node to create a Presentation for.
asiVisu_TriangulationPrs::asiVisu_TriangulationPrs(const Handle(ActAPI_INode)& N) : asiVisu_Prs(N)
{
  Handle(asiData_TriangulationNode)
    triangulationNode = Handle(asiData_TriangulationNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_TriangulationDataProvider)
    dp = new asiVisu_TriangulationDataProvider(triangulationNode);

  // Main pipeline
  Handle(asiVisu_TriangulationPipeline) pl = new asiVisu_TriangulationPipeline();
  //
  this->addPipeline        ( Pipeline_Triangulation, pl );
  this->assignDataProvider ( Pipeline_Triangulation, dp );

  // Set point size and line width
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  // !!! For performance reasons !!!
  pl->Actor()->SetPickable(0);

  /* =========================
   *  Pipeline for mesh links
   * ========================= */

  // Create pipeline for links
  Handle(asiVisu_TriangulationLinksPipeline)
    contour_pl = new asiVisu_TriangulationLinksPipeline( pl->GetSource() );

  // Adjust props
  contour_pl->Actor()->GetProperty()->SetOpacity(0.5);
  contour_pl->Actor()->GetProperty()->SetLineWidth(1.0f);
  contour_pl->Actor()->SetPickable(0);
  //
  this->addPipeline        ( Pipeline_TriangulationLinks, contour_pl );
  this->assignDataProvider ( Pipeline_TriangulationLinks, dp );

  /* =========================
   *  Pipeline for mesh nodes
   * ========================= */

  // Create pipeline for links
  Handle(asiVisu_TriangulationLinksPipeline)
    nodes_pl = new asiVisu_TriangulationLinksPipeline( pl->GetSource() );
  //
  nodes_pl->GetDisplayModeFilter()->SetDisplayMode(MeshDisplayMode_Vertices);

  // Adjust props
  nodes_pl->Actor()->GetProperty()->SetOpacity(0.5);
  nodes_pl->Actor()->GetProperty()->SetPointSize(5.0f);
  nodes_pl->Actor()->SetPickable(0);
  nodes_pl->Actor()->SetVisibility(0);
  //
  this->addPipeline        ( Pipeline_TriangulationNodes, nodes_pl );
  this->assignDataProvider ( Pipeline_TriangulationNodes, dp );

  // Resolve coincident topology between shaded facets and border links
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//-----------------------------------------------------------------------------

//! Factory method for Node's Presentation.
//! \param[in] N Triangulation Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_TriangulationPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_TriangulationPrs(N);
}

//-----------------------------------------------------------------------------

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_TriangulationPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Sets diagnostic tools for the presentation.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
void asiVisu_TriangulationPrs::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                                  ActAPI_PlotterEntry  plotter)
{
  asiVisu_Prs::SetDiagnosticTools(progress, plotter);

  Handle(asiVisu_TriangulationPipeline)
    pl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );

  if ( pl.IsNull() )
    return;

  pl->SetDiagnosticTools(m_progress, m_plotter);
}


//-----------------------------------------------------------------------------

//! Enables visualization of vertices.
void asiVisu_TriangulationPrs::VerticesOn() const
{
  Handle(asiVisu_TriangulationLinksPipeline)
    pl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationNodes) );

  if ( pl.IsNull() )
    return;

  // Configure display mode
  pl->Actor()->SetVisibility(1);
}

//-----------------------------------------------------------------------------

//! Disables visualization of vertices.
void asiVisu_TriangulationPrs::VerticesOff() const
{
  Handle(asiVisu_TriangulationLinksPipeline)
    pl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationNodes) );

  if ( pl.IsNull() )
    return;

  // Configure display mode
  pl->Actor()->SetVisibility(0);
}

//-----------------------------------------------------------------------------

void asiVisu_TriangulationPrs::InitializePicker(const vtkSmartPointer<vtkCellPicker>& picker) const
{
  picker->RemoveAllLocators();

  // Set octee locators to speed up cell picking
  if ( this->MainActor() )
  {
    vtkSmartPointer<vtkCellTreeLocator>
      facetLocator = vtkSmartPointer<vtkCellTreeLocator>::New();
    //
    facetLocator->SetDataSet( this->MainActor()->GetMapper()->GetInput() );
    facetLocator->AutomaticOn();
    facetLocator->BuildLocator();
    //
    picker->AddLocator(facetLocator);
  }

  // NOTICE: we do not apply cell locator for selection of edges as it seems
  //         to work weird (at least for vtkCellTreeLocator)
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_TriangulationPrs::beforeInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_TriangulationPrs::afterInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_TriangulationPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_TriangulationPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for highlighting.
//! \param[in] renderer  renderer.
//! \param[in] pickRes   picking results.
//! \param[in] selNature selection nature (picking or detecting).
void asiVisu_TriangulationPrs::highlight(vtkRenderer*                        renderer,
                                         const Handle(asiVisu_PickerResult)& pickRes,
                                         const asiVisu_SelectionNature       selNature) const
{
  asiVisu_NotUsed(renderer);

  // Can react on cell picking only.
  Handle(asiVisu_CellPickerResult)
    cellPickRes = Handle(asiVisu_CellPickerResult)::DownCast(pickRes);
  //
  if ( cellPickRes.IsNull() )
    return;

  const TColStd_PackedMapOfInteger& cellIds = cellPickRes->GetPickedCellIds();
  const TColStd_PackedMapOfInteger& elemIds = cellPickRes->GetPickedElementIds();

  // There is one peculiarity in selection mechanism for mesh elements. To
  // save memory, we do not store element IDs as pedigrees or global IDs in
  // vtkPolyData. It is enough to have direct VTK cell IDs as element IDs.
  // However, if we keep pedigrees and globals empty, the presentation
  // manager will not populate the set of picked element IDs. Therefore,
  // we have to handle it here by passing cell IDs as picked element IDs to
  // the underlying pipelines.

  // #################################################
  // FACET selection
  if ( cellPickRes->GetPickedActor() == this->MainActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked MAIN actor" << std::endl;
#endif

    Handle(asiVisu_TriangulationPipeline)
      mainPl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );

    mainPl->SetPickedElements( cellIds, selNature );

    // Diagnostic dump.
    if ( selNature == SelectionNature_Persistent )
      for ( TColStd_MapIteratorOfPackedMapOfInteger eit(elemIds); eit.More(); eit.Next() )
        m_progress.SendLogMessage( LogNotice(Normal) << "Picked face with ID %1." << eit.Key() );
  }
  // #################################################
  // LINK selection
  else if ( cellPickRes->GetPickedActor() == this->ContourActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked CONTOUR actor" << std::endl;
#endif

    Handle(asiVisu_TriangulationLinksPipeline)
      contourPl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationLinks) );

    contourPl->SetPickedElements( cellIds, selNature );

    // Diagnostic dump.
    if ( selNature == SelectionNature_Persistent )
      for ( TColStd_MapIteratorOfPackedMapOfInteger eit(elemIds); eit.More(); eit.Next() )
        m_progress.SendLogMessage( LogNotice(Normal) << "Picked link with ID %1." << eit.Key() );
  }
  // #################################################
  // VERTEX selection
  else if ( cellPickRes->GetPickedActor() == this->NodesActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked NODES actor" << std::endl;
#endif

    Handle(asiVisu_TriangulationLinksPipeline)
      nodesPl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationNodes) );

    nodesPl->SetPickedElements( cellIds, selNature );

    // Diagnostic dump.
    if ( selNature == SelectionNature_Persistent )
      for ( TColStd_MapIteratorOfPackedMapOfInteger eit(elemIds); eit.More(); eit.Next() )
        m_progress.SendLogMessage( LogNotice(Normal) << "Picked node with ID %1." << eit.Key() );
  }
}

//-----------------------------------------------------------------------------

//! Callback for highlighting reset.
//! \param[in] renderer  renderer.
//! \param[in] selNature selection nature (picking or detecting).
void asiVisu_TriangulationPrs::unHighlight(vtkRenderer*                  renderer,
                                           const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);

  Handle(asiVisu_TriangulationPipeline)
    mainPl = Handle(asiVisu_TriangulationPipeline)::DownCast( this->GetPipeline(Pipeline_Triangulation) );
  //
  Handle(asiVisu_TriangulationLinksPipeline)
    contourPl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationLinks) );
  //
  Handle(asiVisu_TriangulationLinksPipeline)
    nodesPl = Handle(asiVisu_TriangulationLinksPipeline)::DownCast( this->GetPipeline(Pipeline_TriangulationNodes) );

  mainPl->ResetPickedElements(selNature);
  contourPl->ResetPickedElements(selNature);
  nodesPl->ResetPickedElements(selNature);
}

//-----------------------------------------------------------------------------

//! Callback for rendering.
//! \param[in] renderer renderer.
void asiVisu_TriangulationPrs::renderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
//! \param[in] renderer renderer.
void asiVisu_TriangulationPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  // Do nothing...
}
