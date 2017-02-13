//-----------------------------------------------------------------------------
// Created on: 13 February 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartNodeInfo_h
#define asiVisu_PartNodeInfo_h

// A-Situs includes
#include <asiVisu_NodeInfo.h>

//! This is a sub-class for generic asiVisu_NodeInfo. Use this key to
//! distinguish between part presentations and other presentations bounded
//! to Active Data Nodes.
class asiVisu_PartNodeInfo : public asiVisu_NodeInfo
{
public:

  vtkTypeMacro(asiVisu_PartNodeInfo, vtkObject);

  asiVisu_EXPORT static asiVisu_PartNodeInfo*
    New();

public:

  asiVisu_EXPORT static vtkInformationObjectBaseKey*
    GetKey();

  asiVisu_EXPORT static asiVisu_PartNodeInfo*
    Retrieve(vtkActor* actor);

  asiVisu_EXPORT static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor*                  actor);

private:

  asiVisu_PartNodeInfo();
  asiVisu_PartNodeInfo(const asiVisu_PartNodeInfo&);
  asiVisu_PartNodeInfo& operator=(const asiVisu_PartNodeInfo&);
  ~asiVisu_PartNodeInfo();

};

#endif
