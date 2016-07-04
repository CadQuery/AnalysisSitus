//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_face_prs.h>

// Visualization includes
#include <visu_face_data_provider.h>
#include <visu_face_domain_pipeline.h>
#include <visu_utils.h>

// Geometry includes
#include <geom_utils.h>

// OCCT includes
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Geometry Face Node.
//! \param theNode [in] Geometry Face Node to create a Presentation for.
visu_geom_face_prs::visu_geom_face_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_face_data_provider) DP =
    new visu_face_data_provider( theNode->GetId(),
                                 ActParamStream() << theNode->Parameter(geom_face_node::PID_SelectedFace) );

  // Pipeline for face
  this->addPipeline        ( Pipeline_Main, new visu_face_domain_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);

  /* ========================
   *  Pipeline for detecting
   * ======================== */

  double detect_color[3];
  visu_utils::DefaultDetectionColor(detect_color[0], detect_color[1], detect_color[2]);

  // Create pipeline for highlighting
  Handle(visu_face_domain_pipeline) detect_pl = new visu_face_domain_pipeline(false);

  // Adjust props
  detect_pl->Actor()->GetProperty()->SetColor(detect_color[0], detect_color[1], detect_color[2]);
  detect_pl->Actor()->GetProperty()->SetLineWidth( visu_utils::DefaultDetectionLineWidth() + 1 );
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
  this->installDetectPipeline( detect_pl, DP );

  /* ========================
   *  Pipeline for selection
   * ======================== */

  double sel_color[3];
  visu_utils::DefaultPickingColor(sel_color[0], sel_color[1], sel_color[2]);

  // Create pipeline for selection
  Handle(visu_face_domain_pipeline) sel_pl = new visu_face_domain_pipeline(false);

  // Adjust props
  sel_pl->Actor()->GetProperty()->SetColor(sel_color[0], sel_color[1], sel_color[2]);
  sel_pl->Actor()->GetProperty()->SetLineWidth( visu_utils::DefaultPickLineWidth() + 1 );
  sel_pl->Actor()->GetProperty()->SetPointSize( visu_utils::DefaultHilightPointSize() );
  sel_pl->Actor()->SetPickable(0);
  sel_pl->Mapper()->ScalarVisibilityOff();
  //
  sel_pl->Actor()->GetProperty()->SetOpacity(1.0);
  sel_pl->Actor()->GetProperty()->SetAmbient(0.8);
  sel_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  sel_pl->Actor()->GetProperty()->SetSpecular(0.9);
  sel_pl->Actor()->GetProperty()->SetSpecularPower(20);
  //
  sel_pl->Actor()->SetVisibility(0);

  // Bind to the data provider
  this->installPickPipeline( sel_pl, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Face Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_face_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_face_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_face_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_face_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_face_prs::afterInitPipelines()
{
  Handle(visu_face_data_provider)
    DP = Handle(visu_face_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Get working face
  const int   F_idx        = DP->GetFaceIndexAmongFaces();
  TopoDS_Face F            = DP->ExtractFace();

  // Prepare main title
  TCollection_AsciiString TITLE("Face (#");
  TITLE += F_idx; TITLE += "): ";
  TITLE += geom_utils::OrientationToString(F);

  // Add orientation of all wires
  int wire_idx = 0;
  for ( TopExp_Explorer exp(F, TopAbs_WIRE); exp.More(); exp.Next() )
  {
    ++wire_idx;
    const TopoDS_Wire& W = TopoDS::Wire( exp.Current() );

    TITLE += "\n";
    TITLE += "Wire (#";
    TITLE += wire_idx; TITLE += "): ";
    TITLE += geom_utils::OrientationToString(W);
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );

  //---------------------------------------------------------------------------
  // Detection and highlighting
  //---------------------------------------------------------------------------

  // Access pipelines dedicated for highlighting
  const Handle(visu_face_domain_pipeline)&
    detect_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetDetectPipeline() );
  const Handle(visu_face_domain_pipeline)&
    pick_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetPickPipeline() );

  const Handle(visu_face_data_provider)&
    detect_dp = Handle(visu_face_data_provider)::DownCast( this->dataProviderDetect() );
  const Handle(visu_face_data_provider)&
    pick_dp = Handle(visu_face_data_provider)::DownCast( this->dataProviderPick() );

  // Init PL as selection pipelines are not automated
  detect_pl->SetInput(detect_dp);
  pick_pl->SetInput(pick_dp);
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_face_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_face_prs::afterUpdatePipelines() const
{
  /* ====================================
   *  Update selection pipelines as well
   * ==================================== */

  // Access pipelines dedicated for highlighting
  const Handle(visu_face_domain_pipeline)&
    detect_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetDetectPipeline() );
  const Handle(visu_face_domain_pipeline)&
    pick_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetPickPipeline() );

  // IMPORTANT: We update our highlighting pipelines here just to make things
  // faster. The better place to do that is "highlight" method, because
  // we do not really need to build highlighting pipelines just after
  // the Nodal Presentation is created. Logically, we would better to prepare
  // this pipeline only on actual selection request from user. However, in the
  // latter case the reactivity of application might significantly slow down
  detect_pl->Update();
  pick_pl->Update();
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_face_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_pick_result&      thePickRes,
                                   const visu_selection_nature& theSelNature) const
{
  // Get target actor which is the only sensitive
  Handle(visu_pipeline) poles_pl = this->GetPipeline(Pipeline_Main);
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
  Handle(visu_face_domain_pipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetDetectPipeline() );

  if ( !hili_pl )
    return;

  // Set selection mask...
  hili_pl->SetSelectedCells(cellIds);
  hili_pl->ForceExecution();
  hili_pl->SetInput( this->dataProviderDetect() );

  // ... and visibility
  hili_pl->Actor()->SetVisibility( !cellIds.IsEmpty() );
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_face_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                     const visu_selection_nature& theSelNature) const
{
  // Access pipeline for highlighting
  Handle(visu_face_domain_pipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetDetectPipeline() );

  if ( !hili_pl )
    return;

  // Set selection mask...
  hili_pl->SetSelectedCells( TColStd_PackedMapOfInteger() );

  // ... and visibility
  hili_pl->Actor()->SetVisibility(0);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_face_prs::renderPipelines(vtkRenderer* theRenderer) const
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

  Handle(visu_face_domain_pipeline)
    detect_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetDetectPipeline() ),
    pick_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetPickPipeline() );

  // Picking pipeline must be added to renderer the LAST (!). Otherwise
  // we experience some strange coloring bug because of their coincidence
  /* (1) */ detect_pl->AddToRenderer(theRenderer);
  /* (2) */ pick_pl->AddToRenderer(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_geom_face_prs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  m_textWidget->Off();

  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  Handle(visu_face_domain_pipeline)
    detect_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetDetectPipeline() ),
    pick_pl = Handle(visu_face_domain_pipeline)::DownCast( this->GetPickPipeline() );
  //
  detect_pl->RemoveFromRenderer(theRenderer);
  pick_pl->RemoveFromRenderer(theRenderer);
}
