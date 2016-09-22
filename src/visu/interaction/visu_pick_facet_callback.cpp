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
#include <geom_find_nearest_facet.h>

// GUI includes
#include <gui_common.h>

// Visualization includes
#include <visu_prs_manager.h>
#include <visu_utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
visu_pick_facet_callback* visu_pick_facet_callback::New()
{
  return new visu_pick_facet_callback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
visu_pick_facet_callback* visu_pick_facet_callback::New(gui_viewer* theViewer)
{
  return new visu_pick_facet_callback(theViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
visu_pick_facet_callback::visu_pick_facet_callback(gui_viewer* theViewer)
: visu_viewer_callback(theViewer)
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

  const vtkSmartPointer<visu_prs_manager>& mgr = common_facilities::Instance()->Prs.Part;

  // Get hit position
  gp_XYZ hit = *( (gp_XYZ*) theCallData );

  /*ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);
  IV.DRAW_POINT( hit, Color_Red );*/

  // Build triangle set. Constructor will initialize the internal structures
  // storing the triangle nodes with references to the owning parts
  Handle(geom_bvh_facets)
    triangleSet = new geom_bvh_facets(part,
                                      common_facilities::Instance()->Notifier,
                                      common_facilities::Instance()->Plotter);
  //
  triangleSet->BVH(); // This invocation builds the BVH tree

  // Prepare a tool to find the nearest facet
  geom_find_nearest_facet nearest(triangleSet,
                                  common_facilities::Instance()->Notifier,
                                  common_facilities::Instance()->Plotter);

  // Find nearest
  int facet_idx;
  if ( !nearest(hit, facet_idx) )
  {
    std::cout << "Error: cannot find the nearest facet" << std::endl;
  }
}
