//-----------------------------------------------------------------------------
// Created on: 22 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiData_ReCoedgeNode_h
#define asiData_ReCoedgeNode_h

// asiData includes
#include <asiData_ReEdgeNode.h>

// Active Data includes
#include <ActData_BaseNode.h>

//! Data Node representing an occurrence of a topological edge in a patch.
//! Technically, this is nothing but a pointer to the edge plus orientation
//! flag. CoEdges allow for reusing geometry of edges in adjacent patches.
class asiData_ReCoedgeNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_ReCoedgeNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_ReCoedgeNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //---------------//
    PID_Name,      //!< Name of the Node.
    PID_EdgeRef,   //!< Reference to the edge (i.e. real geometry).
    PID_SameSense, //!< Orientation flag (to agree or not with the orientation of edge).
  //---------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of the Node ensuring its correct
  //! allocation in a heap.
  //! \return new instance of the Node.
  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  //! Performs initial actions required to make Node WELL-FORMED.
  //! \param[in] edge      edge to reference.
  //! \param[in] samesense orientation flag.
  asiData_EXPORT void
    Init(const Handle(asiData_ReEdgeNode)& edge,
         const bool                        samesense);

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param[in] name name to set.
  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

public:

  //! \return referenced edge.
  asiData_EXPORT Handle(asiData_ReEdgeNode)
    GetEdge() const;

  //! Sets the reference to the edge.
  //! \param[in] edge reference to set.
  asiData_EXPORT void
    SetEdge(const Handle(asiData_ReEdgeNode)& edge);

  //! \return first vertex of the edge taking into account the orientation
  //!         flag associated with the coedge.
  asiData_EXPORT Handle(asiData_ReVertexNode)
    GetFirstVertex() const;

  //! \return last vertex of the edge taking into account the orientation
  //!         flag associated with the coedge.
  asiData_EXPORT Handle(asiData_ReVertexNode)
    GetLastVertex() const;

  //! \return true if the orientation of the coedge is the same as the
  //!         natural orientation of the referenced edge.
  asiData_EXPORT bool
    IsSameSense() const;

  //! Sets orientation flag.
  //! \param[in] isSameSense flag to set.
  asiData_EXPORT void
    SetSameSense(const bool isSameSense);

  //! \return opposite coedge (if any).
  asiData_EXPORT Handle(asiData_ReCoedgeNode)
    GetOpposite() const;

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_ReCoedgeNode();

};

#endif
