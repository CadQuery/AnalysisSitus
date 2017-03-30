//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartPipeline.h>

// Visualization includes
#include <asiVisu_PartDataProvider.h>
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Shape Pipeline initialized by default VTK mapper and actor.
//! \param isOCCTColorScheme [in] indicates whether to use native OCCT
//!                               color scheme for rendering of OCCT shapes.
//! \param isBound2Node      [in] indicates whether to bind Node ID to actor.
asiVisu_PartPipeline::asiVisu_PartPipeline(const bool isOCCTColorScheme,
                                           const bool isBound2Node)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bOCCTColorScheme ( isOCCTColorScheme ), // Native OCCT color scheme (as in Draw)
  m_bIsBound2Node    ( isBound2Node ),
  m_bMapperColorsSet ( false )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Initialize Data Source
  m_source = vtkSmartPointer<asiVisu_ShapeRobustSource>::New();
  ////
  //m_DS->FastTransformModeOn(); // Use optimized approach to isotropic transformations
  //m_DS->SetFaceter(asiVisu_ShapeDataSource::Faceter_Robust);

  // Set line width
  this->Actor()->GetProperty()->SetLineWidth(1);
  this->Actor()->GetProperty()->SetPointSize(8);
  //
  asiVisu_Utils::ApplyLightingRules( this->Actor() );
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param dataProvider [in] Data Provider.
void asiVisu_PartPipeline::SetInput(const Handle(asiVisu_DataProvider)& dataProvider)
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

  if ( DP->MustExecute( this->GetMTime() ) )
  {
    m_source->SetAAG( DP->GetAAG() );

    //static int ShapeID = 0; ++ShapeID;
    //Handle(IVtkOCC_Shape) aShapeIntoVtk = new IVtkOCC_Shape(aShape);
    //aShapeIntoVtk->SetId(ShapeID);

    //// Re-use existing Data Source to benefit from its lightweight
    //// transformation capabilities
    //m_DS->SetShape(aShapeIntoVtk);

    //// Bind actor to owning Node ID. Thus we set back reference from VTK
    //// entity to data object
    //if ( m_bIsBound2Node )
    //  asiVisu_PartNodeInfo::Store( DP->GetNodeID(), this->Actor() );

    //// Bind Shape DS with actor. This is necessary for VIS picker -- it will
    //// not work without the corresponding Information key
    //IVtkTools_ShapeObject::SetShapeSource( m_DS, this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( m_source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_PartPipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//-----------------------------------------------------------------------------

//! Callback for RemoveFromRenderer() routine.
void asiVisu_PartPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//-----------------------------------------------------------------------------

//! Callback for Update() routine.
void asiVisu_PartPipeline::callback_update()
{
  if ( m_bOCCTColorScheme && !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = true;
  }
}
