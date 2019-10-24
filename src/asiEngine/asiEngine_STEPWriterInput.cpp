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

// Own include
#include <asiEngine_STEPWriterInput.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_Utils.h>

//-----------------------------------------------------------------------------

asiEngine_STEPWriterInput::asiEngine_STEPWriterInput(const Handle(asiEngine_Model)& M)
: asiAlgo_WriteSTEPWithMetaInput (),
  m_model                        (M)
{
  asiEngine_Part api(m_model);
  api.GetMetadataElems(m_metaElems);
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiEngine_STEPWriterInput::GetShape() const
{
  return m_model->GetPartNode()->GetShape(false);
}

//-----------------------------------------------------------------------------

int asiEngine_STEPWriterInput::GetNumSubShapes() const
{
  if ( m_metaElems.IsNull() )
    return 0;

  return m_metaElems->Length();
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiEngine_STEPWriterInput::GetSubShape(const int zeroBasedIdx) const
{
  const Handle(asiData_ElemMetadataNode)&
    elemNode = Handle(asiData_ElemMetadataNode)::DownCast( m_metaElems->Value(zeroBasedIdx + 1) );
  //
  return elemNode->GetShape();
}

//-----------------------------------------------------------------------------

bool asiEngine_STEPWriterInput::HasColor(const TopoDS_Shape& shape) const
{
  Handle(asiData_ElemMetadataNode) metaNode = this->elemByShape(shape);
  //
  if ( metaNode.IsNull() )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

Quantity_Color
  asiEngine_STEPWriterInput::GetColor(const TopoDS_Shape& shape) const
{
  Handle(asiData_ElemMetadataNode) metaNode = this->elemByShape(shape);
  //
  if ( metaNode.IsNull() )
    return Quantity_Color(1., 1., 1., Quantity_TOC_RGB);

  QColor qCol = asiVisu_Utils::IntToColor( metaNode->GetColor() );

  return Quantity_Color(qCol.redF(), qCol.greenF(), qCol.blueF(), Quantity_TOC_RGB);
}

//-----------------------------------------------------------------------------

Handle(asiData_ElemMetadataNode)
  asiEngine_STEPWriterInput::elemByShape(const TopoDS_Shape& shape) const
{
  if ( m_metaElems.IsNull() )
    return NULL;

  for ( ActAPI_HNodeList::Iterator nit(*m_metaElems); nit.More(); nit.Next() )
  {
    const Handle(asiData_ElemMetadataNode)&
      metaNode = Handle(asiData_ElemMetadataNode)::DownCast( nit.Value() );

    if ( metaNode->GetShape().IsSame(shape) )
      return metaNode;
  }

  return NULL;
}
