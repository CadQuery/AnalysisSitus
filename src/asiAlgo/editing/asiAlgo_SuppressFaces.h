//-----------------------------------------------------------------------------
// Created on: 02 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_SuppressFaces_h
#define asiAlgo_SuppressFaces_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopTools_ListOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to delete faces.
class asiAlgo_SuppressFaces
{
public:

  asiAlgo_EXPORT
    asiAlgo_SuppressFaces(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT virtual bool
    Perform(const TColStd_PackedMapOfInteger& faceIndices,
            const bool                        facesOnly);

  asiAlgo_EXPORT virtual bool
    Perform(const TopTools_ListOfShape& faces,
            const bool                  facesOnly);

public:

  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return instance of Re-Shape utility used for topological reduction.
  const Handle(BRepTools_ReShape)& GetReShape() const
  {
    return m_reShape;
  }

protected:

  TopoDS_Shape              m_master;  //!< Master model.
  TopoDS_Shape              m_result;  //!< Result.
  Handle(BRepTools_ReShape) m_reShape; //!< Re-Shape tool.

};

#endif
