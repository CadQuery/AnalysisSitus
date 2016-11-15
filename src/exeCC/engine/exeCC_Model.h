//-----------------------------------------------------------------------------
// Created on: 15 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_Model_h
#define exeCC_Model_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiData includes
#include <asiData_RENode.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(exeCC_Model, asiEngine_Model)

//! Data Model for contour capture.
class exeCC_Model : public asiEngine_Model
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exeCC_Model, asiEngine_Model)

public:

  exeCC_Model();

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

  Handle(asiData_ContourNode)
    GetContourNode() const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<exeCC_Model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to Contour Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ContourNode>) GetContourPartition() const
  {
    return Handle(asiData_Partition<asiData_ContourNode>)::DownCast( this->Partition(Partition_Contour) );
  }

private:

  virtual void
    initPartitions();

private:

  //! IDs of the registered Partitions.
  enum PartitionId
  {
    Partition_Contour = Partition_LAST
  };

};

#endif
