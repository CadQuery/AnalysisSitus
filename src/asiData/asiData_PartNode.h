//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_PartNode_h
#define asiData_PartNode_h

// A-Situs includes
#include <asiData.h>

// A-Situs (geometry) includes
#include <asiData_BoundaryEdgesNode.h>
#include <asiData_ContourNode.h>
#include <asiData_CurveNode.h>
#include <asiData_EdgeNode.h>
#include <asiData_FaceNode.h>
#include <asiData_SurfNode.h>

// Active Data includes
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_PartNode, ActData_BaseNode)

//! Node representing b-rep part.
class asiData_PartNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_PartNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_PartNode, Instance)

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

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& );

// Handy accessors to the stored data:
public:

  asiData_EXPORT void
    SetShape(const TopoDS_Shape&);

  asiData_EXPORT TopoDS_Shape
    GetShape() const;

  asiData_EXPORT void
    SetHasColor(const bool);

  asiData_EXPORT bool
    HasColor() const;

  asiData_EXPORT void
    SetColor(const int);

  asiData_EXPORT int
    GetColor() const;

  asiData_EXPORT void
    SetDisplayMode(const int);

  asiData_EXPORT int
    GetDisplayMode() const;

  asiData_EXPORT void
    SetHasVertices(const bool);

  asiData_EXPORT bool
    HasVertices() const;

  //-------------------------------------------------------------------------//

  asiData_EXPORT Handle(asiData_FaceNode)
    GetFaceRepresentation() const;

  asiData_EXPORT Handle(asiData_SurfNode)
    GetSurfaceRepresentation() const;

  asiData_EXPORT Handle(asiData_EdgeNode)
    GetEdgeRepresentation() const;

  asiData_EXPORT Handle(asiData_CurveNode)
    GetCurveRepresentation() const;

  asiData_EXPORT Handle(asiData_BoundaryEdgesNode)
    GetBoundaryEdgesRepresentation() const;

  asiData_EXPORT Handle(asiData_ContourNode)
    GetContour() const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_PartNode();

};

#endif
