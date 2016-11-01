//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ViewerCallback.h>

//! Constructor.
//! \param theViewer [in] owning viewer.
asiVisu_ViewerCallback::asiVisu_ViewerCallback(asiUI_Viewer* theViewer)
: vtkCommand(), m_pViewer(theViewer)
{}

//! Destructor.
asiVisu_ViewerCallback::~asiVisu_ViewerCallback()
{}

//! Sets viewer.
//! \param theViewer [in] owning viewer.
void asiVisu_ViewerCallback::SetViewer(asiUI_Viewer* theViewer)
{
  m_pViewer = theViewer;
}

//! Returns viewer instance.
//! \return viewer instance.
asiUI_Viewer* asiVisu_ViewerCallback::Viewer()
{
  return m_pViewer;
}
