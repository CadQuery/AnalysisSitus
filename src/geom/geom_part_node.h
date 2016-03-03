//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_part_node_h
#define geom_part_node_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (geometry) includes
#include <geom_face_node.h>
#include <geom_surf_node.h>

// Active Data includes
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_part_node, ActData_BaseNode)

//! Node representing b-rep part.
class geom_part_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_part_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_part_node, Instance)

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
    PID_Geometry,     //!< B-rep model.
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

  void                   SetShape              (const TopoDS_Shape& shape);
  TopoDS_Shape           GetShape              ()                           const;
  void                   SetHasColor           (const bool hasColor);
  bool                   HasColor              ()                           const;
  void                   SetColor              (const int color);
  int                    GetColor              ()                           const;
  void                   SetDisplayMode        (const int mode);
  int                    GetDisplayMode        ()                           const;
  Handle(geom_face_node) FaceRepresentation    ();
  Handle(geom_surf_node) SurfaceRepresentation ();

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_part_node();

};

#endif
