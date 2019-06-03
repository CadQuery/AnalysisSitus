//-----------------------------------------------------------------------------
// Created on: 02 June 2019
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

#ifndef asiEngine_STEPReaderOutput_h
#define asiEngine_STEPReaderOutput_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiAlgo includes
#include <asiAlgo_ReadSTEPWithMetaOutput.h>

//-----------------------------------------------------------------------------

//! Input data adaptor for the STEP reader with metadata.
class asiEngine_STEPReaderOutput : public asiAlgo_ReadSTEPWithMetaOutput
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiEngine_STEPReaderOutput, asiAlgo_ReadSTEPWithMetaOutput)

public:

  //! Ctor accepting the Data Model instance.
  //! \param[in] M Data Model to export.
  asiEngine_EXPORT
    asiEngine_STEPReaderOutput(const Handle(asiEngine_Model)& M);

public:

  asiEngine_EXPORT virtual void
    SetShape(const TopoDS_Shape& shape);

  asiEngine_EXPORT virtual void
    SetColor(const TopoDS_Shape&   subshape,
             const Quantity_Color& color);

protected:

  //! Finds metadata element by shape.
  //! \param[in] shape shape in question.
  //! \return metadata element or null if no such element exists in the list.
  asiEngine_EXPORT Handle(asiData_ElemMetadataNode)
    elemByShape(const TopoDS_Shape& shape) const;

protected:

  //! Data Model instance.
  Handle(asiEngine_Model) m_model;

};

#endif
