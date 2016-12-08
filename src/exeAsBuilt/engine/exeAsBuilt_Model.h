//-----------------------------------------------------------------------------
// Created on: 13 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_Model_h
#define exeAsBuilt_Model_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiData includes
#include <asiData_RENode.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(exeAsBuilt_Model, asiEngine_Model)

//! Data Model for as-built reconstruction.
class exeAsBuilt_Model : public asiEngine_Model
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exeAsBuilt_Model, asiEngine_Model)

public:

  exeAsBuilt_Model();

//-----------------------------------------------------------------------------
// Populate and clear:
public:

  virtual void
    Populate();

  virtual void
    Clear();

//-----------------------------------------------------------------------------
// Accessors to Nodes:
public:

  Handle(asiData_RENode)
    GetRENode() const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<exeAsBuilt_Model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to RE Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RENode>) GetREPartition() const
  {
    return Handle(asiData_Partition<asiData_RENode>)::DownCast( this->Partition(Partition_RE) );
  }

  //! Accessor for a Partition instance dedicated to RE Contour Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_REContourNode>) GetREContourPartition() const
  {
    return Handle(asiData_Partition<asiData_REContourNode>)::DownCast( this->Partition(Partition_REContour) );
  }

  //! Accessor for a Partition instance dedicated to RE Contours Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_REContoursNode>) GetREContoursPartition() const
  {
    return Handle(asiData_Partition<asiData_REContoursNode>)::DownCast( this->Partition(Partition_REContours) );
  }

  //! Accessor for a Partition instance dedicated to RE Points Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_REPointsNode>) GetREPointsPartition() const
  {
    return Handle(asiData_Partition<asiData_REPointsNode>)::DownCast( this->Partition(Partition_REPoints) );
  }

  //! Accessor for a Partition instance dedicated to RE Normals Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RENormalsNode>) GetRENormalsPartition() const
  {
    return Handle(asiData_Partition<asiData_RENormalsNode>)::DownCast( this->Partition(Partition_RENormals) );
  }

  //! Accessor for a Partition instance dedicated to RE Surface Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RESurfaceNode>) GetRESurfacePartition() const
  {
    return Handle(asiData_Partition<asiData_RESurfaceNode>)::DownCast( this->Partition(Partition_RESurface) );
  }

  //! Accessor for a Partition instance dedicated to RE Surfaces Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RESurfacesNode>) GetRESurfacesPartition() const
  {
    return Handle(asiData_Partition<asiData_RESurfacesNode>)::DownCast( this->Partition(Partition_RESurfaces) );
  }

private:

  virtual void
    initPartitions();

private:

  //! IDs of the registered Partitions.
  enum PartitionId
  {
    Partition_RE = Partition_LAST,
    Partition_REContour,
    Partition_REContours,
    Partition_REPoints,
    Partition_RENormals,
    Partition_RESurface,
    Partition_RESurfaces
  };

};

#endif
