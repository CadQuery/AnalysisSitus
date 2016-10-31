//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_point_set_prs.h>

// A-Situs (visualization) includes
#include <visu_iv_point_set_data_provider.h>
#include <visu_points_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
visu_iv_point_set_prs::visu_iv_point_set_prs(const Handle(ActAPI_INode)& theNode)
: visu_iv_prs(theNode)
{
  // Create Data Provider
  Handle(visu_iv_point_set_data_provider) DP = new visu_iv_point_set_data_provider(theNode);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new visu_points_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_iv_point_set_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_iv_point_set_prs(theNode);
}

//! Callback after initialization.
void visu_iv_point_set_prs::afterInitPipelines()
{
  Handle(visu_iv_point_set_data_provider)
    DP = Handle(visu_iv_point_set_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumOfPoints();

  if ( nPts > 1 )
  {
    TCollection_AsciiString title("Num. of points: "); title += nPts;

    // Update text on the annotation
    m_textWidget->GetTextActor()->SetInput( title.ToCString() );
  }
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_iv_point_set_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  Handle(visu_iv_point_set_data_provider)
    DP = Handle(visu_iv_point_set_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  const int nPts = DP->GetPoints()->GetNumOfPoints();
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
void visu_iv_point_set_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(renderer)) const
{
  m_textWidget->Off();
}
