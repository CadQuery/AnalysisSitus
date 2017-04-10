//-----------------------------------------------------------------------------
// Created on: 22 September 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_PickFacetCallback_h
#define asiUI_PickFacetCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// asiEngine includes
#include <asiEngine_Model.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking a facet in 3D viewer.
class asiUI_PickFacetCallback : public asiUI_ViewerCallback
{
public:

  asiUI_EXPORT static asiUI_PickFacetCallback*
    New();

  vtkTypeMacro(asiUI_PickFacetCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

public:

  //! Sets accelerating structure to use for picking.
  //! \param bvh_facets [in] accelerating structure to set.
  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh_facets)
  {
    m_bvh = bvh_facets;
  }

  //! Sets Data Model instance to access the geometry to pick.
  //! \param model [in] Data Model instance.
  void SetModel(const Handle(asiEngine_Model)& model)
  {
    m_model = model;
  }

private:

  asiUI_EXPORT
    asiUI_PickFacetCallback(const Handle(asiAlgo_BVHFacets)& bvh_facets,
                            asiUI_Viewer*                    viewer);

  asiUI_EXPORT
    ~asiUI_PickFacetCallback();

private:

  Handle(asiAlgo_BVHFacets) m_bvh;      //!< Accelerating structure for picking.
  Handle(asiEngine_Model)   m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry      m_notifier; //!< Progress notifier.
  ActAPI_PlotterEntry       m_plotter;  //!< Imperative plotter.

};

#endif
