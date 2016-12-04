//-----------------------------------------------------------------------------
// Created on: 04 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_PickTangentPlaneCallback.h>

// exeAsBuilt includes
#include <exeAsBuilt_CommonFacilities.h>

// asiAlgo includes
#include <asiAlgo_PlaneOnPoints.h>

// asiUI includes
#include <asiUI_Common.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <Geom_Plane.hxx>

// exeAsBuilt includes (FlannKdTree should be included after Qt includes)
#include <exeAsBuilt_FlannKdTree.h>

//-----------------------------------------------------------------------------

#define KDTREE_K 20

//-----------------------------------------------------------------------------

//! Instantiation routine.
//! \return instance of the callback class.
exeAsBuilt_PickTangentPlaneCallback* exeAsBuilt_PickTangentPlaneCallback::New()
{
  return new exeAsBuilt_PickTangentPlaneCallback(NULL);
}

//-----------------------------------------------------------------------------

//! Constructor accepting owning viewer as a parameter.
//! \param[in] viewer owning viewer.
exeAsBuilt_PickTangentPlaneCallback::exeAsBuilt_PickTangentPlaneCallback(asiUI_Viewer* viewer)
: asiUI_ViewerCallback(viewer)
{}

//-----------------------------------------------------------------------------

//! Destructor.
exeAsBuilt_PickTangentPlaneCallback::~exeAsBuilt_PickTangentPlaneCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void exeAsBuilt_PickTangentPlaneCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                                  unsigned long vtkNotUsed(eventId),
                                                  void*         pCallData)
{
  Handle(exeAsBuilt_CommonFacilities) cf = exeAsBuilt_CommonFacilities::Instance();
  Handle(asiData_PartNode)            part_n;
  TopoDS_Shape                        part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  // Get picked point
  gp_Pnt P = *( (gp_Pnt*) pCallData );

  ActAPI_PlotterEntry IV(cf->Plotter);

  IV.DRAW_POINT(P, Color_Green);

  // Find neighbors
  std::vector<gp_Pnt> points(KDTREE_K);
  std::vector<int>    indices(KDTREE_K);
  std::vector<float>  distances(KDTREE_K);
  //
  m_kdTree->Search(P, KDTREE_K, indices, distances);

  // Get neighbor points
  const Handle(asiAlgo_PointCloud<float>)& pointCloud = m_kdTree->GetPointCloud();
  //
  for ( size_t k = 0; k < KDTREE_K; ++k )
  {
    float x, y, z;
    pointCloud->GetPoint(indices[k], x, y, z);
    points[k] = gp_Pnt(x, y, z);

    IV.DRAW_POINT(points[k], Color_Yellow);
  }

  // Build a fitting plane
  gp_Pln plane;
  asiAlgo_PlaneOnPoints mkPlaneOnPoints(cf->Notifier, cf->Plotter);
  //
  if ( !mkPlaneOnPoints.Build(points, plane) )
  {
    std::cout << "Error: cannot build fitting plane" << std::endl;
    return;
  }

  // U and V bounds for the drawable surface are chosen with respect to the
  // max distance from the seed point to its neighbors
  const double uvLimit = distances[distances.size() - 1]*0.1;
  IV.DRAW_SURFACE(new Geom_Plane(plane), uvLimit, uvLimit, Color_Green);
}
