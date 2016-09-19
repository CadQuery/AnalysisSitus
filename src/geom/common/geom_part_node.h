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
#include <geom_boundary_edges_node.h>
#include <geom_contour_node.h>
#include <geom_curve_node.h>
#include <geom_edge_node.h>
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
    PID_Geometry,     //!< B-Rep model.
  //------------------//
  // Presentation     //
  //------------------//
    PID_DisplayMode,  //!< Display mode.
    PID_HasColor,     //!< Indicates whether the Color Parameter is active.
    PID_Color,        //!< Color.
    PID_HasVertices,  //!< Indicates whether vertices are active.
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
    SetName(const TCollection_ExtendedString& );

// Handy accessors to the stored data:
public:

  void         SetShape       (const TopoDS_Shape&);
  TopoDS_Shape GetShape       () const;
  void         SetHasColor    (const bool);
  bool         HasColor       () const;
  void         SetColor       (const int);
  int          GetColor       () const;
  void         SetDisplayMode (const int);
  int          GetDisplayMode () const;
  void         SetHasVertices (const bool);
  bool         HasVertices    () const;

  //-------------------------------------------------------------------------//

  Handle(geom_face_node)           GetFaceRepresentation          () const;
  Handle(geom_surf_node)           GetSurfaceRepresentation       () const;
  Handle(geom_edge_node)           GetEdgeRepresentation          () const;
  Handle(geom_curve_node)          GetCurveRepresentation         () const;
  Handle(geom_boundary_edges_node) GetBoundaryEdgesRepresentation () const;
  Handle(geom_contour_node)        GetContour                     () const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_part_node();

};

#endif
