//-----------------------------------------------------------------------------
// Created on: 10 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_prim_data_adaptor.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//! \return handled value as QVariant. This method takes care of conversion
//!         of the persistent data type to the corresponding Qt type.
QVariant gui_prim_data_adaptor::GetValue() const
{
  if ( m_node.IsNull() || !m_node->IsWellFormed() )
    return QVariant();

  QVariant result;
  Handle(ActAPI_IParameter) P = m_node->Parameter(m_iPID);

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
    result = (PBool->GetValue() > 0);
  }
  else
    result = QVariant();

  return result;
}

//! Sets value for to the Data Model.
//! \param ui_val [in] value to set in Qt format.
void gui_prim_data_adaptor::SetValue(const QVariant& ui_val) const
{
  if ( m_node.IsNull() || !m_node->IsWellFormed() )
    return;

  Handle(ActAPI_IParameter) P = m_node->Parameter(m_iPID);

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
