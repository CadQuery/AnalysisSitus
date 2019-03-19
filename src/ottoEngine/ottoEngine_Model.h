//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoEngine_Model_h
#define ottoEngine_Model_h

// ottoEngine includes
#include <ottoEngine.h>

// ottoData includes
#include <ottoData_LimbNode.h>

// asiEngine includes
#include <asiEngine_Model.h>

//-----------------------------------------------------------------------------

//! Data Model for Ottobock Workbench.
class ottoEngine_Model : public asiEngine_Model
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoEngine_Model, asiEngine_Model)

public:

  ottoEngine_EXPORT
    ottoEngine_Model();

public:

  ottoEngine_EXPORT virtual void
    Populate();

public:

  ottoEngine_EXPORT virtual void
    clearCustom();

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<ottoEngine_Model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Partition of limbs.
  //! \return requested Partition.
  Handle(asiData_Partition<ottoData_LimbNode>) GetLimbPartition() const
  {
    return Handle(asiData_Partition<ottoData_LimbNode>)::DownCast( this->Partition(Partition_Limb) );
  }

//-----------------------------------------------------------------------------
// Nodes:
public:

  //! \return Limb Node.
  Handle(ottoData_LimbNode) GetLimbNode() const
  {
    return Handle(ottoData_LimbNode)::DownCast( this->GetLimbPartition()->GetNode(1) );
  }

private:

  virtual void
    initPartitions();

  virtual void
    initFunctionDrivers();

private:

  //! IDs of partities.
  enum PartitionId
  {
    Partition_Limb = asiEngine_Model::Partition_LAST
  };

};

#endif
