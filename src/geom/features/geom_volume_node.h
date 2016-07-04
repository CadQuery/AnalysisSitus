//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_volume_node_h
#define geom_volume_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Volume Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_volume_node, ActData_BaseNode)

//! Node representing b-rep solid (part or feature).
class geom_volume_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_volume_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_volume_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //----------------------//
    PID_Name,             //!< Name of the Node.
    PID_Geometry,         //!< B-rep model.
    PID_Features2Keep,    //!< Features to keep.
    PID_Features2Exclude, //!< Features to exclude.
  //----------------------//
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

  void                           SetShape            (const TopoDS_Shape& shape);
  TopoDS_Shape                   GetShape            () const;
  void                           SetFeatures2Keep    (const Handle(ActAPI_HDataCursorList)& items);
  Handle(ActAPI_HDataCursorList) GetFeatures2Keep    () const;
  void                           SetFeatures2Exclude (const Handle(ActAPI_HDataCursorList)& items);
  Handle(ActAPI_HDataCursorList) GetFeatures2Exclude () const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_volume_node();

};

#endif
