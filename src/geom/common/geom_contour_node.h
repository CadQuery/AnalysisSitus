//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_contour_node_h
#define geom_contour_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Node representing a contour on shape
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_contour_node, ActData_BaseNode)

//! Node representing a contour on shape.
class geom_contour_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_contour_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_contour_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
  // Geometry         //
  //------------------//
    PID_Coords,       //!< Coordinates of the polyline nodes.
    PID_IsClosed,     //!< Indicates whether a contour is closed or not.
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

  void               SetCoords (const Handle(HRealArray)& coords);
  Handle(HRealArray) GetCoords () const;
  //void               AddPoint

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_contour_node();

};

#endif
