//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_create_contour_callback_h
#define visu_create_contour_callback_h

// A-Situs (visualization) includes
#include <visu_viewer_callback.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for contour creation.
class visu_create_contour_callback : public QObject,
                                     public visu_viewer_callback
{
  Q_OBJECT

public:

  static visu_create_contour_callback* New();
  static visu_create_contour_callback* New(gui_viewer* theViewer);
  vtkTypeMacro(visu_create_contour_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

private:

  visu_create_contour_callback  (gui_viewer* theViewer);
  ~visu_create_contour_callback ();

};

#endif
