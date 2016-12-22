//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVPointSetPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_IVPointSetDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVPointSetPrs::asiVisu_IVPointSetPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_IVPointSetDataProvider) DP = new asiVisu_IVPointSetDataProvider(theNode);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVPointSetPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVPointSetPrs(theNode);
}

//! Callback after initialization.
void asiVisu_IVPointSetPrs::afterInitPipelines()
{
  Handle(asiVisu_IVPointSetDataProvider)
    DP = Handle(asiVisu_IVPointSetDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumberOfPoints();

  if ( nPts > 1 )
  {
    TCollection_AsciiString title("Num. of points: "); title += nPts;

    // Update text on the annotation
    m_textWidget->GetTextActor()->SetInput( title.ToCString() );
  }
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_IVPointSetPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  Handle(asiVisu_IVPointSetDataProvider)
    DP = Handle(asiVisu_IVPointSetDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumberOfPoints();
  //
  if ( nPts > 1 )
  {
    if ( !m_textWidget->GetCurrentRenderer() )
    {
      m_textWidget->SetInteractor      ( theRenderer->GetRenderWindow()->GetInteractor() );
      m_textWidget->SetDefaultRenderer ( theRenderer );
      m_textWidget->SetCurrentRenderer ( theRenderer );
      m_textWidget->On                 ( );
    }
  }
}

//! Callback for de-rendering.
//! \param renderer [in] renderer.
void asiVisu_IVPointSetPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(renderer)) const
{
  m_textWidget->Off();
}
