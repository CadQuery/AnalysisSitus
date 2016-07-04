//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_tess_node_h
#define visu_iv_tess_node_h

// Visualization includes
#include <visu_iv_tess_item_node.h>

//-----------------------------------------------------------------------------
// Data Node representing a set of tessellation items in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_iv_tess_node, ActData_BaseNode)

//! Data Node representing a set of tessellation items in IV (Imperative Viewer).
class visu_iv_tess_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_tess_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(visu_iv_tess_node, Instance)

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

  Handle(visu_iv_tess_item_node) Item(const int oneBased_idx);

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  visu_iv_tess_node();

};

#endif
