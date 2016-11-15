//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerCallback.h>

//! Constructor.
//! \param pViewer [in] owning viewer.
asiUI_ViewerCallback::asiUI_ViewerCallback(asiUI_Viewer* pViewer)
: vtkCommand(), m_pViewer(pViewer)
{}

//! Destructor.
asiUI_ViewerCallback::~asiUI_ViewerCallback()
{}

//! Sets viewer.
//! \param pViewer [in] owning viewer.
void asiUI_ViewerCallback::SetViewer(asiUI_Viewer* pViewer)
{
  m_pViewer = pViewer;
}

//! Returns viewer instance.
//! \return viewer instance.
asiUI_Viewer* asiUI_ViewerCallback::GetViewer()
{
  return m_pViewer;
}
