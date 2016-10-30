//-----------------------------------------------------------------------------
// Created on: 22 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_boundary_edges_node_h
#define geom_boundary_edges_node_h

// A-Situs includes
#include <asitusGeom.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_boundary_edges_node, ActData_BaseNode)

//! Node representing b-rep boundary edges.
class geom_boundary_edges_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_boundary_edges_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_boundary_edges_node, Instance)

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
    PID_Green,        //!< "Green" (good) edges.
    PID_Red,          //!< "Red" (bad) edges.
    PID_Ordinary,     //!< "Ordinary" (nothing special) edges.
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
    SetName(const TCollection_ExtendedString& theName);

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_boundary_edges_node();

};

#endif
