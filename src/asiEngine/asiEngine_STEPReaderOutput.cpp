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

// Own include
#include <asiEngine_STEPReaderOutput.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_Utils.h>

//-----------------------------------------------------------------------------

asiEngine_STEPReaderOutput::asiEngine_STEPReaderOutput(const Handle(asiEngine_Model)& M)
: asiAlgo_ReadSTEPWithMetaOutput (),
  m_model                        (M)
{}

//-----------------------------------------------------------------------------

void asiEngine_STEPReaderOutput::SetShape(const TopoDS_Shape& shape)
{
  asiEngine_Part partApi(m_model);
  //
  partApi.Update( shape, nullptr, !m_model->GetPartNode()->IsKeepTessParams() );
}

//-----------------------------------------------------------------------------

void asiEngine_STEPReaderOutput::SetColor(const TopoDS_Shape&   subshape,
                                          const Quantity_Color& color)
{
  if ( subshape.ShapeType() < TopAbs_FACE )
    return; // Skip sub-shapes other than faces, edges, and vertices.

  Handle(asiData_ElemMetadataNode) elemNode = this->elemByShape(subshape);
  //
  if ( elemNode.IsNull() || !elemNode->IsWellFormed() )
    return;

  // Convert color to a persistent form.
  double colComponents[3] = { color.Red(), color.Green(), color.Blue() };
  const int iCol = asiVisu_Utils::ColorToInt(colComponents);

  // Store color.
  elemNode->SetColor(iCol);
}

//-----------------------------------------------------------------------------

Handle(asiData_ElemMetadataNode)
  asiEngine_STEPReaderOutput::elemByShape(const TopoDS_Shape& shape) const
{
  asiEngine_Part partApi(m_model);

  return partApi.FindElemMetadata(shape, true);
}
