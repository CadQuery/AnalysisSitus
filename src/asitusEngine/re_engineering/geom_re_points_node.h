//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_re_points_node_h
#define geom_re_points_node_h

// Geometry includes
#include <geom_point_cloud.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Points for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_re_points_node, ActData_BaseNode)

//! Node representing a point cloud for reverse engineering.
class geom_re_points_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_re_points_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_re_points_node, Instance)

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

  static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  void Init();

// Generic naming support:
public:

  virtual TCollection_ExtendedString
    GetName();

  virtual void
    SetName(const TCollection_ExtendedString& theName);

// Handy accessors to the stored data:
public:

  Handle(geom_point_cloud)
    GetPoints() const;

  void
    TX_SetPoints(const Handle(geom_point_cloud)& points);

protected:

  //! Allocation is allowed only via Instance method.
  geom_re_points_node();

};

#endif
