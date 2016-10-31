//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_section_node_h
#define geom_section_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Section Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_section_node, ActData_BaseNode)

//! Node representing a single section for skinning.
class geom_section_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_section_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_section_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Geometry of a section.
    PID_Tx,           //!< Transformation Tx.
    PID_Ty,           //!< Transformation Ty.
    PID_Tz,           //!< Transformation Tz.
    PID_Rx,           //!< Transformation Rx.
    PID_Ry,           //!< Transformation Ry.
    PID_Rz,           //!< Transformation Rz.
  //------------------//
    PID_CircleRadius, //!< Radius (for circular sections only).
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

  void Init();

public:

  TopoDS_Shape GetGeometry() const;

  void SetGeometry(const TopoDS_Shape& geometry);

  void GetTransform(double& Tx, double& Ty, double& Tz,
                    double& Rx, double& Ry, double& Rz) const;

  void SetTransform(const double Tx, const double Ty, const double Tz,
                    const double Rx, const double Ry, const double Rz);

  double GetRadius() const;

  void SetRadius(const double radius);

protected:

  //! Allocation is allowed only via Instance method.
  geom_section_node();

};

#endif
