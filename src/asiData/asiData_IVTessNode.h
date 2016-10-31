//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_IVTessNode_h
#define asiData_IVTessNode_h

// Visualization includes
#include <asiData_IVTessItemNode.h>

//-----------------------------------------------------------------------------
// Data Node representing a set of tessellation items in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_IVTessNode, ActData_BaseNode)

//! Data Node representing a set of tessellation items in IV (Imperative Viewer).
class asiData_IVTessNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVTessNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVTessNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
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

// Handy accessors to the stored data:
public:

  Handle(asiData_IVTessItemNode) Item(const int oneBased_idx);

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_IVTessNode();

};

#endif
