//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_IVCurveNode_h
#define asiData_IVCurveNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Curve.hxx>

//-----------------------------------------------------------------------------
// Data Node representing a single curve in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_IVCurveNode, ActData_BaseNode)

//! Data Node representing a single curve in IV (Imperative Viewer).
class asiData_IVCurveNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVCurveNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVCurveNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Stored geometry.
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
    SetName(const TCollection_ExtendedString& theName);

// Handy accessors to the stored data:
public:

  asiData_EXPORT Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  asiData_EXPORT void
    SetCurve(const Handle(Geom_Curve)& curve,
             const double              f,
             const double              l);

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_IVCurveNode();

};

#endif
