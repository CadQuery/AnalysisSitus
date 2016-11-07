//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerDomainListener.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] host geometry viewer.
//! \param model          [in] Data Model instance.
asiUI_ViewerDomainListener::asiUI_ViewerDomainListener(asiUI_ViewerPart*              wViewerPart,
                                                       asiUI_ViewerDomain*            wViewerDomain,
                                                       asiUI_ViewerSurface*           wViewerSurface,
                                                       const Handle(asiEngine_Model)& model)
: QObject          (),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerDomainListener::~asiUI_ViewerDomainListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_ViewerDomainListener::Connect()
{
  // Nothing yet
}
