//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef geom_surf_node_h
#define geom_surf_node_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Host surface Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_surf_node, ActData_BaseNode)

//! Node representing a host surface of a single b-rep face.
class geom_surf_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(geom_surf_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(geom_surf_node, Instance)

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
    PID_SelectedFace, //!< ID of the selected face.
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

  void SetSelectedFace (const int faceId);
  int  GetSelectedFace ()                  const;

// Initialization:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  geom_surf_node();

};

#endif
