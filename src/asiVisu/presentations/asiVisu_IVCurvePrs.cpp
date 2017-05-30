//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_IVCurvePrs.h>

// A-Situs (visualization) includes
#include <asiVisu_CurvePipeline.h>
#include <asiVisu_IVCurveDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVCurvePrs::asiVisu_IVCurvePrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_IVCurveDataProvider) DP = new asiVisu_IVCurveDataProvider(theNode);

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new asiVisu_CurvePipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Adjust line width
  this->GetPipeline(Pipeline_Main)->Actor()->GetProperty()->SetLineWidth(3.0f);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVCurvePrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVCurvePrs(theNode);
}
