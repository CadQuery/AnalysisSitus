//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_ubend_laws_node_h
#define geom_ubend_laws_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Container for evolution laws in U-bend Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_ubend_laws_node, ActData_BaseNode)

//! Node representing u-bend geometry.
class geom_ubend_laws_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_ubend_laws_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_ubend_laws_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //
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
  geom_ubend_laws_node();

};

#endif
