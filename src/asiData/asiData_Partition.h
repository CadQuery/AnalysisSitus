//-----------------------------------------------------------------------------
// Created on: 26 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
