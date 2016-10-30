//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_re_contours_node_h
#define geom_re_contours_node_h

// Geometry includes
#include <geom_re_contour_node.h>

//-----------------------------------------------------------------------------
// Contours for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_re_contours_node, ActData_BaseNode)

//! Node representing a collection of contours for reverse engineering.
class geom_re_contours_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_re_contours_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_re_contours_node, Instance)

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

  Handle(geom_re_contour_node)
    Contour(const int oneBased_idx);

  Handle(geom_re_contour_node)
    TX_AddContour(const TopoDS_Wire& contour);

  void
    TX_CleanContours();

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_re_contours_node();

};

#endif
