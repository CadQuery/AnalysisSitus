//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_BCurvePolesPipeline.h>

// Visualization includes
#include <asiVisu_BCurvePolesSource.h>
#include <asiVisu_CurveDataProvider.h>
#include <asiVisu_NodeInfo.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Creates new Pipeline initialized by default VTK mapper and actor.
asiVisu_BCurvePolesPipeline::asiVisu_BCurvePolesPipeline()
//
: asiVisu_Pipeline      ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet (false),
  m_bForced          (false)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
  this->Actor()->GetProperty()->SetLineStipplePattern(0xf0f0);
  this->Actor()->GetProperty()->SetLineStippleRepeatFactor(1);
  this->Actor()->GetProperty()->SetPointSize(5.0f);

  //---------------------------------------------------------------------------

  // Selected items
  m_selected = vtkSmartPointer<vtkIdTypeArray>::New();

  // Selection node
  m_selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
  m_selectionNode->SetFieldType(vtkSelectionNode::CELL);
  m_selectionNode->SetContentType(vtkSelectionNode::GLOBALIDS);

  // Selection
  m_selection = vtkSmartPointer<vtkSelection>::New();
  m_selection->AddNode(m_selectionNode);

  // Selection extraction
  m_extractSelection = vtkSmartPointer<vtkExtractSelection>::New();

  // Geometry filter to convert unstructured selection results to poly data
  m_toPolyData = vtkSmartPointer<vtkGeometryFilter>::New();
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_BCurvePolesPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_CurveDataProvider) dp = Handle(asiVisu_CurveDataProvider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(Standard_Type) curve_type = dp->GetCurveType();
  //
  if ( curve_type.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(dummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( m_bForced || dp->MustExecute( this->GetMTime() ) )
  {
    if ( m_bForced ) m_bForced = false; // Executed, reset forced

    // Bind to Node
    asiVisu_NodeInfo::Store( dp->GetNodeID(), this->Actor() );

    // Access curve (3d or 2d)
    double f, l;
    Handle(Geom_Curve)   c3d = dp->GetCurve(f, l);
    Handle(Geom2d_Curve) c2d = dp->GetCurve2d(f, l);

    // B-curve poles
    if ( !c3d.IsNull() && c3d->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) )
    {
      Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(c3d);
      //
      vtkSmartPointer<asiVisu_BCurvePolesSource>
        bpoles_src = vtkSmartPointer<asiVisu_BCurvePolesSource>::New();
      //
      bpoles_src->SetInputCurve(BC);

      // Apply selection
      if ( m_selected->GetSize() )
      {
        m_extractSelection->SetInputConnection( 0, bpoles_src->GetOutputPort() );
        m_extractSelection->SetInputData(1, m_selection);
        m_toPolyData->SetInputConnection( m_extractSelection->GetOutputPort() );

        // Set ultimate input
        this->SetInputConnection( m_toPolyData->GetOutputPort() );
      }
      else
        this->SetInputConnection( bpoles_src->GetOutputPort() );
    }
    else if ( !c2d.IsNull() && c2d->IsKind( STANDARD_TYPE(Geom2d_BSplineCurve) ) )
    {
      Handle(Geom2d_BSplineCurve) BC = Handle(Geom2d_BSplineCurve)::DownCast(c2d);
      //
      vtkSmartPointer<asiVisu_BCurvePolesSource>
        bpoles_src = vtkSmartPointer<asiVisu_BCurvePolesSource>::New();
      //
      bpoles_src->SetInputCurve2d(BC);

      // Apply selection
      if ( m_selected->GetSize() )
      {
        m_extractSelection->SetInputConnection(0, bpoles_src->GetOutputPort() );
        m_extractSelection->SetInputData(1, m_selection);
        m_toPolyData->SetInputConnection( m_extractSelection->GetOutputPort() );

        // Set ultimate input
        this->SetInputConnection( m_toPolyData->GetOutputPort() );
      }
      else
        this->SetInputConnection( bpoles_src->GetOutputPort() );
    }
    else
    {
      // Pass empty data set in order to have valid pipeline
      vtkSmartPointer<vtkPolyData> dummyDS = vtkSmartPointer<vtkPolyData>::New();
      this->SetInputData(dummyDS);
    }
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Sets poles selected for visualization.
//! \param mask [in] selected poles.
void asiVisu_BCurvePolesPipeline::SetSelectedPoles(const TColStd_PackedMapOfInteger& mask)
{
  m_selected->Reset();
  m_selected->SetNumberOfComponents(1);

  // Populate VTK array with cell IDs to keep in visualization
  for ( TColStd_MapIteratorOfPackedMapOfInteger it(mask); it.More(); it.Next() )
  {
    const int id = it.Key();
    m_selected->InsertNextValue(id);
  }

  // Initialize selection node
  m_selectionNode->SetSelectionList(m_selected);
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_BCurvePolesPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_BCurvePolesPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_BCurvePolesPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = asiVisu_Utils::InitDomainLookupTable();
    asiVisu_Utils::InitMapper(m_mapper, aLookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
