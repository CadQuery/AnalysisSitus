//-----------------------------------------------------------------------------
// Created on: 22 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_pick_facet_callback_h
#define visu_pick_facet_callback_h

// Visualization includes
#include <visu_viewer_callback.h>

// Geometry includes
#include <asiAlgo_BVHFacets.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking a facet in 3D viewer.
class visu_pick_facet_callback : public visu_viewer_callback
{
public:

  static visu_pick_facet_callback* New();
  //
  vtkTypeMacro(visu_pick_facet_callback, visu_viewer_callback);

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

public:

  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh_facets)
  {
    m_bvh = bvh_facets;
  }

private:

  visu_pick_facet_callback  (const Handle(asiAlgo_BVHFacets)& bvh_facets, gui_viewer* viewer);
  ~visu_pick_facet_callback ();

private:

  Handle(asiAlgo_BVHFacets) m_bvh; //!< Accelerating structure for picking.

};

#endif
