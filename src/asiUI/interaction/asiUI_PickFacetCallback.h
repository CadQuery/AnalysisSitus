//-----------------------------------------------------------------------------
// Created on: 22 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_PickFacetCallback_h
#define asiVisu_PickFacetCallback_h

// Visualization includes
#include <asiVisu_ViewerCallback.h>

// Geometry includes
#include <asiAlgo_BVHFacets.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking a facet in 3D viewer.
class asiVisu_PickFacetCallback : public asiVisu_ViewerCallback
{
public:

  static asiVisu_PickFacetCallback* New();
  //
  vtkTypeMacro(asiVisu_PickFacetCallback, asiVisu_ViewerCallback);

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

  asiVisu_PickFacetCallback  (const Handle(asiAlgo_BVHFacets)& bvh_facets, asiUI_Viewer* viewer);
  ~asiVisu_PickFacetCallback ();

private:

  Handle(asiAlgo_BVHFacets) m_bvh; //!< Accelerating structure for picking.

};

#endif
