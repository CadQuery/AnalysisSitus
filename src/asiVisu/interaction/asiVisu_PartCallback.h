//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_part_callback_h
#define visu_part_callback_h

// A-Situs (visualization) includes
#include <visu_viewer_callback.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for operations in Part viewer.
class visu_part_callback : public QObject,
                           public visu_viewer_callback
{
  Q_OBJECT

public:

  static visu_part_callback* New();
  static visu_part_callback* New(gui_viewer* theViewer);
  vtkTypeMacro(visu_part_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

signals:

  void findFace();

private:

  visu_part_callback  (gui_viewer* theViewer);
  ~visu_part_callback ();

};

#endif
