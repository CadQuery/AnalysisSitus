//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_node_h
#define visu_iv_node_h

// Visualization includes
#include <visu_iv_curves_node.h>
#include <visu_iv_points_2d_node.h>
#include <visu_iv_points_node.h>
#include <visu_iv_surfaces_node.h>
#include <visu_iv_tess_node.h>
#include <visu_iv_text_node.h>
#include <visu_iv_topo_node.h>

//-----------------------------------------------------------------------------
// Root Node for all items visualized in the imperative viewer
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_iv_node, ActData_BaseNode)

//! Root Node for all items visualized in the imperative viewer.
class visu_iv_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(visu_iv_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
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

  Handle(visu_iv_points_2d_node) Points2d();
  Handle(visu_iv_points_node)    Points();
  Handle(visu_iv_curves_node)    Curves();
  Handle(visu_iv_surfaces_node)  Surfaces();
  Handle(visu_iv_topo_node)      Topology();
  Handle(visu_iv_tess_node)      Tessellation();
  Handle(visu_iv_text_node)      Text();

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  visu_iv_node();

private:

  enum Child
  {
    Child_Points2d = 1,
    Child_Points,
    Child_Curves,
    Child_Surfaces,
    Child_Topology,
    Child_Tessellation,
    Child_Text
  };

};

#endif
