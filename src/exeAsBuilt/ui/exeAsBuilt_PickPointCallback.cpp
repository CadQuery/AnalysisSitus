//-----------------------------------------------------------------------------
// Created on: 02 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_PickPointCallback.h>

// exeAsBuilt includes
#include <exeAsBuilt_CommonFacilities.h>

// GUI includes
#include <asiUI_Common.h>

// Visualization includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// exeAsBuilt includes (FlannKdTree should be included after Qt includes)
#include <exeAsBuilt_FlannKdTree.h>

//! Instantiation routine.
//! \return instance of the callback class.
exeAsBuilt_PickPointCallback* exeAsBuilt_PickPointCallback::New()
{
  return new exeAsBuilt_PickPointCallback(NULL);
}

//! Constructor accepting owning viewer as a parameter.
//! \param[in] viewer owning viewer.
exeAsBuilt_PickPointCallback::exeAsBuilt_PickPointCallback(asiUI_Viewer* viewer)
: asiUI_ViewerCallback(viewer)
{}

//! Destructor.
exeAsBuilt_PickPointCallback::~exeAsBuilt_PickPointCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void exeAsBuilt_PickPointCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                           unsigned long vtkNotUsed(eventId),
                                           void*         pCallData)
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get picked point
  gp_Pnt P = *( (gp_Pnt*) pCallData );

  ActAPI_PlotterEntry IV(exeAsBuilt_CommonFacilities::Instance()->Plotter);

  IV.DRAW_POINT(P, Color_Green);

  // Find neighbors
  std::vector<int> indices(10);
  std::vector<float> distances(10);
  //
  m_kdTree->Search(P, 10, indices, distances);

  std::cout << "test done" << std::endl;
}
