//-----------------------------------------------------------------------------
// Created on: 25 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_REGaussMapNode_h
#define asiData_REGaussMapNode_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Gauss map for Reverse Engineering
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_REGaussMapNode, ActData_BaseNode)

//! Node representing Gauss map.
class asiData_REGaussMapNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_REGaussMapNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_REGaussMapNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Vectors,      //!< Vectors (array of coordinates).
    PID_AxisX,        //!< X coordinate of a central axis.
    PID_AxisY,        //!< Y coordinate of a central axis.
    PID_AxisZ,        //!< Z coordinate of a central axis.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  asiData_EXPORT void
    Init();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  asiData_EXPORT gp_Vec
    GetVector(const int zeroBasedIndex) const;

  asiData_EXPORT Handle(asiAlgo_BaseCloud<double>)
    GetVectors() const;

  asiData_EXPORT Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf() const;

  asiData_EXPORT void
    SetVectors(const Handle(asiAlgo_BaseCloud<double>)& vectors);

  asiData_EXPORT void
    SetVectorsf(const Handle(asiAlgo_BaseCloud<float>)& vectors);

  asiData_EXPORT bool
    IsEmpty() const;

  asiData_EXPORT void
    SetCentralAxis(const double x, const double y, const double z);

  asiData_EXPORT void
    GetCentralAxis(double& x, double& y, double& z) const;

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_REGaussMapNode();

};

#endif
