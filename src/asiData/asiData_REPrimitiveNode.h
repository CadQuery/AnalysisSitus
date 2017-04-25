//-----------------------------------------------------------------------------
// Created on: 19 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_REPrimitiveNode_h
#define asiData_REPrimitiveNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

//-----------------------------------------------------------------------------
// Topological primitive as a result of Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_REPrimitiveNode, ActData_BaseNode)

//! Node representing a single topological primitive being a result of
//! reverse engineering.
class asiData_REPrimitiveNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_REPrimitiveNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_REPrimitiveNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Geometry.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Initialization:
public:

  asiData_EXPORT void
    Init(const TopoDS_Shape& primitive);

// Convenience methods:
public:

  asiData_EXPORT void
    SetShape(const TopoDS_Shape& primitive);

  asiData_EXPORT TopoDS_Shape
    GetShape() const;

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_REPrimitiveNode();

};

#endif
