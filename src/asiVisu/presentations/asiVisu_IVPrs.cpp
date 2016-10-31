//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_prs.h>

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_iv_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_iv_prs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_iv_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_iv_prs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void visu_iv_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                            const visu_pick_result&      ASitus_NotUsed(thePickRes),
                            const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void visu_iv_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                              const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void visu_iv_prs::renderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void visu_iv_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  // Do nothing...
}
