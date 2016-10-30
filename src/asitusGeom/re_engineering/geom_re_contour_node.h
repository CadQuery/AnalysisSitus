//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_re_contour_node_h
#define geom_re_contour_node_h

// A-Situs includes
#include <asitusGeom.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <TopoDS_Wire.hxx>

//-----------------------------------------------------------------------------
// Single contour for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_re_contour_node, ActData_BaseNode)

//! Node representing a single contour for reverse engineering.
class geom_re_contour_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_re_contour_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_re_contour_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Contour,      //!< Geometry.
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

// Initialization:
public:

  void Init(const TopoDS_Wire& contour);

// Convenience methods:
public:

  void        SetContour(const TopoDS_Wire& contour);
  TopoDS_Wire GetContour() const;

protected:

  //! Allocation is allowed only via Instance method.
  geom_re_contour_node();

};

#endif
