//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_face_prs.h>

// A-Situs (visualization) includes
#include <visu_face_data_provider.h>
#include <visu_face_domain_pipeline.h>
#include <visu_utils.h>

// OCCT includes
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

// VTK includes
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
  const int   F_idx = DP->GetFaceIndex();
  TopoDS_Face F     = DP->ExtractFace();

  // Prepare main title
  TCollection_AsciiString TITLE("Face #");
  TITLE += F_idx;
  TITLE += ": ";
  TITLE += this->orientationToString(F);

  // Add orientation of all wires
  int wire_idx = 0;
  for ( TopExp_Explorer exp(F, TopAbs_WIRE); exp.More(); exp.Next() )
  {
    ++wire_idx;
    const TopoDS_Wire& W = TopoDS::Wire( exp.Current() );

    TITLE += "\n";
    TITLE += "Wire #";
    TITLE += wire_idx;
    TITLE += ": ";
    TITLE += this->orientationToString(W);
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
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
  // Do nothing...
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_face_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                   const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_face_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                     const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
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
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_geom_face_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  m_textWidget->Off();
}

//-----------------------------------------------------------------------------

//! Converts orientation of the given shape to string.
//! \param shape [in] shape to access orientation.
TCollection_AsciiString
  visu_geom_face_prs::orientationToString(const TopoDS_Shape& shape) const
{
  TCollection_AsciiString oriStr;

  // Check orientation
  if ( shape.Orientation() == TopAbs_FORWARD )
    oriStr = "FORWARD";
  else if ( shape.Orientation() == TopAbs_REVERSED )
    oriStr = "REVERSED";
  else if ( shape.Orientation() == TopAbs_INTERNAL )
    oriStr = "INTERNAL";
  else if ( shape.Orientation() == TopAbs_EXTERNAL )
    oriStr = "EXTERNAL";

  return oriStr;
}
