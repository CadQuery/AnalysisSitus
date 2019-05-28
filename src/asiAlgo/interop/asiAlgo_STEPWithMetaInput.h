//-----------------------------------------------------------------------------
// Created on: 28 May 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiAlgo_STEPWithMetaInput_h
#define asiAlgo_STEPWithMetaInput_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>

//-----------------------------------------------------------------------------

//! Input data provider for the STEP writer with metadata.
class asiAlgo_STEPWithMetaInput : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_STEPWithMetaInput, Standard_Transient)

public:

  //! \return shape to write.
  virtual TopoDS_Shape
    GetShape() const = 0;

  //! \return number of subshapes to be resolved by the STEP writer as
  //!         individual entities to attach metadata to.
  virtual int
    GetNumSubShapes() const = 0;

  //! Accessor for a subshape by its zero-based index.
  //! \param[in] zeroBasedIdx zero-based index of a sub-shape to access.
  //! \return transient pointer to a sub-shape.
  virtual TopoDS_Shape
    GetSubShape(const int zeroBasedIdx) const = 0;

  //! Checks whether the passed shape has color or not.
  //! \param[in] shape shape in question.
  //! \return false if no color is attached, true -- otherwise.
  virtual bool
    HasColor(const TopoDS_Shape& shape) const = 0;

  //! Accessor for the color associated with the given shape.
  //! \param[in] shape shape in question.
  //! \return attached color.
  virtual Quantity_Color
    GetColor(const TopoDS_Shape& shape) const = 0;

};

#endif
