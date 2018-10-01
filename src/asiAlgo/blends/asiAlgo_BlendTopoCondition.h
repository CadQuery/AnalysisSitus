//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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

#ifndef asiAlgo_BlendTopoCondition_h
#define asiAlgo_BlendTopoCondition_h

// asiAlgo includes
#include <asiAlgo_BlendTopoConditionType.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Base class for a topological condition.
class asiAlgo_BlendTopoCondition : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BlendTopoCondition, Standard_Transient)

public:

  TopoDS_Face blend; //!< Blend face.
  TopoDS_Face s1;    //!< Support face 1.
  TopoDS_Face s2;    //!< Support face 2.
  TopoDS_Edge s1_s2; //!< s1-s2 intersection edge.

  //! Modified geometry.
  struct ModifGeom
  {
    Handle(Geom_Surface) geom_s1;
    Handle(Geom_Surface) geom_s2;
    Handle(Geom_Curve)   geom_s1_s2;
  };

  //! Default ctor.
  asiAlgo_BlendTopoCondition() : pModifGeom(NULL) {}

  //! Dtor.
  ~asiAlgo_BlendTopoCondition()
  {
    if ( pModifGeom )
      delete pModifGeom;
  }

  //! \return type of blend condition.
  virtual asiAlgo_BlendTopoConditionType GetType() const
  {
    return BlendTopoConditionType_Base;
  }

  //! Sets geometric modification info to the bend condition. The blend
  //! condition structure takes ownership of the info: the geometric info
  //! will be deleted by dtor of the blend info.
  //! \param[in] geometric modification info to set.
  void LoadGeomInfo(ModifGeom* info)
  {
    pModifGeom = info;
  }

public:

  ModifGeom* pModifGeom; //!< Geometric modification info.

};

#endif
