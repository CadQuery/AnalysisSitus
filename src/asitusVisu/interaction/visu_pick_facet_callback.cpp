//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_pick_facet_callback.h>

// Common includes
#include <common_facilities.h>

// Geometry includes
#include <geom_bvh_facets.h>
#include <geom_hit_facet.h>

// GUI includes
#include <gui_common.h>

// Visualization includes
#include <visu_prs_manager.h>
#include <visu_utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
visu_pick_facet_callback*
  visu_pick_facet_callback::New()
{
  return new visu_pick_facet_callback(NULL, NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param[in] bvh_facets accelerating structure for picking.
//! \param[in] viewer     owning viewer.
visu_pick_facet_callback::visu_pick_facet_callback(const Handle(geom_bvh_facets)& bvh_facets,
                                                   gui_viewer*                    viewer)
: visu_viewer_callback(viewer), m_bvh(bvh_facets)
{}

//! Destructor.
visu_pick_facet_callback::~visu_pick_facet_callback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void visu_pick_facet_callback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                       unsigned long theEventId,
                                       void*         theCallData)
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Get picking ray
  gp_Lin pickRay = *( (gp_Lin*) theCallData );

  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);

  // Prepare a tool to find the intersected facet
  geom_hit_facet HitFacet(m_bvh,
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
