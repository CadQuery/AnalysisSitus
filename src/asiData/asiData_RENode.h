//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_RENode_h
#define asiData_RENode_h

// Geometry includes
#include <asiData_REContoursNode.h>
#include <asiData_REPointsNode.h>
#include <asiData_RESurfacesNode.h>

//-----------------------------------------------------------------------------
// Reverse Engineering Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_RENode, ActData_BaseNode)

//! Node representing reverse engineering data.
class asiData_RENode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_RENode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_RENode, Instance)

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

  Handle(asiData_RESurfacesNode) Surfaces();
  Handle(asiData_REContoursNode) Contours();
  Handle(asiData_REPointsNode)   Points();

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_RENode();

private:

  enum Child
  {
    Child_Surfaces = 1,
    Child_Contours,
    Child_Points
  };

};

#endif
