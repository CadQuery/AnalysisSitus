//-----------------------------------------------------------------------------
// Created on: 19 September 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_ContourNode_h
#define asiData_ContourNode_h

// A-Situs includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopoDS_Wire.hxx>

//-----------------------------------------------------------------------------
// Node representing a contour on shape
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_ContourNode, ActData_BaseNode)

//! This Node represents a contour on a shape. Initially a contour is defined
//! by a series of vertices connected with straight line segments. Such
//! a contour does not have any associated B-Rep, and AsShape() method will
//! construct it on flight whenever requested. However, it is also possible
//! to cache B-Rep geometry within this Node in order to have it explicitly.
//! The latter not only makes the data access faster (as the mentioned on-flight
//! construction will not happen), but also allows storing the alternative
//! geometry like curved representations instead the originally defined
//! polyline. The alternative geometry can be generated by some post-processing
//! algorithms, e.g. smoothing tools, healers, etc. It should be noted that
//! associating this alternative geometry to the Node does not affect the
//! original polyline definition. The latter makes possible to keep track
//! of contour modification, and to use the original contour definition for
//! validation of dependent algorithms (e.g. contour capture).
class asiData_ContourNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_ContourNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_ContourNode, Instance)

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
    PID_Faces,        //!< Host faces (one-to-one relation between point and face index).
    PID_IsClosed,     //!< Indicates whether a contour is closed or not.
    PID_Geometry,     //!< Explicitly defined geometry.
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
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  asiData_EXPORT void
    SetCoords(const Handle(HRealArray)& coords);

  asiData_EXPORT Handle(HRealArray)
    GetCoords() const;

  asiData_EXPORT void
    ReplacePoint(const int     zeroBasedIndex,
                 const gp_Pnt& point);

  asiData_EXPORT void
    SetFaces(const Handle(HIntArray)& indices);

  asiData_EXPORT Handle(HIntArray)
    GetFaces() const;

  asiData_EXPORT void
    AddPoint(const gp_XYZ& point,
             const int     face_idx);

  asiData_EXPORT void
    SetClosed(const bool isClosed);

  asiData_EXPORT bool
    IsClosed() const;

  asiData_EXPORT TopoDS_Wire
    AsShape(const bool useCache = true) const;

  asiData_EXPORT void
    AsPointsOnFaces(TColgp_SequenceOfPnt&      points,
                    TColStd_SequenceOfInteger& faces) const;

  asiData_EXPORT void
    SetPointsOnFaces(const TColgp_SequenceOfPnt&      points,
                     const TColStd_SequenceOfInteger& faces);

  asiData_EXPORT void
    SetGeometry(const TopoDS_Shape& shape);

  asiData_EXPORT TopoDS_Shape
    GetGeometry() const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_ContourNode();

};

#endif
