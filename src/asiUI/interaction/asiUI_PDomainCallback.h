//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_PDomainCallback_h
#define asiVisu_PDomainCallback_h

// A-Situs (visualization) includes
#include <asiVisu_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Face Domain viewer.
class asiVisu_PDomainCallback : public QObject,
                                public asiVisu_ViewerCallback
{
  Q_OBJECT

public:

  static asiVisu_PDomainCallback* New();
  static asiVisu_PDomainCallback* New(asiUI_Viewer* theViewer);
  vtkTypeMacro(asiVisu_PDomainCallback, asiVisu_ViewerCallback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

signals:

  void killEdges();
  void joinEdges();

private:

  asiVisu_PDomainCallback  (asiUI_Viewer* theViewer);
  ~asiVisu_PDomainCallback ();

};

#endif
