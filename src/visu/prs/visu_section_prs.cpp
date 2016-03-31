//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_section_prs.h>

// A-Situs (visualization) includes
#include <visu_bcurve_poles_pipeline.h>
#include <visu_section_data_provider.h>
#include <visu_section_pipeline.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapeObject.hxx>
#pragma warning(pop)

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>

//! Creates a Presentation object for the passed Section Node.
//! \param N [in] Section Node to create a Presentation for.
visu_section_prs::visu_section_prs(const Handle(ActAPI_INode)& N) : visu_prs(N)
{
  // Create Data Provider
  Handle(visu_section_data_provider) DP =
    new visu_section_data_provider( N->GetId(),
                                    ActParamStream() << N->Parameter(geom_section_node::PID_Geometry) );

  // Main pipeline
  this->addPipeline        ( Pipeline_Main, new visu_section_pipeline() );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Pipeline for poles
  this->addPipeline        ( Pipeline_Poles, new visu_bcurve_poles_pipeline() );
  this->assignDataProvider ( Pipeline_Poles, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);

  /* ========================
   *  Pipeline for detecting
   * ======================== */

  double detect_color[3];
  visu_utils::DefaultDetectionColor(detect_color[0], detect_color[1], detect_color[2]);

  // Create pipeline for highlighting
  Handle(visu_bcurve_poles_pipeline) detect_pl = new visu_bcurve_poles_pipeline();

  // Adjust props
  detect_pl->Actor()->GetProperty()->SetColor(detect_color[0], detect_color[1], detect_color[2]);
  detect_pl->Actor()->GetProperty()->SetLineWidth( visu_utils::DefaultDetectionLineWidth() );
  detect_pl->Actor()->GetProperty()->SetPointSize( visu_utils::DefaultHilightPointSize() );
  detect_pl->Actor()->SetPickable(0);
  detect_pl->Mapper()->ScalarVisibilityOff();
  //
  detect_pl->Actor()->GetProperty()->SetOpacity(1.0);
  detect_pl->Actor()->GetProperty()->SetAmbient(0.8);
  detect_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  detect_pl->Actor()->GetProperty()->SetSpecular(0.9);
  detect_pl->Actor()->GetProperty()->SetSpecularPower(20);
  //
  detect_pl->Actor()->SetVisibility(0);

  // Bind to the data provider
  this->installDetectPipeline( detect_pl, DP->Clone() );
}

