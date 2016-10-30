//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_create_contour_callback.h>

// Common includes
#include <common_facilities.h>

// Geometry includes
#include <geom_hit_facet.h>

// GUI includes
#include <gui_viewer_part.h>

// Visualization includes
#include <visu_prs_manager.h>
#include <visu_utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
visu_create_contour_callback* visu_create_contour_callback::New()
{
  return new visu_create_contour_callback(NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
visu_create_contour_callback::visu_create_contour_callback(gui_viewer* theViewer)
: visu_viewer_callback(theViewer)
{}

//! Destructor.
visu_create_contour_callback::~visu_create_contour_callback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void visu_create_contour_callback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                           unsigned long theEventId,
                                           void*         theCallData)
{
  const vtkSmartPointer<visu_prs_manager>& mgr = common_facilities::Instance()->Prs.Part;

  // Get picking ray
  gp_Lin pickRay = *( (gp_Lin*) theCallData );

  /*ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);
  IV.DRAW_POINT( hit, Color_Red );*/

  // Prepare a tool to find the intersected facet
  geom_hit_facet HitFacet(m_bvh,
                          common_facilities::Instance()->Notifier,
                          common_facilities::Instance()->Plotter);

  // Find intersection
  gp_XYZ hit;
  int facet_idx;
  if ( !HitFacet(pickRay, facet_idx, hit) )
  {
    std::cout << "Error: cannot find the intersected facet" << std::endl;
    return;
  }

  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    contour_n->AddPoint(hit, m_bvh->GetFacet(facet_idx).FaceIndex);
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  mgr->Actualize( contour_n.get() );
}
