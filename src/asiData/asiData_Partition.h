//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_Partition_h
#define asiData_Partition_h

// A-Situs includes
#include <asiData.h>

// Active Data includes
#include <ActData_BasePartition.h>

//! Partition for Data Nodes.
template<typename TNodeType>
class asiData_Partition : public ActData_BasePartition
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_Partition, ActData_BasePartition)

public:

  //! Instantiation routine.
  //! \return instance of Mesh Partition.
  static Handle(asiData_Partition<TNodeType>) Instance()
  {
    return new asiData_Partition();
  }

  //! Returns the Data Node type applicable for this Partition.
  //! \return applicable Node type.
  Handle(Standard_Type) GetNodeType() const
  {
    return STANDARD_TYPE(TNodeType);
  }

protected:

  asiData_Partition() : ActData_BasePartition() {} //!< Default constructor.

};


#endif
