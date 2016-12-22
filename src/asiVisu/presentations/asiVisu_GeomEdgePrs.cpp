//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomEdgePrs.h>

// Visualization includes
#include <asiVisu_EdgeDataProvider.h>
#include <asiVisu_EdgeDomainPipeline.h>
#include <asiVisu_Utils.h>

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
asiVisu_GeomEdgePrs::asiVisu_GeomEdgePrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  Handle(asiData_EdgeNode) edge_n = Handle(asiData_EdgeNode)::DownCast(theNode);

  // Create Data Provider
  Handle(asiVisu_EdgeDataProvider) DP = new asiVisu_EdgeDataProvider(edge_n);

  // Pipeline for edge
  this->addPipeline        ( Pipeline_Main, new asiVisu_EdgeDomainPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Edge Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomEdgePrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_GeomEdgePrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomEdgePrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomEdgePrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomEdgePrs::afterInitPipelines()
{
  Handle(asiVisu_EdgeDataProvider)
    DP = Handle(asiVisu_EdgeDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

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
void asiVisu_GeomEdgePrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomEdgePrs::afterUpdatePipelines() const
{}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_GeomEdgePrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                   const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                                   const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomEdgePrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                                     const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomEdgePrs::renderPipelines(vtkRenderer* theRenderer) const
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
void asiVisu_GeomEdgePrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  m_textWidget->Off();
}
