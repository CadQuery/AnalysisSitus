//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerPartListener_h
#define asiUI_ViewerPartListener_h

// asiUI includes
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

// Qt includes
#include <QObject>

//! Default slots for part viewer.
class asiUI_ViewerPartListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                             asiUI_ViewerDomain*            wViewerDomain,
                             asiUI_ViewerSurface*           wViewerSurface,
                             const Handle(asiEngine_Model)& model,
                             ActAPI_PlotterEntry            plotter);

  asiUI_EXPORT virtual
    ~asiUI_ViewerPartListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected slots:

  asiUI_EXPORT void
    onFacePicked(const asiVisu_PickResult& pickRes);

  asiUI_EXPORT void
    onEdgePicked(const asiVisu_PickResult& pickRes);

  asiUI_EXPORT void
    onContextMenu(const QPoint&);

protected:

  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerSurface*    m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.
  ActAPI_PlotterEntry     m_plotter;        //!< Imperative plotter.

};

#endif
