//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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

  void Init(const Handle(Geom_Surface)& surface,
            const double                uLimit,
            const double                vLimit);

// Convenience methods:
public:

  void                 SetSurface (const Handle(Geom_Surface)& surface);
  Handle(Geom_Surface) GetSurface () const;
  void                 SetLimits  (const double uLimit, const double vLimit);
  void                 GetLimits  (double& uLimit, double& vLimit) const;

protected:

  //! Allocation is allowed only via Instance method.
  asiData_RESurfaceNode();

};

#endif
