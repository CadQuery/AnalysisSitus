//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_REPointsNode_h
#define asiData_REPointsNode_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_PointCloud.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Points for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_REPointsNode, ActData_BaseNode)

//! Node representing a point cloud for reverse engineering.
class asiData_REPointsNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_REPointsNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_REPointsNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Points,       //!< Point cloud (array of coordinates).
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
    SetName(const TCollection_ExtendedString& theName);

// Handy accessors to the stored data:
public:

  asiData_EXPORT Handle(asiAlgo_PointCloud<double>)
    GetPoints() const;

  asiData_EXPORT Handle(asiAlgo_PointCloud<float>)
    GetPointsf() const;

  asiData_EXPORT void
    SetPoints(const Handle(asiAlgo_PointCloud<double>)& points);

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_REPointsNode();

};

#endif
