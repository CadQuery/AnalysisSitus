//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_create_contour_callback.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_viewer_part.h>

// A-Situs (visualization) includes
#include <visu_prs_manager.h>
#include <visu_utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
visu_create_contour_callback* visu_create_contour_callback::New()
{
  return new visu_create_contour_callback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
visu_create_contour_callback* visu_create_contour_callback::New(gui_viewer* theViewer)
{
  return new visu_create_contour_callback(theViewer);
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
  const vtkSmartPointer<visu_prs_manager>& mgr = this->Viewer()->PrsMgr();

  // Get hit position
  gp_XYZ hit = *( (gp_XYZ*) theCallData );

  ActAPI_PlotterEntry IV(common_facilities::Instance()->Plotter);
  IV.DRAW_POINT( hit, Color_Red );
}
