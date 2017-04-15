//-----------------------------------------------------------------------------
// Created on: 15 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapePipeline.h>

// Visualization includes
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiVisu_ShapePipeline::asiVisu_ShapePipeline(bool isScalarMode)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bIsScalarModeOn  ( isScalarMode ),
  m_bMapperColorsSet ( false )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Initialize Data Source
  m_source = vtkSmartPointer<asiVisu_ShapeRobustSource>::New();

  // Filter for normals
  m_normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  // Display mode filter
  m_dmFilter = vtkSmartPointer<asiVisu_DisplayModeFilter>::New();
  m_dmFilter->SetDisplayMode(DisplayMode_Shaded);

  // Set line width
  this->Actor()->GetProperty()->SetLineWidth(1);
  this->Actor()->GetProperty()->SetPointSize(8);
  //
  asiVisu_Utils::ApplyLightingRules( this->Actor() );

  // Compose pipeline
  this->append(m_dmFilter);
  this->append(m_normalsFilter);
}

//-----------------------------------------------------------------------------

void asiVisu_ShapePipeline::SetDiagnosticTools(ActAPI_ProgressEntry progress,
                                               ActAPI_PlotterEntry  plotter)
{
  m_progress = progress;
  m_plotter  = plotter;
  //
  m_source->SetDiagnosticTools(m_progress, m_plotter);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param dataProvider [in] Data Provider.
void asiVisu_ShapePipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
{
  Handle(asiVisu_ShapeDataProvider)
    DP = Handle(asiVisu_ShapeDataProvider)::DownCast(dataProvider);

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

  /* ====================
   *  Configure pipeline
   * ==================== */

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    // Get shape
    TopoDS_Shape shape = DP->GetShape();

    // Configure data source
    m_source->SetShape(shape);

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_ShapePipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_ShapePipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_ShapePipeline::callback_update()
{
  if ( m_bIsScalarModeOn && !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = true;
  }

  if ( m_bIsScalarModeOn )
    this->Mapper()->ScalarVisibilityOn();
  else
    this->Mapper()->ScalarVisibilityOff();
}
