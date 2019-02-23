//-----------------------------------------------------------------------------
// Created on: 10 December 2015
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

// Own include
#include <asiUI_PrimDataAdaptor.h>

// asiUI includes
#include <asiUI_Common.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//! \return handled value as QVariant. This method takes care of conversion
//!         of the persistent data type to the corresponding Qt type.
QVariant asiUI_PrimDataAdaptor::GetValue() const
{
  if ( m_node.IsNull() || !m_node->IsWellFormed() )
    return QVariant();

  QVariant result;
  Handle(ActAPI_IUserParameter) P = m_node->Parameter(m_iPID);

  // Resolve the actual data type
  if ( P->GetParamType() == Parameter_Real )
  {
    Handle(ActData_RealParameter) PReal = ActParamTool::AsReal(P);
    result = PReal->GetValue();
  }
  else if ( P->GetParamType() == Parameter_AsciiString )
  {
    Handle(ActData_AsciiStringParameter) PStr = ActParamTool::AsAsciiString(P);
    result = AsciiStr2QStr( PStr->GetValue() );
  }
  else if ( P->GetParamType() == Parameter_Int )
  {
    Handle(ActData_IntParameter) PInt = ActParamTool::AsInt(P);
    result = PInt->GetValue();
  }
  else if ( P->GetParamType() == Parameter_Bool )
  {
    Handle(ActData_BoolParameter) PBool = ActParamTool::AsBool(P);
    result = PBool->GetValue();
  }
  else
    result = QVariant();

  return result;
}

//! Sets value for to the Data Model.
//! \param ui_val [in] value to set in Qt format.
void asiUI_PrimDataAdaptor::SetValue(const QVariant& ui_val) const
{
  if ( m_node.IsNull() || !m_node->IsWellFormed() )
    return;

  Handle(ActAPI_IUserParameter) P = m_node->Parameter(m_iPID);

  // Resolve data type
  if ( P->GetParamType() == Parameter_Real ) // Proceed with Real
  {
    Handle(ActData_RealParameter) PReal = ActParamTool::AsReal(P);
    PReal->SetValue( ui_val.toDouble() );
  }
  else if ( P->GetParamType() == Parameter_Int ) // Proceed with Integer
  {
    Handle(ActData_IntParameter) PInt = ActParamTool::AsInt(P);
    PInt->SetValue( ui_val.toInt() );
  }
  else if ( P->GetParamType() == Parameter_AsciiString ) // Proceed with ASCII String
  {
    Handle(ActData_AsciiStringParameter) PStr = ActParamTool::AsAsciiString(P);
    PStr->SetValue( QStr2AsciiStr( ui_val.toString() ) );
  }
  else if ( P->GetParamType() == Parameter_Bool ) // Proceed with Boolean
  {
    Handle(ActData_BoolParameter) PBool = ActParamTool::AsBool(P);
    PBool->SetValue( ui_val.toBool() ? 1 : 0 );
  }
}
