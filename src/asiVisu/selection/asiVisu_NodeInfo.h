//-----------------------------------------------------------------------------
// Created on: 16 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
  static asiVisu_NodeInfo* New();

public:

  void
    SetNodeId(const ActAPI_DataObjectId& nodeId);

  ActAPI_DataObjectId
    GetNodeId() const;

public:

  static vtkInformationObjectBaseKey*
    GetKey();

  static asiVisu_NodeInfo*
    Retrieve(vtkActor* actor);

  static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor*                  actor);

private:

  asiVisu_NodeInfo();
  asiVisu_NodeInfo(const asiVisu_NodeInfo&);
  asiVisu_NodeInfo& operator=(const asiVisu_NodeInfo&);
  ~asiVisu_NodeInfo();

private:

  //! Information key.
  static vtkSmartPointer<vtkInformationObjectBaseKey> m_key;

  //! Node ID wrapped for transferring through a pipeline.
  ActAPI_DataObjectId m_nodeId;

};

#endif
