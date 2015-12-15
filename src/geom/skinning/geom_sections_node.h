//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef geom_sections_node_h
#define geom_sections_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Sections Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_sections_node, ActData_BaseNode)

//! Node representing a collection of sections for skinning.
class geom_sections_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(geom_sections_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_sections_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Skinning result.
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

// Initialization:
public:

  void Init();

// Convenience methods:
public:

  TopoDS_Shape
    GetGeometry() const;

  void
    SetGeometry(const TopoDS_Shape& shape);

protected:

  //! Allocation is allowed only via Instance method.
  geom_sections_node();

};

#endif
