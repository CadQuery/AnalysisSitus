//-----------------------------------------------------------------------------
// Created on: 12 (*) April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVTopoItemPrs_h
#define asiVisu_IVTopoItemPrs_h

// Visualization includes
#include <asiVisu_IVPrs.h>
#include <asiData_IVTopoItemNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVTopoItemPrs, asiVisu_IVPrs)

//! Presentation class for topological items in IV.
class asiVisu_IVTopoItemPrs : public asiVisu_IVPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVTopoItemPrs, asiVisu_IVPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_IVTopoItemNode, Instance)

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
  asiVisu_IVTopoItemPrs(const Handle(ActAPI_INode)& theNode);

};

#endif
