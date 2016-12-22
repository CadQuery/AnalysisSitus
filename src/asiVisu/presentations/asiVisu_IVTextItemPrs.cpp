//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVTextItemPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVTextItemPrs::asiVisu_IVTextItemPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVTextItemPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVTextItemPrs(theNode);
}

//! Callback after initialization.
void asiVisu_IVTextItemPrs::afterInitPipelines()
{
  TCollection_AsciiString
    title = Handle(asiData_IVTextItemNode)::DownCast(m_node)->GetText();

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( title.ToCString() );
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_IVTextItemPrs::renderPipelines(vtkRenderer* theRenderer) const
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
//! \param renderer [in] renderer.
void asiVisu_IVTextItemPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(renderer)) const
{
  m_textWidget->Off();
}
