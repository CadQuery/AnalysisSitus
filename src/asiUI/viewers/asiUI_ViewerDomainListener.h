//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerDomainListener_h
#define asiUI_ViewerDomainListener_h

// asiUI includes
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// Qt includes
#include <QObject>

//! Default slots for domain viewer.
class asiUI_ViewerDomainListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ViewerDomainListener(asiUI_ViewerPart*              wViewerPart,
                               asiUI_ViewerDomain*            wViewerDomain,
                               asiUI_ViewerSurface*           wViewerSurface,
                               const Handle(asiEngine_Model)& model);

  asiUI_EXPORT virtual
    ~asiUI_ViewerDomainListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected:

  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerSurface*    m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.

};

#endif