//! Factory method for Presentation.
//! \param N [in] Section Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_section_prs::Instance(const Handle(ActAPI_INode)& N)
{
  return new visu_section_prs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_section_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_section_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_section_prs::afterInitPipelines()
{
  Handle(visu_section_data_provider)
    DP = Handle(visu_section_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Access the visualized contour
  TopoDS_Wire W = TopoDS::Wire( DP->GetShape() );
  if ( W.IsNull() )
    return;

  TCollection_AsciiString TITLE;

  // Loop over the edges
  for ( TopExp_Explorer exp(W, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );
    //
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);

    // Some annotations come for b-curves
    if ( C->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) )
    {
      Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(C);

      // Prepare main title
      TITLE += BC->DynamicType()->Name(); TITLE += "\n";
      TITLE += "Degree: "; TITLE += BC->Degree(); TITLE += "\n";
      TITLE += "Continuity: ";
      //
      switch ( BC->Continuity() )
      {
        case GeomAbs_C0: TITLE += "C0"; break;
        case GeomAbs_C1: TITLE += "C1"; break;
        case GeomAbs_C2: TITLE += "C2"; break;
        case GeomAbs_C3: TITLE += "C3"; break;
        case GeomAbs_CN: TITLE += "CN"; break;
        case GeomAbs_G1: TITLE += "G1"; break;
        case GeomAbs_G2: TITLE += "G2"; break;
        default: break;
      }
      TITLE += "\n";
      TITLE += "Rational: ";  (TITLE += BC->IsRational() ? "true" : "false"); TITLE += "\n";
      TITLE += "Closed: ";    (TITLE += BC->IsClosed()   ? "true" : "false"); TITLE += "\n";
      TITLE += "Periodic: ";  (TITLE += BC->IsPeriodic() ? "true" : "false"); TITLE += "\n";
      //
      TITLE += "Num poles: "; TITLE += BC->Poles().Length(); TITLE += "\n";
    }
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );

  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  // Access pipelines dedicated for highlighting
  const Handle(visu_bcurve_poles_pipeline)&
    detect_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetDetectPipeline() );

  const Handle(visu_section_data_provider)&
    detect_dp = Handle(visu_section_data_provider)::DownCast( this->dataProviderDetect() );

  // Init PL as selection pipelines are not automated
  detect_pl->SetInput(detect_dp);
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_section_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_section_prs::afterUpdatePipelines() const
{
  /* ====================================
   *  Update selection pipelines as well
   * ==================================== */

  // Access pipelines dedicated for highlighting
  const Handle(visu_bcurve_poles_pipeline)&
    detect_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetDetectPipeline() );

  // IMPORTANT: We update our highlighting pipelines here just to make things
  // faster. The better place to do that is "highlight" method, because
  // we do not really need to build highlighting pipelines just after
  // the Nodal Presentation is created. Logically, we would better to prepare
  // this pipeline only on actual selection request from user. However, in the
  // latter case the reactivity of application might significantly slow down
  detect_pl->Update();
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_section_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                 const visu_pick_result&      thePickRes,
                                 const visu_selection_nature& theSelNature) const
{
  // Get target actor which is the only sensitive
  Handle(visu_pipeline) poles_pl = this->GetPipeline(Pipeline_Poles);
  //
  vtkActor* poles_actor = poles_pl->Actor();

  // Get the list of picked cell IDs
  TColStd_PackedMapOfInteger cellIds;
  if ( thePickRes.IsSelectionWorkpiece() )
  {
    const visu_actor_elem_map& pickMap = thePickRes.GetPickMap();
    for ( visu_actor_elem_map::Iterator it(pickMap); it.More(); it.Next() )
    {
      const vtkSmartPointer<vtkActor>& aResActor = it.Key();
      if ( aResActor != poles_actor )
        continue;

      cellIds = it.Value();
    }
  }

  //---------------------------------------------------------------------------
  // Update highlighting pipelines
  //---------------------------------------------------------------------------

  // Access pipeline for highlighting
  Handle(visu_bcurve_poles_pipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetDetectPipeline() );

  if ( !hili_pl )
    return;

  // Set selection mask...
  hili_pl->SetSelectedPoles(cellIds);
  hili_pl->ForceExecution();
  hili_pl->SetInput( this->dataProviderDetect() );

  // ... and visibility
  hili_pl->Actor()->SetVisibility( !cellIds.IsEmpty() );
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_section_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_selection_nature& theSelNature) const
{
  // Access pipeline for highlighting
  Handle(visu_bcurve_poles_pipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetDetectPipeline() );

  if ( !hili_pl )
    return;

  // Set selection mask...
  hili_pl->SetSelectedPoles( TColStd_PackedMapOfInteger() );

  // ... and visibility
  hili_pl->Actor()->SetVisibility(0);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_section_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  if ( !m_textWidget->GetCurrentRenderer() )
  {
    m_textWidget->SetInteractor      ( theRenderer->GetRenderWindow()->GetInteractor() );
    m_textWidget->SetDefaultRenderer ( theRenderer );
    m_textWidget->SetCurrentRenderer ( theRenderer );
    m_textWidget->On                 ( );
  }

  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  Handle(visu_bcurve_poles_pipeline)
    detect_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetDetectPipeline() );

  // Picking pipeline must be added to renderer the LAST (!). Otherwise
  // we experience some strange coloring bug because of their coincidence
  /* (1) */ detect_pl->AddToRenderer(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_section_prs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  m_textWidget->Off();

  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  Handle(visu_bcurve_poles_pipeline)
    detect_pl = Handle(visu_bcurve_poles_pipeline)::DownCast( this->GetDetectPipeline() );
  //
  detect_pl->RemoveFromRenderer(theRenderer);
}
