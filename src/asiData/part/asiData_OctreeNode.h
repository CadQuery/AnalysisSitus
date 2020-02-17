//-----------------------------------------------------------------------------
// Created on: 23 January 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiData_OctreeNode_h
#define asiData_OctreeNode_h

// asiData includes
#include <asiData_OctreeParameter.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

//! Node representing octree spatial decomposition.
class asiData_OctreeNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_OctreeNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_OctreeNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //----------------------//
    PID_Name,             //!< Name of the Node.
    PID_Octree,           //!< Octree decomposition of the space occupied by the part.
    PID_SamplingStrategy, //!< Sampling strategy.
    PID_ExtractPoints,    //!< Whether to extract points instead of voxels.
    PID_NumElements,      //!< Number of extracted elements (cells or points).
  //----------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of the Octree Node ensuring its correct
  //! allocation in a heap.
  //! \return new instance of the Octree Node.
  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  //! Performs initial actions required to make the Octree Node WELL-FORMED.
  asiData_EXPORT void
    Init();

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param[in] name name to set.
  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString&);

// Handy accessors to the stored data:
public:

  //! \return void pointer to octree data structure.
  asiData_EXPORT void*
    GetOctree() const;

  //! Sets octree to store.
  //! \param[in] pOctree octree to store.
  asiData_EXPORT void
    SetOctree(void* pOctree);

  //! \return sampling strategy as integer.
  asiData_EXPORT int
    GetSamplingStrategy() const;

  //! Sets sampling strategy.
  //! \param[in] mode value to set.
  asiData_EXPORT void
    SetSamplingStrategy(const int mode);

  //! \return true if the points extraction mode is enabled, false -- otherwise.
  asiData_EXPORT bool
    GetExtractPoints() const;

  //! Sets points extraction mode.
  //! \param[in] toExtract true/false.
  asiData_EXPORT void
    SetExtractPoints(const bool toExtract);

  //! \return number of elements (points or cells).
  asiData_EXPORT int
    GetNumElements() const;

  //! Sets the number of elements.
  //! \param[in] numElems the number of elements to store.
  asiData_EXPORT void
    SetNumElements(const int numElems);

protected:

  //! Default ctor. Registers all involved Parameters.
  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_OctreeNode();

};

#endif
