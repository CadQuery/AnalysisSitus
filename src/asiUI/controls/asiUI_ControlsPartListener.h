//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_ControlsPartListener_h
#define asiUI_ControlsPartListener_h

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// Qt includes
#include <QObject>

//! Default slots for controls operating with parts.
class asiUI_ControlsPartListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ControlsPartListener(asiUI_ControlsPart*            wControls,
                               asiUI_ViewerPart*              wViewerPart,
                               asiUI_ViewerDomain*            wViewerDomain,
                               asiUI_ViewerSurface*           wViewerSurface,
                               const Handle(asiEngine_Model)& model,
                               ActAPI_ProgressEntry           notifier);

  asiUI_EXPORT virtual
    ~asiUI_ControlsPartListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected slots:

  asiUI_EXPORT void onPartLoaded       ();
  asiUI_EXPORT void onPartModified     ();
  asiUI_EXPORT void onRobustOn         ();
  asiUI_EXPORT void onRobustOff        ();
  asiUI_EXPORT void onVerticesOn       ();
  asiUI_EXPORT void onVerticesOff      ();
  asiUI_EXPORT void onNormalsOn        ();
  asiUI_EXPORT void onNormalsOff       ();
  asiUI_EXPORT void onSelectionFacesOn ();
  asiUI_EXPORT void onSelectionEdgesOn ();

protected:

  asiUI_EXPORT void
    cleanViewers();

  asiUI_EXPORT virtual void
    reinitializeEverything();

protected:

  asiUI_ControlsPart*     m_wControls;      //!< Controls.
  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerSurface*    m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.
  ActAPI_ProgressEntry    m_notifier;       //!< Progress notifier.

};

#endif
