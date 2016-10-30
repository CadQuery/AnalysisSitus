//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_ubend_law_node_h
#define geom_ubend_law_node_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (calculus) includes
#include <calculus_design_law_node.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_ubend_law_node, ActData_BaseNode)

//! Node representing a single law for u-bend geometry.
class geom_ubend_law_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_ubend_law_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_ubend_law_node, Instance)

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

// Handy accessors:
public:

  Handle(calculus_design_law_node) GetLaw_X() const;
  Handle(calculus_design_law_node) GetLaw_Y() const;

protected:

  //! Allocation is allowed only via Instance method.
  geom_ubend_law_node();

};

#endif
