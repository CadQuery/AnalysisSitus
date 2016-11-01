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

  static asiUI_PDomainCallback* New();
  static asiUI_PDomainCallback* New(asiUI_Viewer* theViewer);
  vtkTypeMacro(asiUI_PDomainCallback, asiUI_ViewerCallback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

signals:

  void killEdges();
  void joinEdges();

private:

  asiUI_PDomainCallback  (asiUI_Viewer* theViewer);
  ~asiUI_PDomainCallback ();

};

#endif
