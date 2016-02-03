//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef common_root_node_h
#define common_root_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_SelectionParameter.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Mesh Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(common_root_node, ActData_BaseNode)

//! Root Node in the hierarchy.
class common_root_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(common_root_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(common_root_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-----------------//
  // Common          //
  //-----------------//
    PID_Name,        //!< Name of the Node.
  //-----------------//
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
  common_root_node();

};

#endif
