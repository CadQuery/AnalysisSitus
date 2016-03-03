//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_viewer_callback.h>

//! Constructor.
//! \param theViewer [in] owning viewer.
visu_viewer_callback::visu_viewer_callback(gui_viewer* theViewer)
: vtkCommand(), m_pViewer(theViewer)
{}

//! Destructor.
visu_viewer_callback::~visu_viewer_callback()
{}

//! Sets viewer.
//! \param theViewer [in] owning viewer.
void visu_viewer_callback::SetViewer(gui_viewer* theViewer)
{
  m_pViewer = theViewer;
}

//! Returns viewer instance.
//! \return viewer instance.
gui_viewer* visu_viewer_callback::Viewer()
{
  return m_pViewer;
}
