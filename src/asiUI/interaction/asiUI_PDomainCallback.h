//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_PDomainCallback_h
#define asiUI_PDomainCallback_h

// A-Situs (UI) includes
#include <asiUI_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Face Domain viewer.
class asiUI_PDomainCallback : public QObject,
                              public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  asiUI_EXPORT static asiUI_PDomainCallback*
    New();

  asiUI_EXPORT static asiUI_PDomainCallback*
    New(asiUI_Viewer* pViewer);

  vtkTypeMacro(asiUI_PDomainCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

signals:

  void killEdges();
  void joinEdges();

protected:

  asiUI_EXPORT
    asiUI_PDomainCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_PDomainCallback();

};

#endif
