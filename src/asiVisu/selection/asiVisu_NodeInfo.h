//-----------------------------------------------------------------------------
// Created on: 16 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_NodeInfo_h
#define asiVisu_NodeInfo_h

// A-Situs includes
#include <asiVisu.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// VTK forward declarations
class vtkActor;
class vtkDataSet;
class vtkInformationObjectBaseKey;

//! VTK holder for Node ID which can be passed through visualization
//! pipelines in order to resolve Nodes by their corresponding actors.
class asiVisu_NodeInfo : public vtkObject
{
public:

  vtkTypeMacro(asiVisu_NodeInfo, vtkObject);

  asiVisu_EXPORT static asiVisu_NodeInfo*
    New();

public:

  asiVisu_EXPORT void
    SetNodeId(const ActAPI_DataObjectId& nodeId);

  asiVisu_EXPORT ActAPI_DataObjectId
    GetNodeId() const;

public:

  asiVisu_EXPORT static vtkInformationObjectBaseKey*
    GetKey();

  asiVisu_EXPORT static asiVisu_NodeInfo*
    Retrieve(vtkActor* actor);

  asiVisu_EXPORT static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor*                  actor);

protected:

  asiVisu_NodeInfo();
  asiVisu_NodeInfo(const asiVisu_NodeInfo&);
  asiVisu_NodeInfo& operator=(const asiVisu_NodeInfo&);
  ~asiVisu_NodeInfo();

protected:

  //! Information key.
  static vtkSmartPointer<vtkInformationObjectBaseKey> m_key;

  //! Node ID wrapped for transferring through a pipeline.
  ActAPI_DataObjectId m_nodeId;

};

#endif
