//-----------------------------------------------------------------------------
// Created on: 06 November 2016 (that's nice refactoring)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ControlsPartResponder_h
#define asiUI_ControlsPartResponder_h

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// Qt includes
#include <QObject>

//! Default responses for controls operating with parts.
class asiUI_ControlsPartResponder : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ControlsPartResponder(asiUI_ViewerPart*              wViewerPart,
                                asiUI_ViewerDomain*            wViewerDomain,
                                asiUI_ViewerSurface*           wViewerSurface,
                                const Handle(asiEngine_Model)& model);

  asiUI_EXPORT virtual
    ~asiUI_ControlsPartResponder();

public:

  asiUI_EXPORT virtual void
    Connect(asiUI_ControlsPart* wControls);

protected slots:

  asiUI_EXPORT void onPartLoaded       ();
  asiUI_EXPORT void onPartModified     ();
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

  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerSurface*    m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.

};

#endif
