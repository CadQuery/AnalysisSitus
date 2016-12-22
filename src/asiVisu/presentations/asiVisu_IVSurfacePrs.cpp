//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVSurfacePrs.h>

// A-Situs (visualization) includes
#include <asiVisu_IVSurfacePipeline.h>
#include <asiVisu_IVSurfaceDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVSurfacePrs::asiVisu_IVSurfacePrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_IVSurfaceDataProvider) DP = new asiVisu_IVSurfaceDataProvider(theNode);

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new asiVisu_IVSurfacePipeline );
  this->assignDataProvider ( Pipeline_Main, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVSurfacePrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVSurfacePrs(theNode);
}
