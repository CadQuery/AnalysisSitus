//-----------------------------------------------------------------------------
// Created on: 16 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_node_info_h
#define visu_node_info_h

// A-Situs includes
#include <analysis_situs.h>

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
class visu_node_info : public vtkObject
{
public:

  vtkTypeMacro(visu_node_info, vtkObject);
  static visu_node_info* New();

public:

  ASitus_EXPORT void
    SetNodeId(const ActAPI_DataObjectId& nodeId);

  ASitus_EXPORT ActAPI_DataObjectId
    GetNodeId() const;

public:

  ASitus_EXPORT static vtkInformationObjectBaseKey*
    GetKey();

  ASitus_EXPORT static visu_node_info*
    Retrieve(vtkActor* actor);

  ASitus_EXPORT static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor* actor);

private:

  visu_node_info();
  visu_node_info(const visu_node_info&);
  visu_node_info& operator=(const visu_node_info&);
  ~visu_node_info();

private:

  //! Information key.
  static vtkSmartPointer<vtkInformationObjectBaseKey> m_key;

  //! Node ID wrapped for transferring through a pipeline.
  ActAPI_DataObjectId m_nodeId;

};

#endif
