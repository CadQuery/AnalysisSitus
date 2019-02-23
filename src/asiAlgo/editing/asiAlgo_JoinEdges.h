//-----------------------------------------------------------------------------
// Created on: 30 March 2016
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

#ifndef asiAlgo_JoinEdges_h
#define asiAlgo_JoinEdges_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to join edge chains.
class asiAlgo_JoinEdges
{
public:

  asiAlgo_EXPORT
    asiAlgo_JoinEdges(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& edges,
            const TopoDS_Face&                face);

public:

  //! \return result shape.
  const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  void chooseOrder(const TopTools_IndexedMapOfShape& edges,
                   TopoDS_Edge&                      eFirst,
                   TopoDS_Edge&                      eSecond) const;

  bool joinEdges(const TopoDS_Edge& eFirst,
                 const TopoDS_Edge& eSecond,
                 const TopoDS_Face& face,
                 TopoDS_Edge&       eResult) const;

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
