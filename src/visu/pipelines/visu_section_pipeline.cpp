//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_section_pipeline.h>

// Visualization includes
#include <visu_curve_source.h>
#include <visu_section_data_provider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//! Creates new Section Pipeline initialized by default VTK mapper and actor.
visu_section_pipeline::visu_section_pipeline()
: visu_pipeline      ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet (false)
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
  this->Actor()->SetPickable(0); // No sense to select the curve. Setting it to non-pickable
                                 // lets us select control points belonging to the curve.
                                 // Without this setting it would become impossible
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_section_pipeline::SetInput(const Handle(visu_data_provider)& DP)
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

  if ( sDP->MustExecute( this->GetMTime() ) )
  {
    // Append filter
    vtkSmartPointer<vtkAppendPolyData>
      appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

    // Loop over the edges
    for ( TopExp_Explorer exp(shape, TopAbs_EDGE); exp.More(); exp.Next() )
    {
      const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

      // Curve source
      vtkSmartPointer<visu_curve_source> src = vtkSmartPointer<visu_curve_source>::New();
      src->SetInputEdge(E);

      // Append poly data for curve
      appendFilter->AddInputConnection( src->GetOutputPort() );
    }

    // Initialize pipeline
    this->SetInputConnection( appendFilter->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_section_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_section_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_section_pipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = visu_utils::InitDomainLookupTable();
    visu_utils::InitMapper(m_mapper, aLookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
