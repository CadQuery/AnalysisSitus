//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_surf_prs.h>

// A-Situs (visualization) includes
#include <visu_face_data_provider.h>
#include <visu_face_surface_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkTextActor.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>

//! Creates a Presentation object for the passed Geometry Surface Node.
//! \param theNode [in] Geometry Surface Node to create a Presentation for.
visu_geom_surf_prs::visu_geom_surf_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_face_data_provider) DP =
    new visu_face_data_provider( theNode->GetId(),
                                 ActParamStream() << theNode->Parameter(geom_surf_node::PID_SelectedFace) );

  // Pipeline for face
  this->addPipeline        ( Pipeline_Main, new visu_face_surface_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Surface Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_surf_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_surf_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_surf_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_surf_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_surf_prs::afterInitPipelines()
{
  Handle(visu_face_data_provider)
    DP = Handle(visu_face_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Get working face and its host surface
  const int            F_idx = DP->GetFaceIndex();
  TopoDS_Face          F     = DP->ExtractFace();
  Handle(Geom_Surface) S     = F.IsNull() ? NULL : BRep_Tool::Surface(F);

  // Prepare main title
  TCollection_AsciiString TITLE("Face #");
  TITLE += F_idx;
  TITLE += ": ";
  TITLE += ( S.IsNull() ? "NONE" : S->DynamicType()->Name() );

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_surf_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_surf_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_surf_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                   const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_surf_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                     const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_surf_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  if ( !m_textWidget->GetCurrentRenderer() )
  {
    m_textWidget->SetInteractor      ( theRenderer->GetRenderWindow()->GetInteractor() );
    m_textWidget->SetDefaultRenderer ( theRenderer );
    m_textWidget->SetCurrentRenderer ( theRenderer );
    m_textWidget->On                 ( );
  }
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_geom_surf_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  m_textWidget->Off();
}
