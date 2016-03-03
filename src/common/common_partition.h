//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef common_partition_h
#define common_partition_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <ActData_BasePartition.h>

//! Partition for Data Nodes.
template<typename TNodeType>
class common_partition : public ActData_BasePartition
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(common_partition, ActData_BasePartition)

public:

  //! Instantiation routine.
  //! \return instance of Mesh Partition.
  static Handle(common_partition<TNodeType>) Instance()
  {
    return new common_partition();
  }

  //! Returns the Data Node type applicable for this Partition.
  //! \return applicable Node type.
  Handle(Standard_Type) GetNodeType() const
  {
    return STANDARD_TYPE(TNodeType);
  }

protected:

  common_partition() : ActData_BasePartition() {} //!< Default constructor.

};


#endif
