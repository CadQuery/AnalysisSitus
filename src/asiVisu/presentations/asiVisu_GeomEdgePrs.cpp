//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_edge_prs.h>

// Visualization includes
#include <visu_edge_data_provider.h>
#include <visu_edge_domain_pipeline.h>
#include <visu_utils.h>

// Geometry includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Geometry Edge Node.
//! \param theNode [in] Geometry Edge Node to create a Presentation for.
visu_geom_edge_prs::visu_geom_edge_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_edge_data_provider) DP =
    new visu_edge_data_provider( theNode->GetId(),
                                 ActParamStream() << theNode->Parameter(asiData_EdgeNode::PID_SelectedEdge) );

  // Pipeline for edge
  this->addPipeline        ( Pipeline_Main, new visu_edge_domain_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Edge Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_edge_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_edge_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_edge_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_edge_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_edge_prs::afterInitPipelines()
{
  Handle(visu_edge_data_provider)
    DP = Handle(visu_edge_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Get working edge
  const int   E_idx = DP->GetEdgeIndexAmongEdges();
  TopoDS_Edge E     = DP->ExtractEdge();

  // Prepare main title
  TCollection_AsciiString TITLE("Edge (#");
  TITLE += E_idx; TITLE += "): ";
  TITLE += asiAlgo_Utils::OrientationToString(E);

  // Add orientation of all vertices
  int vertex_idx = 0;
  for ( TopExp_Explorer exp(E, TopAbs_VERTEX); exp.More(); exp.Next() )
  {
    ++vertex_idx;
    const TopoDS_Vertex& V = TopoDS::Vertex( exp.Current() );

    TITLE += "\n";
    TITLE += "Vertex (#";
    TITLE += vertex_idx; TITLE += "): ";
    TITLE += asiAlgo_Utils::OrientationToString(V);
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_edge_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_edge_prs::afterUpdatePipelines() const
{}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_edge_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                   const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_edge_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                     const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_edge_prs::renderPipelines(vtkRenderer* theRenderer) const
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
void visu_geom_edge_prs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  m_textWidget->Off();
}
