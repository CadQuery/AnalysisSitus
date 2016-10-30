//-----------------------------------------------------------------------------
// Created on: 12 (*) April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_topo_item_prs.h>

// A-Situs (visualization) includes
#include <visu_shape_data_provider.h>
#include <visu_shape_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
visu_iv_topo_item_prs::visu_iv_topo_item_prs(const Handle(ActAPI_INode)& theNode)
: visu_iv_prs(theNode)
{
  // Create Data Provider
  Handle(visu_shape_data_provider)
    DP = new visu_shape_data_provider( theNode->GetId(),
                                       ActParamStream() << theNode->Parameter(visu_iv_topo_item_node::PID_Geometry) );

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new visu_shape_pipeline(false) );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_iv_topo_item_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_iv_topo_item_prs(theNode);
}
