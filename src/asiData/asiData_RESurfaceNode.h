//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_RESurfaceNode_h
#define asiData_RESurfaceNode_h

// A-Situs includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

//-----------------------------------------------------------------------------
// Single surface for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_RESurfaceNode, ActData_BaseNode)

//! Node representing a single surface for reverse engineering.
class asiData_RESurfaceNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_RESurfaceNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_RESurfaceNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Surface,      //!< Geometry.
    PID_ULimit,       //!< Value to limit possibly infinite domain in U (absolute).
    PID_VLimit,       //!< Value to limit possibly infinite domain in V (absolute).
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

// Initialization:
public:

  asiData_EXPORT void
    Init(const Handle(Geom_Surface)& surface,
         const double                uLimit,
         const double                vLimit);

// Convenience methods:
public:

  asiData_EXPORT void
    SetSurface(const Handle(Geom_Surface)& surface);

  asiData_EXPORT Handle(Geom_Surface)
    GetSurface() const;

  asiData_EXPORT void
    SetLimits(const double uLimit, const double vLimit);

  asiData_EXPORT void
    GetLimits(double& uLimit, double& vLimit) const;

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_RESurfaceNode();

};

#endif
