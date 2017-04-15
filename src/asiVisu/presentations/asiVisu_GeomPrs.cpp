//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomPrs.h>

// asiVisu includes
#include <asiVisu_DisplayMode.h>
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartEdgesPipeline.h>
#include <asiVisu_PartPipeline.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkCellTreeLocator.h>
#include <vtkIdTypeArray.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

//! Convert integer value to a color.
//! \param color [in] integer value.
//! \return converted value
static QColor IntToColor(const int color)
{
  unsigned char red   = ( color >> 16 ) & 0xFF;
  unsigned char green = ( color >>  8 ) & 0xFF;
  unsigned char blue  =   color         & 0xFF;
  return QColor(red, green, blue);
}

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Part Node.
//! \param N [in] Part Node to create a Presentation for.
asiVisu_GeomPrs::asiVisu_GeomPrs(const Handle(ActAPI_INode)& N) : asiVisu_Prs(N)
{
  Handle(asiData_PartNode) partNode = Handle(asiData_PartNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_PartDataProvider) dp = new asiVisu_PartDataProvider(partNode);

  // Main pipeline
  Handle(asiVisu_PartPipeline) pl = new asiVisu_PartPipeline();
  //
  this->addPipeline        ( Pipeline_Main, pl );
  this->assignDataProvider ( Pipeline_Main, dp );

  // Set point size and line width
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  /* ====================
   *  Pipeline for edges
   * ==================== */

  // Create pipeline for edges
  Handle(asiVisu_PartEdgesPipeline)
    contour_pl = new asiVisu_PartEdgesPipeline( pl->GetSource() );

  // Adjust props
  contour_pl->Actor()->GetProperty()->SetOpacity(0.5);
  contour_pl->Actor()->GetProperty()->SetLineWidth(1.5f);
  contour_pl->Actor()->SetPickable(0);
  //
  this->addPipeline        ( Pipeline_Contour, contour_pl );
  this->assignDataProvider ( Pipeline_Contour, dp );

  // Resolve coincident topology between shaded facets and border links
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param N [in] Geometry Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_GeomPrs(N);
}

//-----------------------------------------------------------------------------

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Sets diagnostic tools for the presentation.
//! \param progress [in] progress notifier.
//! \param plotter  [in] imperative plotter.
void asiVisu_GeomPrs::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
{
  asiVisu_Prs::SetDiagnosticTools(progress, plotter);

  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Enables visualization of vertices.
void asiVisu_GeomPrs::VerticesOn() const
{
  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  // Configure display mode
  pl->GetDisplayModeFilter()->AddPrimitive(ShapePrimitive_SharedVertex);
}

//-----------------------------------------------------------------------------

//! Disables visualization of vertices.
void asiVisu_GeomPrs::VerticesOff() const
{
  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  // Configure display mode
  pl->GetDisplayModeFilter()->RemovePrimitive(ShapePrimitive_SharedVertex);
}

//-----------------------------------------------------------------------------

//! Sets custom color for the geometry.
//! \param color [in] color to set.
void asiVisu_GeomPrs::DoColor(const QColor& color) const
{
  if ( !color.isValid() )
    return;

  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->Mapper()->ScalarVisibilityOff();
  pl->Actor()->GetProperty()->SetColor( color.redF(),
                                        color.greenF(),
                                        color.blueF() );
}

//-----------------------------------------------------------------------------

//! Unsets custom color for the geometry.
void asiVisu_GeomPrs::DoUnColor() const
{
  Handle(asiVisu_PartPipeline)
    pl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  if ( pl.IsNull() )
    return;

  pl->Mapper()->ScalarVisibilityOn();
}

//-----------------------------------------------------------------------------

void asiVisu_GeomPrs::InitializePicker(const vtkSmartPointer<vtkCellPicker>& picker) const
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
void asiVisu_GeomPrs::beforeInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomPrs::afterInitPipelines()
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for highlighting.
//! \param renderer  [in] renderer.
//! \param pickRes   [in] picking results.
//! \param selNature [in] selection nature (picking or detecting).
void asiVisu_GeomPrs::highlight(vtkRenderer*                  renderer,
                                const asiVisu_PickResult&     pickRes,
                                const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);

  // #################################################
  // FACE selection
  if ( pickRes.GetPickedActor() == this->MainActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked MAIN actor" << std::endl;
#endif

    Handle(asiVisu_PartPipeline)
      mainPl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

    mainPl->SetPickedElements( pickRes.GetPickedElementIds(), selNature );
  }
  // #################################################
  // EDGE selection
  else if ( pickRes.GetPickedActor() == this->ContourActor() )
  {
#if defined COUT_DEBUG
    std::cout << "Picked CONTOUR actor" << std::endl;
#endif

    Handle(asiVisu_PartEdgesPipeline)
      contourPl = Handle(asiVisu_PartEdgesPipeline)::DownCast( this->GetPipeline(Pipeline_Contour) );

    contourPl->SetPickedElements( pickRes.GetPickedElementIds(), selNature );
  }
}

//-----------------------------------------------------------------------------

//! Callback for highlighting reset.
//! \param renderer  [in] renderer.
//! \param selNature [in] selection nature (picking or detecting).
void asiVisu_GeomPrs::unHighlight(vtkRenderer*                  renderer,
                                  const asiVisu_SelectionNature selNature) const
{
  asiVisu_NotUsed(renderer);

  Handle(asiVisu_PartPipeline)
    mainPl = Handle(asiVisu_PartPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  Handle(asiVisu_PartEdgesPipeline)
    contourPl = Handle(asiVisu_PartEdgesPipeline)::DownCast( this->GetPipeline(Pipeline_Contour) );

  mainPl->ResetPickedElements(selNature);
  contourPl->ResetPickedElements(selNature);
}

//-----------------------------------------------------------------------------

//! Callback for rendering.
//! \param renderer [in] renderer.
void asiVisu_GeomPrs::renderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  // Do nothing...
}

//-----------------------------------------------------------------------------

//! Callback for de-rendering.
//! \param renderer [in] renderer.
void asiVisu_GeomPrs::deRenderPipelines(vtkRenderer* renderer) const
{
  asiVisu_NotUsed(renderer);

  // Do nothing...
}
