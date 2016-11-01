//-----------------------------------------------------------------------------
// Created on: 22 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_PickFacetCallback_h
#define asiUI_PickFacetCallback_h

// UI includes
#include <asiUI_ViewerCallback.h>

// Geometry includes
#include <asiAlgo_BVHFacets.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking a facet in 3D viewer.
class asiUI_PickFacetCallback : public asiUI_ViewerCallback
{
public:

  static asiUI_PickFacetCallback* New();
  //
  vtkTypeMacro(asiUI_PickFacetCallback, asiUI_ViewerCallback);

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

  asiUI_PickFacetCallback  (const Handle(asiAlgo_BVHFacets)& bvh_facets, asiUI_Viewer* viewer);
  ~asiUI_PickFacetCallback ();

private:

  Handle(asiAlgo_BVHFacets) m_bvh; //!< Accelerating structure for picking.

};

#endif
