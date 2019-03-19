//-----------------------------------------------------------------------------
// Created on: 21 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoData_LimbNode_h
#define ottoData_LimbNode_h

// ottoData includes
#include <ottoData.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Limb Node
//-----------------------------------------------------------------------------

//! Node representing a limb.
class ottoData_LimbNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoData_LimbNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(ottoData_LimbNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-----------------------//
  // Common                //
  //-----------------------//
    PID_Name,              //!< Name of the Node.
  //-----------------------//
    PID_FilenameMesh,      //!< Mesh filename.
    PID_Mesh,              //!< Mesh representing the limb shape.
    PID_OBB,               //!< Oriented bounding box as a B-Rep shape.
    PID_OBBProps,          //!< Array of OBB properties.
    PID_Sections,          //!< Compound of sections.
    PID_NumSections,       //!< Number of sections.
    PID_Surface,           //!< Skinned surface.
  //-----------------------//
    PID_LoadMeshFunc,      //!< Tree Function to load mesh.
    PID_BuildOBBFunc,      //!< Tree Function to build OBB.
    PID_BuildSectionsFunc, //!< Tree Function to build sections.
    PID_SkinSurfaceFunc,   //!< Tree Function for surface skinning.
  //-----------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of Turbine Node ensuring its correct
  //! allocation in heap.
  //! \return new instance of Turbine Node.
  ottoData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  //! Performs initial actions required to make Turbine Node WELL-FORMED.
  ottoData_EXPORT void
    Init();

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  ottoData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param name [in] name to set.
  ottoData_EXPORT virtual void
    SetName(const TCollection_ExtendedString&);

protected:

  //! Default ctor. Registers all involved Parameters.
  ottoData_EXPORT
    ottoData_LimbNode();

};

#endif
