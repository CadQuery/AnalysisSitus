//-----------------------------------------------------------------------------
// Created on: 16 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_NodeInfo_h
#define asiUI_NodeInfo_h

// A-Situs includes
#include <asiUI.h>

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
class asiUI_NodeInfo : public vtkObject
{
public:

  vtkTypeMacro(asiUI_NodeInfo, vtkObject);
  static asiUI_NodeInfo* New();

public:

  void
    SetNodeId(const ActAPI_DataObjectId& nodeId);

  ActAPI_DataObjectId
    GetNodeId() const;

public:

  static vtkInformationObjectBaseKey*
    GetKey();

  static asiUI_NodeInfo*
    Retrieve(vtkActor* actor);

  static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor*                  actor);

private:

  asiUI_NodeInfo();
  asiUI_NodeInfo(const asiUI_NodeInfo&);
  asiUI_NodeInfo& operator=(const asiUI_NodeInfo&);
  ~asiUI_NodeInfo();

private:

  //! Information key.
  static vtkSmartPointer<vtkInformationObjectBaseKey> m_key;

  //! Node ID wrapped for transferring through a pipeline.
  ActAPI_DataObjectId m_nodeId;

};

#endif
