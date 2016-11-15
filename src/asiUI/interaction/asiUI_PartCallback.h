//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_PartCallback_h
#define asiUI_PartCallback_h

// A-Situs (visualization) includes
#include <asiUI_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Part viewer.
class asiUI_PartCallback : public QObject,
                           public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  asiUI_EXPORT static asiUI_PartCallback*
    New();

  asiUI_EXPORT static asiUI_PartCallback*
    New(asiUI_Viewer* pViewer);

  vtkTypeMacro(asiUI_PartCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

signals:

  void findFace();

private:

  asiUI_EXPORT
    asiUI_PartCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_PartCallback();

};

#endif
