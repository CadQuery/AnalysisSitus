//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_surface_node_h
#define visu_iv_surface_node_h

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

//-----------------------------------------------------------------------------
// Data Node representing a single surface in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_iv_surface_node, ActData_BaseNode)

//! Data Node representing a single surface in IV (Imperative Viewer).
class visu_iv_surface_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_surface_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(visu_iv_surface_node, Instance)

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

// Handy accessors to the stored data:
public:

  Handle(Geom_Surface) GetSurface () const;
  void                 SetSurface (const Handle(Geom_Surface)& surface);
  void                 SetLimits  (const double uLimit, const double vLimit);
  void                 GetLimits  (double& uLimit, double& vLimit) const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  visu_iv_surface_node();

};

#endif
