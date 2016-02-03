//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef geom_ubend_node_h
#define geom_ubend_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_ubend_node, ActData_BaseNode)

//! Node representing u-bend geometry.
class geom_ubend_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_ubend_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_ubend_node, Instance)

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
    PID_Path,         //!< Path geometry.
    PID_Sections,     //!< Geometry of sections.
    PID_Skin,         //!< Skin geometry.
  //------------------//
  // Presentation     //
  //------------------//
    PID_DisplayMode,  //!< Display mode.
    PID_HasColor,     //!< Indicates whether the Color Parameter is active.
    PID_Color,        //!< Color.
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

  void         SetPath        (const TopoDS_Shape& shape);
  TopoDS_Shape GetPath        ()                           const;
  void         SetSections    (const TopoDS_Shape& shape);
  TopoDS_Shape GetSections    ()                           const;
  void         SetSkin        (const TopoDS_Shape& shape);
  TopoDS_Shape GetSkin        ()                           const;
  void         SetHasColor    (const bool hasColor);
  bool         HasColor       ()                           const;
  void         SetColor       (const int color);
  int          GetColor       ()                           const;
  void         SetDisplayMode (const int mode);
  int          GetDisplayMode ()                           const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_ubend_node();

};

#endif
