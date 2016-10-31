//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_volume_prs.h>

// A-Situs (visualization) includes
#include <visu_shape_data_provider.h>
#include <visu_shape_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
visu_geom_volume_prs::visu_geom_volume_prs(const Handle(ActAPI_INode)& theNode)
: visu_iv_prs(theNode)
{
  // Create Data Provider
  Handle(visu_shape_data_provider)
    DP = new visu_shape_data_provider( theNode->GetId(),
                                       ActParamStream() << theNode->Parameter(geom_volume_node::PID_Geometry) );

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new visu_shape_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_volume_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_volume_prs(theNode);
}
