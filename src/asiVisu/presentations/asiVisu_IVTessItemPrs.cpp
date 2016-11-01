//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVTessItemPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_MeshContourPipeline.h>
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVTessItemPrs::asiVisu_IVTessItemPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_MeshDataProvider)
    DP = new asiVisu_MeshDataProvider( theNode->GetId(),
                                      ActParamStream() << theNode->Parameter(asiData_IVTessItemNode::PID_Mesh) );

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new asiVisu_MeshPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Pipeline for mesh contour
  this->addPipeline(Pipeline_MeshContour, new asiVisu_MeshContourPipeline);
  this->assignDataProvider(Pipeline_MeshContour, DP);

  // We use CONTOUR mesh pipeline along with an ordinary one. Thus it is
  // really necessary to resolve coincident primitives to avoid blinking
  // on mesh edges
  vtkMapper::SetResolveCoincidentTopology(1);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVTessItemPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVTessItemPrs(theNode);
}
