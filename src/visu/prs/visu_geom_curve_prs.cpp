//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_curve_prs.h>

// A-Situs (visualization) includes
#include <visu_edge_data_provider.h>
#include <visu_edge_curve_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Geometry Curve Node.
//! \param theNode [in] Geometry Curve Node to create a Presentation for.
visu_geom_curve_prs::visu_geom_curve_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_edge_data_provider) DP =
    new visu_edge_data_provider( theNode->GetId(),
                                 ActParamStream() << theNode->Parameter(geom_curve_node::PID_SelectedEdge) );

  // Pipeline for curve
  this->addPipeline        ( Pipeline_Main, new visu_edge_curve_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Curve Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_curve_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_curve_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_curve_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_curve_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_curve_prs::afterInitPipelines()
{
  Handle(visu_edge_data_provider)
    DP = Handle(visu_edge_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Get working edge and its host curve
  double f, l;
  const int          E_idx = DP->GetEdgeIndexAmongEdges();
  TopoDS_Edge        E     = DP->ExtractEdge();
  Handle(Geom_Curve) C     = E.IsNull() ? NULL : BRep_Tool::Curve(E, f, l);

  // Prepare main title
  TCollection_AsciiString TITLE("Edge (#");
  TITLE += E_idx;
  TITLE += "): ";
  TITLE += ( C.IsNull() ? "NONE" : C->DynamicType()->Name() );

  // Trimmed Curve
  if ( !C.IsNull() && C->IsInstance( STANDARD_TYPE(Geom_TrimmedCurve) ) )
  {
    Handle(Geom_TrimmedCurve) TC = Handle(Geom_TrimmedCurve)::DownCast(C);

    // Let's check basis curve
    Handle(Geom_Curve) BC = TC->BasisCurve();
    //
    TITLE += "\nBasis curve: ";
    TITLE += ( BC.IsNull() ? "NONE" : BC->DynamicType()->Name() );
  }

  // B-curve
  if ( !C.IsNull() && C->IsInstance( STANDARD_TYPE(Geom_BSplineCurve) ) )
  {
    Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(C);

    // Get curve properties
    TITLE += "\nDegree: "; TITLE += BC->Degree();
    TITLE += "\nContinuity: ";
    //
    const GeomAbs_Shape cont = BC->Continuity();
    switch ( cont )
    {
      case GeomAbs_C0 : TITLE += "C0"; break;
      case GeomAbs_C1 : TITLE += "C1"; break;
      case GeomAbs_C2 : TITLE += "C2"; break;
      case GeomAbs_C3 : TITLE += "C3"; break;
      case GeomAbs_CN : TITLE += "CN"; break;
      case GeomAbs_G1 : TITLE += "G1"; break;
      case GeomAbs_G2 : TITLE += "G2"; break;
      default: break;
    }

    if ( BC->IsPeriodic() )
      TITLE += "\nPeriodic";
    else
      TITLE += "\nNot periodic";
  }
  else if ( !C.IsNull() && C->IsInstance( STANDARD_TYPE(Geom_Circle) ) )
  {
    Handle(Geom_Circle) CC = Handle(Geom_Circle)::DownCast(C);
    const double r = CC->Radius();
    TITLE += "\nRadius: "; TITLE += r;
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_curve_prs::beforeUpdatePipelines() const
{}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_curve_prs::afterUpdatePipelines() const
{}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_curve_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                    const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                    const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_curve_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                      const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_curve_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  // Annotation
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
void visu_geom_curve_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  m_textWidget->Off();
}
