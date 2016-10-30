//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_edge_node_h
#define geom_edge_node_h

// A-Situs includes
#include <asitusGeom.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Individual edge Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_edge_node, ActData_BaseNode)

//! Node representing a single b-rep edge.
class geom_edge_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_edge_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_edge_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
  // Geometry         //
  //------------------//
    PID_SelectedEdge, //!< ID of the selected edge.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  virtual TCollection_ExtendedString
    GetName();

  virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  void SetSelectedEdge (const int edgeId);
  int  GetSelectedEdge ()                  const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_edge_node();

};

#endif
