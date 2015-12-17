//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_bcurve_poles_pipeline.h>

// Visualization includes
#include <visu_bcurve_poles_source.h>
#include <visu_node_info.h>
#include <visu_section_data_provider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//! Creates new Pipeline initialized by default VTK mapper and actor.
visu_bcurve_poles_pipeline::visu_bcurve_poles_pipeline()
: visu_pipeline      ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
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
void visu_bcurve_poles_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  Handle(visu_section_data_provider) sDP = Handle(visu_section_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape shape = sDP->GetShape();
  if ( shape.IsNull() || shape.ShapeType() != TopAbs_WIRE )
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

  if ( m_bForced || sDP->MustExecute( this->GetMTime() ) )
  {
    if ( m_bForced ) m_bForced = false; // Executed, reset forced

    // Bind to Node
    visu_node_info::Store( sDP->GetNodeID(), this->Actor() );

    // WARNING: only a single edge is currently supported
    TopExp_Explorer exp(shape, TopAbs_EDGE);
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );
    //
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);

    // B-curve poles
    if ( C->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) )
    {
      Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(C);
      //
      vtkSmartPointer<visu_bcurve_poles_source>
        bpoles_src = vtkSmartPointer<visu_bcurve_poles_source>::New();
      //
      bpoles_src->SetInputCurve(BC);

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
void visu_bcurve_poles_pipeline::SetSelectedPoles(const TColStd_PackedMapOfInteger& mask)
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
void visu_bcurve_poles_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_bcurve_poles_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_bcurve_poles_pipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = visu_utils::InitDomainLookupTable();
    visu_utils::InitMapper(m_mapper, aLookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
