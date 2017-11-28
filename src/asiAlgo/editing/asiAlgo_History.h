//-----------------------------------------------------------------------------
// Created on: 27 November 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiAlgo_History_h
#define asiAlgo_History_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Shape.hxx>

// Standard includes
#include <vector>

//! \brief History graph for topological elements.
//!
//! This utility class serves tracking history of topological modifications.
class asiAlgo_History : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_History, Standard_Transient)

public:

  //! Node in the history graph.
  struct t_item
  {
    int                 Op;           //!< Operation index.
    TopoDS_Shape        TransientPtr; //!< Shape as a transient pointer.
    std::vector<t_item> Generated;    //!< List of items generated from this one.
    std::vector<t_item> Modified;     //!< List of items substituted this one.
    bool                IsDeleted;    //!< Identifies the item as deleted one.

    //! Ctor.
    t_item() : Op(0), IsDeleted(false) {}
  };

public:

  //! \brief Initializes history graph.
  asiAlgo_EXPORT
    asiAlgo_History();

protected:

  std::vector<t_item> m_roots; //!< Root items in the history.

};

#endif
