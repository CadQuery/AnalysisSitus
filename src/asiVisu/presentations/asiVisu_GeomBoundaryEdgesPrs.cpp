//-----------------------------------------------------------------------------
// Created on: 22 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomBoundaryEdgesPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_ShapePipeline.h>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param N [in] Node to create a Presentation for.
asiVisu_GeomBoundaryEdgesPrs::asiVisu_GeomBoundaryEdgesPrs(const Handle(ActAPI_INode)& N) : asiVisu_Prs(N)
{
  // Create Data Providers
  Handle(asiVisu_ShapeDataProvider) DP_convex =
    new asiVisu_ShapeDataProvider( N->GetId(),
                                   ActParamStream() << N->Parameter(asiData_BoundaryEdgesNode::PID_Green) );
  //
  Handle(asiVisu_ShapeDataProvider) DP_concave =
    new asiVisu_ShapeDataProvider( N->GetId(),
                                   ActParamStream() << N->Parameter(asiData_BoundaryEdgesNode::PID_Red) );
  //
  Handle(asiVisu_ShapeDataProvider) DP_undefined =
    new asiVisu_ShapeDataProvider( N->GetId(),
                                   ActParamStream() << N->Parameter(asiData_BoundaryEdgesNode::PID_Ordinary) );

  //---------------------------------------------------------------------------
  // Pipeline for convex edges
  Handle(asiVisu_ShapePipeline) pl_convex = new asiVisu_ShapePipeline(false, false, false, false);
  //
  this->addPipeline        ( Pipeline_Convex, pl_convex );
  this->assignDataProvider ( Pipeline_Convex, DP_convex );
  //
  pl_convex->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl_convex->Actor()->GetProperty()->SetColor(0.1, 0.7, 0.0);
  pl_convex->Mapper()->ScalarVisibilityOff();
  pl_convex->WireframeModeOn();

  //---------------------------------------------------------------------------
  // Pipeline for concave edges
  Handle(asiVisu_ShapePipeline) pl_concave = new asiVisu_ShapePipeline(false, false, false, false);
  //
  this->addPipeline        ( Pipeline_Concave, pl_concave );
  this->assignDataProvider ( Pipeline_Concave, DP_concave );
  //
  pl_concave->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl_concave->Actor()->GetProperty()->SetColor(0.7, 0.0, 0.0);
  pl_concave->Mapper()->ScalarVisibilityOff();
  pl_concave->WireframeModeOn();

  //---------------------------------------------------------------------------
  // Pipeline for undefined edges
  Handle(asiVisu_ShapePipeline) pl_undefined = new asiVisu_ShapePipeline(false, false, false, false);
  //
  this->addPipeline        ( Pipeline_Undefined, pl_undefined );
  this->assignDataProvider ( Pipeline_Undefined, DP_undefined );
  //
  pl_undefined->Actor()->GetProperty()->SetLineWidth(2.0f);
  pl_undefined->Actor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
  pl_undefined->Mapper()->ScalarVisibilityOff();
  pl_undefined->WireframeModeOn();

  //---------------------------------------------------------------------------
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomBoundaryEdgesPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_GeomBoundaryEdgesPrs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomBoundaryEdgesPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomBoundaryEdgesPrs::beforeInitPipelines()
{
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomBoundaryEdgesPrs::afterInitPipelines()
{
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomBoundaryEdgesPrs::beforeUpdatePipelines() const
{
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomBoundaryEdgesPrs::afterUpdatePipelines() const
{
}

//! Callback for highlighting.
void asiVisu_GeomBoundaryEdgesPrs::highlight(vtkRenderer*                   theRenderer,
                                             const asiVisu_PickResult&      thePickRes,
                                             const asiVisu_SelectionNature& theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
void asiVisu_GeomBoundaryEdgesPrs::unHighlight(vtkRenderer*                   theRenderer,
                                               const asiVisu_SelectionNature& theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
void asiVisu_GeomBoundaryEdgesPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}

//! Callback for de-rendering.
void asiVisu_GeomBoundaryEdgesPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}
