//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVSurfacePrs_h
#define asiVisu_IVSurfacePrs_h

// Visualization includes
#include <asiData_IVSurfaceNode.h>
#include <asiVisu_IVPrs.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVSurfacePrs, asiVisu_IVPrs)

//! Presentation class for surfaces in IV.
class asiVisu_IVSurfacePrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVSurfacePrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVSurfaceNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_IVSurfacePrs(const Handle(ActAPI_INode)& theNode);

};

#endif
