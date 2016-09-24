//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_create_contour_callback_h
#define visu_create_contour_callback_h

// Visualization includes
#include <visu_viewer_callback.h>

// Geometry includes
#include <geom_bvh_facets.h>

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
  //
  vtkTypeMacro(visu_create_contour_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

public:

  void SetBVH(const Handle(geom_bvh_facets)& bvh_facets)
  {
    m_bvh = bvh_facets;
  }

private:

  visu_create_contour_callback  (gui_viewer* theViewer);
  ~visu_create_contour_callback ();

private:

  Handle(geom_bvh_facets) m_bvh; //!< Accelerating structure for picking.

};

#endif
