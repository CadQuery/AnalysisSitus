//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_pdomain_callback_h
#define visu_pdomain_callback_h

// A-Situs (visualization) includes
#include <visu_viewer_callback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Face Domain viewer.
class visu_pdomain_callback : public QObject,
                              public visu_viewer_callback
{
  Q_OBJECT

public:

  static visu_pdomain_callback* New();
  static visu_pdomain_callback* New(gui_viewer* theViewer);
  vtkTypeMacro(visu_pdomain_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

signals:

  void killEdges();
  void joinEdges();

private:

  visu_pdomain_callback  (gui_viewer* theViewer);
  ~visu_pdomain_callback ();

};

#endif
