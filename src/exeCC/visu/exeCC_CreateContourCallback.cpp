//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_CreateContourCallback.h>

// exeCC includes
#include <exeCC_CommonFacilities.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>

// asiUI includes
#include <asiUI_ViewerPart.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

//! Instantiation routine.
//! \return instance of the callback class.
exeCC_CreateContourCallback* exeCC_CreateContourCallback::New()
{
  return new exeCC_CreateContourCallback(NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
exeCC_CreateContourCallback::exeCC_CreateContourCallback(asiUI_Viewer* theViewer)
: asiUI_ViewerCallback(theViewer)
{}

//! Destructor.
exeCC_CreateContourCallback::~exeCC_CreateContourCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void exeCC_CreateContourCallback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                          unsigned long vtkNotUsed(theEventId),
                                          void*         theCallData)
{
  const vtkSmartPointer<asiVisu_PrsManager>& mgr = exeCC_CommonFacilities::Instance()->Prs.Part;

  // Get picking ray
  gp_Lin pickRay = *( (gp_Lin*) theCallData );

  /*ActAPI_PlotterEntry IV(exeCC_CommonFacilities::Instance()->Plotter);
  IV.DRAW_POINT( hit, Color_Red );*/

  // Prepare a tool to find the intersected facet
  asiAlgo_HitFacet HitFacet(m_bvh,
                            exeCC_CommonFacilities::Instance()->Notifier,
                            exeCC_CommonFacilities::Instance()->Plotter);

  // Find intersection
  gp_XYZ hit;
  int facet_idx;
  if ( !HitFacet(pickRay, facet_idx, hit) )
  {
    std::cout << "Error: cannot find the intersected facet" << std::endl;
    return;
  }

  Handle(asiData_ContourNode)
    contour_n = exeCC_CommonFacilities::Instance()->Model->GetPartNode()->GetContour();
  //
  exeCC_CommonFacilities::Instance()->Model->OpenCommand();
  {
    contour_n->AddPoint(hit, m_bvh->GetFacet(facet_idx).FaceIndex);
  }
  exeCC_CommonFacilities::Instance()->Model->CommitCommand();
  //
  mgr->Actualize( contour_n.get() );
}
