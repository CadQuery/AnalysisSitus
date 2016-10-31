//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_tess_item_prs.h>

// A-Situs (visualization) includes
#include <visu_mesh_contour_pipeline.h>
#include <visu_mesh_data_provider.h>
#include <visu_mesh_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
visu_iv_tess_item_prs::visu_iv_tess_item_prs(const Handle(ActAPI_INode)& theNode)
: visu_iv_prs(theNode)
{
  // Create Data Provider
  Handle(visu_mesh_data_provider)
    DP = new visu_mesh_data_provider( theNode->GetId(),
                                      ActParamStream() << theNode->Parameter(asiData_IVTessItemNode::PID_Mesh) );

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new visu_mesh_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Pipeline for mesh contour
  this->addPipeline(Pipeline_MeshContour, new visu_mesh_contour_pipeline);
  this->assignDataProvider(Pipeline_MeshContour, DP);

  // We use CONTOUR mesh pipeline along with an ordinary one. Thus it is
  // really necessary to resolve coincident primitives to avoid blinking
  // on mesh edges
  vtkMapper::SetResolveCoincidentTopology(1);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_iv_tess_item_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_iv_tess_item_prs(theNode);
}
