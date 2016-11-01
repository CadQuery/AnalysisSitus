//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PickFacetCallback.h>

// Common includes
#include <common_facilities.h>

// Geometry includes
#include <asiAlgo_BVHFacets.h>
#include <asiAlgo_HitFacet.h>

// GUI includes
#include <asiUI_Common.h>

// Visualization includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickFacetCallback*
  asiUI_PickFacetCallback::New()
{
  return new asiUI_PickFacetCallback(NULL, NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param[in] bvh_facets accelerating structure for picking.
//! \param[in] viewer     owning viewer.
asiUI_PickFacetCallback::asiUI_PickFacetCallback(const Handle(asiAlgo_BVHFacets)& bvh_facets,
                                                   asiUI_Viewer*                    viewer)
: asiUI_ViewerCallback(viewer), m_bvh(bvh_facets)
{}

//! Destructor.
asiUI_PickFacetCallback::~asiUI_PickFacetCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickFacetCallback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                       unsigned long theEventId,
                                       void*         theCallData)
{
  TopoDS_Shape part;
  if ( !asiUI_Common::PartShape(part) ) return;

  // Get picking ray
  gp_Lin pickRay = *( (gp_Lin*) theCallData );

  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  // Prepare a tool to find the intersected facet
  asiAlgo_HitFacet HitFacet(m_bvh,
                          common_facilities::Instance()->Notifier,
                          common_facilities::Instance()->Plotter);

  // Find intersection
  gp_XYZ hit;
  int facet_idx;
  if ( !HitFacet(pickRay, facet_idx, hit) )
    std::cout << "Error: cannot find the intersected facet" << std::endl;
  else
    IV.DRAW_POINT(hit, Color_Red);
}
