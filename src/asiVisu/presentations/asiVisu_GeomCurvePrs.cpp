//-----------------------------------------------------------------------------
// Created on: 18 August 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomCurvePrs.h>

// asiVisu includes
#include <asiVisu_BCurveKnotsPipeline.h>
#include <asiVisu_BCurvePolesPipeline.h>
#include <asiVisu_EdgeDataProvider.h>
#include <asiVisu_EdgeCurvePipeline.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

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
  Handle(asiData_CurveNode) curve_n = Handle(asiData_CurveNode)::DownCast(theNode);

  // Initialize working part
  m_partNode = Handle(asiData_PartNode)::DownCast( curve_n->GetParentNode() );

  // Create Data Provider
  Handle(asiVisu_EdgeDataProvider) DP = new asiVisu_EdgeDataProvider(curve_n);

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
  this->GetPipeline(Pipeline_Poles) -> Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
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

void asiVisu_GeomCurvePrs::SetColor(const QColor& color) const
{
  for ( PipelineMap::Iterator pit( m_pipelineRepo.Find(Group_Prs) ); pit.More(); pit.Next() )
  {
    if ( pit.Key() == Pipeline_Knots )
      continue;

    if ( pit.Key() == Pipeline_Knots )
      continue;

    // Get pipeline and actor.
    const Handle(asiVisu_Pipeline)& pipeline = pit.Value();
    //
    vtkActor*    actor = pipeline->Actor();
    vtkProperty* prop  = actor->GetProperty();

    // Set color for VTK property.
    prop->SetColor( color.redF(), color.greenF(), color.blueF() );
  }

  // Adjust color of text.
  vtkTextActor* actor = m_textWidget->GetTextActor();
  actor->GetTextProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
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
  TITLE += asiAlgo_Utils::OrientationToString(E);
  TITLE += " / ";
  TITLE += ( C.IsNull() ? "NONE" : C->DynamicType()->Name() );

  // If naming service is alive, add persistent name
  if ( !m_partNode->GetNaming().IsNull() )
  {
    TCollection_AsciiString namingName;

    if ( m_partNode->GetNaming()->FindName(E, namingName) )
    {
      TITLE += " [";
      TITLE += namingName;
      TITLE += "]";
    }
  }

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
    TITLE += asiAlgo_Utils::ContinuityToString( BC->Continuity() );

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
void asiVisu_GeomCurvePrs::highlight(vtkRenderer*                        theRenderer,
                                     const Handle(asiVisu_PickerResult)& thePickRes,
                                     const asiVisu_SelectionNature       theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomCurvePrs::unHighlight(vtkRenderer*                  theRenderer,
                                       const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

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
    m_textWidget->ReleaseFocus       ( );
  }
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomCurvePrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);

  m_textWidget->Off();
}
