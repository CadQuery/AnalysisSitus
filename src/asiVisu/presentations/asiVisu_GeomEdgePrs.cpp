//-----------------------------------------------------------------------------
// Created on: 18 August 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
void asiVisu_GeomEdgePrs::highlight(vtkRenderer*                 theRenderer,
                                   const asiVisu_PickResult&     thePickRes,
                                   const asiVisu_SelectionNature theSelNature) const
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
