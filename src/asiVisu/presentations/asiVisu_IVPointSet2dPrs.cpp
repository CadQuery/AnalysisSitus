//-----------------------------------------------------------------------------
// Created on: 16 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVPointSet2dPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_IVPointSet2dDataProvider.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVPointSet2dPrs::asiVisu_IVPointSet2dPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_IVPointSet2dDataProvider)
    DP = new asiVisu_IVPointSet2dDataProvider(theNode);

  // Pipeline for points
  this->addPipeline        ( Pipeline_Main, new asiVisu_PointsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVPointSet2dPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVPointSet2dPrs(theNode);
}
