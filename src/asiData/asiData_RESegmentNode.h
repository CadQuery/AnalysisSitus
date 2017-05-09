//-----------------------------------------------------------------------------
// Created on: 01 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_RESegmentNode_h
#define asiData_RESegmentNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <TColStd_HPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------
// Node for a single point cloud segment (region)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_RESegmentNode, ActData_BaseNode)

//! Node for a point cloud segment.
class asiData_RESegmentNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_RESegmentNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_RESegmentNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Indices,      //!< Indices of the points in a segment.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  asiData_EXPORT void
    Init();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// API:
public:

  asiData_EXPORT Handle(TColStd_HPackedMapOfInteger)
    GetIndices() const;

  asiData_EXPORT void
    SetIndices(const Handle(TColStd_HPackedMapOfInteger)& indices);

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_RESegmentNode();

};

#endif
