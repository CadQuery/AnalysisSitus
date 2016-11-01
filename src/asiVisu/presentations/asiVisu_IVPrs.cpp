//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVPrs.h>

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_IVPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_IVPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_IVPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_IVPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_IVPrs::highlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                            const asiVisu_PickResult&      asiVisu_NotUsed(thePickRes),
                            const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
void asiVisu_IVPrs::unHighlight(vtkRenderer*                 asiVisu_NotUsed(theRenderer),
                              const asiVisu_SelectionNature& asiVisu_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
void asiVisu_IVPrs::renderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}

//! Callback for de-rendering.
void asiVisu_IVPrs::deRenderPipelines(vtkRenderer* asiVisu_NotUsed(theRenderer)) const
{
  // Do nothing...
}
