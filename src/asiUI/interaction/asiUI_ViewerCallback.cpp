//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerCallback.h>

//! Constructor.
//! \param theViewer [in] owning viewer.
asiUI_ViewerCallback::asiUI_ViewerCallback(asiUI_Viewer* theViewer)
: vtkCommand(), m_pViewer(theViewer)
{}

//! Destructor.
asiUI_ViewerCallback::~asiUI_ViewerCallback()
{}

//! Sets viewer.
//! \param theViewer [in] owning viewer.
void asiUI_ViewerCallback::SetViewer(asiUI_Viewer* theViewer)
{
  m_pViewer = theViewer;
}

//! Returns viewer instance.
//! \return viewer instance.
asiUI_Viewer* asiUI_ViewerCallback::Viewer()
{
  return m_pViewer;
}
