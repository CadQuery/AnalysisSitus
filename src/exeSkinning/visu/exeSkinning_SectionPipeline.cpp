//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_SectionPipeline.h>

// Visualization includes
#include <asiVisu_CurveSource.h>
#include <asiVisu_SectionDataProvider.h>

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
asiVisu_SectionPipeline::asiVisu_SectionPipeline()
: asiVisu_Pipeline      ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bMapperColorsSet (false)
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);
  this->Actor()->SetPickable(0); // No sense to select the curve. Setting it to non-pickable
                                 // lets us select control points belonging to the curve.
                                 // Without this setting it would become impossible
}

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_SectionPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_SectionDataProvider) sDP = Handle(asiVisu_SectionDataProvider)::DownCast(DP);

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
      vtkSmartPointer<asiVisu_CurveSource> src = vtkSmartPointer<asiVisu_CurveSource>::New();
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
void asiVisu_SectionPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_SectionPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_SectionPipeline::callback_update()
{
  if ( !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = asiVisu_Utils::InitDomainLookupTable();
    asiVisu_Utils::InitMapper(m_mapper, aLookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
