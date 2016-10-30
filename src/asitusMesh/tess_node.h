//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef tess_node_h
#define tess_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>
#include <ActData_MeshParameter.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Mesh Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(tess_node, ActData_BaseNode)

//! Node representing surface mesh (triangles and quads).
class tess_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(tess_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(tess_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-----------------//
  // Common          //
  //-----------------//
    PID_Name,        //!< Name of the Node.
  //-----------------//
  // Geometry        //
  //-----------------//
    PID_Mesh,        //!< Tessellation.
  //-----------------//
  // Presentation    //
  //-----------------//
    PID_DisplayMode, //!< Display mode.
    PID_HasColor,    //!< Indicates whether the Color Parameter is active.
    PID_Color,       //!< Color.
  //-----------------//
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

  Handle(Mesh)
    GetMesh() const;

  void
    SetMesh(const Handle(Mesh)& mesh);

  void
    SetHasColor(const bool hasColor) const;

  bool
    HasColor() const;

  void
    SetColor(const int theColor) const;

  int
    GetColor() const;

  void
    SetDisplayMode(const int theMode) const;

  int
    GetDisplayMode() const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  tess_node();

};

#endif
