//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef common_model_h
#define common_model_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (common) includes
#include <common_partition.h>
#include <common_root_node.h>
#include <common_version.h>

// A-Situs (geometry) includes
#include <geom_part_node.h>
#include <geom_sections_node.h>
#include <geom_section_node.h>

// A-Situs (mesh) includes
#include <mesh_node.h>

// Active Data includes
#include <ActData_BaseModel.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(common_model, ActData_BaseModel)

//! Data Model of A-Situs.
class common_model : public ActData_BaseModel
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(common_model, ActData_BaseModel)

public:

  common_model();

//-----------------------------------------------------------------------------
// Populate and clear:
public:

  void Populate();
  void Clear();

//-----------------------------------------------------------------------------
// Accessors to Nodes:
public:

  Handle(mesh_node)          MeshNode()     const;
  Handle(geom_part_node)     PartNode()     const;
  Handle(geom_sections_node) SectionsNode() const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  inline virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<common_model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to Mesh Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<mesh_node>) MeshPartition() const
  {
    return Handle(common_partition<mesh_node>)::DownCast( this->Partition(Partition_Mesh) );
  }

  //! Accessor for a Partition instance dedicated to root Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<common_root_node>) RootPartition() const
  {
    return Handle(common_partition<common_root_node>)::DownCast( this->Partition(Partition_Root) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Part Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_part_node>) PartPartition() const
  {
    return Handle(common_partition<geom_part_node>)::DownCast( this->Partition(Partition_GeomPart) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Face Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_face_node>) GeomFacePartition() const
  {
    return Handle(common_partition<geom_face_node>)::DownCast( this->Partition(Partition_GeomFace) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Surface Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_surf_node>) GeomSurfacePartition() const
  {
    return Handle(common_partition<geom_surf_node>)::DownCast( this->Partition(Partition_GeomSurface) );
  }

  //! Accessor for a Partition instance dedicated to Sections Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_sections_node>) SectionsPartition() const
  {
    return Handle(common_partition<geom_sections_node>)::DownCast( this->Partition(Partition_Sections) );
  }

  //! Accessor for a Partition instance dedicated to Section Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_section_node>) SectionPartition() const
  {
    return Handle(common_partition<geom_section_node>)::DownCast( this->Partition(Partition_Section) );
  }

private:

  virtual void
    initPartitions();

  virtual void
    initFunctionDrivers();

private:

  virtual Handle(ActAPI_IPartition)
    getVariablePartition(const VariableType& theVarType) const;

  virtual Handle(ActAPI_INode)
    getRootNode() const;

  virtual void
    invariantCopyRefs(ActAPI_FuncGUIDStream&         FuncGUIDs,
                      ActAPI_ParameterLocatorStream& Refs) const;

private:

  virtual int
    actualVersionApp();

  virtual Handle(ActData_CAFConverter)
    converterApp();

private:

  //! IDs of the registered Partitions.
  enum PartitionId
  {
    Partition_RealVar = 1, // Good practice to start with 1 when working with CAF
  //---------------------------------------------------------------------------
    Partition_Root,
  //---------------------------------------------------------------------------
    Partition_Mesh,
  //---------------------------------------------------------------------------
    Partition_GeomPart,
    Partition_GeomFace,
    Partition_GeomSurface,
  //---------------------------------------------------------------------------
    Partition_Sections,
    Partition_Section
  };

};

#endif
