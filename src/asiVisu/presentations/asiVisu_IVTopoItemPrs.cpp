//-----------------------------------------------------------------------------
// Created on: 12 (*) April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVTopoItemPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVTopoItemPrs::asiVisu_IVTopoItemPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_ShapeDataProvider)
    DP = new asiVisu_ShapeDataProvider( theNode->GetId(),
                                       ActParamStream() << theNode->Parameter(asiData_IVTopoItemNode::PID_Geometry) );

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new asiVisu_ShapePipeline(false) );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVTopoItemPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVTopoItemPrs(theNode);
}
