//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomCurvePrs.h>

// A-Situs (visualization) includes
#include <asiVisu_BCurveKnotsPipeline.h>
#include <asiVisu_BCurvePolesPipeline.h>
#include <asiVisu_EdgeDataProvider.h>
#include <asiVisu_EdgeCurvePipeline.h>
#include <asiVisu_Utils.h>

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
asiVisu_GeomCurvePrs::asiVisu_GeomCurvePrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  Handle(asiData_EdgeNode) edge_n = Handle(asiData_EdgeNode)::DownCast(theNode);

  // Create Data Provider
  Handle(asiVisu_EdgeDataProvider) DP = new asiVisu_EdgeDataProvider(edge_n);

  // Pipeline for curve
  this->addPipeline        ( Pipeline_Main, new asiVisu_EdgeCurvePipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Pipeline for poles of b-curves
  this->addPipeline        ( Pipeline_Poles, new asiVisu_BCurvePolesPipeline );
  this->assignDataProvider ( Pipeline_Poles, DP );

  // Pipeline for knots of b-curves
  this->addPipeline        ( Pipeline_Knots, new asiVisu_BCurveKnotsPipeline );
  this->assignDataProvider ( Pipeline_Knots, DP );

  // Configure presentation
  this->GetPipeline(Pipeline_Main)  -> Actor()->GetProperty()->SetLineWidth(2.0f);
  this->GetPipeline(Pipeline_Poles) -> Actor()->GetProperty()->SetColor(0.6, 0.6, 0.6);
  this->GetPipeline(Pipeline_Knots) -> Actor()->GetProperty()->SetColor(0.0, 1.0, 0.0);

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Curve Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomCurvePrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_GeomCurvePrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomCurvePrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomCurvePrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomCurvePrs::afterInitPipelines()
{
  Handle(asiVisu_EdgeDataProvider)
    DP = Handle(asiVisu_EdgeDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

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
void asiVisu_GeomCurvePrs::beforeUpdatePipelines() const
{}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomCurvePrs::afterUpdatePipelines() const
{}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_GeomCurvePrs::highlight(vtkRenderer*                   asiVisu_NotUsed(theRenderer),
                                     const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                                     const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomCurvePrs::unHighlight(vtkRenderer*                   asiVisu_NotUsed(theRenderer),
                                       const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomCurvePrs::renderPipelines(vtkRenderer* theRenderer) const
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
void asiVisu_GeomCurvePrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  m_textWidget->Off();
}
