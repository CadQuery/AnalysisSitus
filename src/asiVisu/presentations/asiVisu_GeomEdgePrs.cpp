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
#include <asiVisu_GeomEdgePrs.h>

// asiVisu includes
#include <asiVisu_EdgeDataProvider.h>
#include <asiVisu_EdgeDomainPipeline.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
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
#include <vtkTextProperty.h>

//! Creates a Presentation object for the passed Geometry Edge Node.
//! \param theNode [in] Geometry Edge Node to create a Presentation for.
asiVisu_GeomEdgePrs::asiVisu_GeomEdgePrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  Handle(asiData_EdgeNode) edge_n = Handle(asiData_EdgeNode)::DownCast(theNode);

  // Initialize working part
  m_partNode = Handle(asiData_PartNode)::DownCast( edge_n->GetParentNode() );

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

void asiVisu_GeomEdgePrs::SetColor(const QColor& color) const
{
  asiVisu_Prs::SetColor(color);

  // Adjust color of text.
  vtkTextActor* actor = m_textWidget->GetTextActor();
  actor->GetTextProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
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

  // If naming service is alive, add persistent name
  if ( !m_partNode->GetNaming().IsNull() )
  {
    TCollection_AsciiString namingName;

    TITLE += " [";
    TITLE += m_partNode->GetNaming()->GenerateName(E);
    TITLE += "]";
  }

  // Add orientation of all vertices. We are interested in relative orientations
  // of vertices wrt the owner edge. Therefore, exploration is done for edge
  // whose orientation is reset to FORWARD.
  int vertex_idx = 0;
  for ( TopExp_Explorer exp(E.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); exp.More(); exp.Next() )
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
void asiVisu_GeomEdgePrs::highlight(vtkRenderer*                       theRenderer,
                                   const Handle(asiVisu_PickerResult)& thePickRes,
                                   const asiVisu_SelectionNature       theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomEdgePrs::unHighlight(vtkRenderer*                  theRenderer,
                                      const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

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
    m_textWidget->ReleaseFocus       ( );
  }
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomEdgePrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
  m_textWidget->Off();
}
