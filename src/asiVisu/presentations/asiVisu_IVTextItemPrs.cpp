//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_text_item_prs.h>

// A-Situs (visualization) includes
#include <visu_utils.h>

// VTK includes
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
visu_iv_text_item_prs::visu_iv_text_item_prs(const Handle(ActAPI_INode)& theNode)
: visu_iv_prs(theNode)
{
  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_iv_text_item_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_iv_text_item_prs(theNode);
}

//! Callback after initialization.
void visu_iv_text_item_prs::afterInitPipelines()
{
  TCollection_AsciiString
    title = Handle(asiData_IVTextItemNode)::DownCast(m_node)->GetText();

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( title.ToCString() );
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_iv_text_item_prs::renderPipelines(vtkRenderer* theRenderer) const
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
void visu_iv_text_item_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(renderer)) const
{
  m_textWidget->Off();
}
