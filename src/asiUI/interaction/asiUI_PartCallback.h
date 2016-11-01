//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_PartCallback_h
#define asiUI_PartCallback_h

// A-Situs (visualization) includes
#include <asiVisu_ViewerCallback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Part viewer.
class asiUI_PartCallback : public QObject,
                             public asiVisu_ViewerCallback
{
  Q_OBJECT

public:

  static asiUI_PartCallback* New();
  static asiUI_PartCallback* New(asiUI_Viewer* theViewer);
  vtkTypeMacro(asiUI_PartCallback, asiVisu_ViewerCallback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

signals:

  void findFace();

private:

  asiUI_PartCallback  (asiUI_Viewer* theViewer);
  ~asiUI_PartCallback ();

};

#endif
