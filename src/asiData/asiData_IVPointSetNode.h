//-----------------------------------------------------------------------------
// Created on: 08 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_IVPointSetNode_h
#define asiData_IVPointSetNode_h

// asiData includes
#include <asiData.h>

// QrGeom3D includes
#include <QrGeom3D_PositionCloud.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Data Node representing a point cloud in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_IVPointSetNode, ActData_BaseNode)

//! Data Node representing a point cloud in IV (Imperative Viewer).
class asiData_IVPointSetNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVPointSetNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVPointSetNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Point cloud.
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

  asiData_EXPORT QrPtr<pcloud>
    GetPoints() const;

  asiData_EXPORT void
    SetPoints(const QrPtr<pcloud>& pointCloud);

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_IVPointSetNode();

};

#endif
