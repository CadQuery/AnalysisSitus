//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef geom_node_h
#define geom_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ShapeParameter.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Mesh Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_node, ActData_BaseNode)

//! Node representing b-rep geometry.
class geom_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(geom_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-----------------//
  // Common          //
  //-----------------//
    PID_Name,        //!< Name of the Node.
  //-----------------//
  // Geometry        //
  //-----------------//
    PID_Geometry,    //!< B-rep.
  //-----------------//
  // Presentation    //
  //-----------------//
    PID_DisplayMode, //!< Display mode.
    PID_HasColor,    //!< Indicates whether the Color Parameter is active.
    PID_Color,       //!< Color.
  //-----------------//
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

  TopoDS_Shape
    GetShape() const;

  void
    SetShape(const TopoDS_Shape& shape);

  void
    SetHasColor(const bool hasColor) const;

  bool
    HasColor() const;

  void
    SetColor(const int theColor) const;

  int
    GetColor() const;

  void
    SetDisplayMode(const int theMode) const;

  int
    GetDisplayMode() const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_node();

};

#endif
