//-----------------------------------------------------------------------------
// Created on: 25 February 2019
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
#include <asiUI_ParameterEditorListener.h>

// asiUI includes
#include <asiUI_ParamEditorImpl.h>

//-----------------------------------------------------------------------------

asiUI_ParameterEditorListener::asiUI_ParameterEditorListener(const Handle(asiUI_CommonFacilities)& cf)
: QObject (),
  m_cf    (cf)
{}

//-----------------------------------------------------------------------------

asiUI_ParameterEditorListener::~asiUI_ParameterEditorListener()
{}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditorListener::Connect()
{
  Handle(asiUI_ParamEditorImpl)
    peApi = Handle(asiUI_ParamEditorImpl)::DownCast(m_cf->ParamEditor);
  //
  if ( peApi.IsNull() || !peApi->GetWidget() )
    return;

  // Get widget.
  asiUI_ParameterEditor* peWidget = peApi->GetWidget();

  // Connect the default reaction.
  connect( peWidget, SIGNAL( ParamModified(const int, const QVariant&) ),
           this,     SLOT( onParameterChanged(const int, const QVariant&) ) );
}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditorListener::onParameterChanged(const int       pid,
                                                       const QVariant& value)
{
  if ( m_cf->Model.IsNull() )
  {
    m_cf->Progress.SendLogMessage(LogErr(Normal) << "Data Model is null.");
    return;
  }

  // Perform transactional modification.
  m_cf->Model->OpenCommand();
  {
    if ( !this->applyParameter(pid, value) )
    {
      m_cf->Model->AbortCommand();
      return;
    }
  }
  m_cf->Model->CommitCommand();
}

//-----------------------------------------------------------------------------

bool asiUI_ParameterEditorListener::applyParameter(const int       pid,
                                                   const QVariant& value)
{
  /* =================
   *  Contract checks
   * ================= */

  if ( m_cf->Model.IsNull() )
  {
    m_cf->Progress.SendLogMessage(LogErr(Normal) << "Data Model is null.");
    return false;
  }

  if ( !m_cf->Model->HasOpenCommand() )
  {
    m_cf->Progress.SendLogMessage(LogErr(Normal) << "Cannot modify parameter: no current transaction.");
    return false;
  }

  // Get selected Node.
  Handle(ActAPI_INode) N = m_cf->ObjectBrowser->GetSelectedNode();
  //
  if ( N.IsNull() || !N->IsWellFormed() )
  {
    m_cf->Progress.SendLogMessage(LogErr(Normal) << "Node in question is null or bad-formed.");
    return false;
  }

  // Get parameter in question.
  Handle(ActAPI_IUserParameter) P = N->Parameter(pid);
  //
  if ( P.IsNull() )
    return false;

  /* =======================================
   *  Process different types of Parameters
   * ======================================= */

  // We do not check that the passed Parameter value is different from
  // the existing one in case if Node is invalid. In such cases, the user may
  // want to repeat the same input in order to fix invalidity. In other
  // cases, we perform equality check in order to skip redundant changes.
  const bool doForceUpdate = !N->IsValidData();

  ActAPI_ParameterType paramType = (ActAPI_ParameterType) P->GetParamType();

  /* Unicode string */

  if ( paramType == Parameter_Name ) // UNICODE STRING Parameter.
  {
    Handle(ActData_NameParameter)
      stringParam = Handle(ActData_NameParameter)::DownCast(P);

    TCollection_ExtendedString        str     = QStr2ExtStr( value.toString() );
    const TCollection_ExtendedString& prevStr = stringParam->GetValue();

    // No need to do anything in case if nothing has been really changed.
    if ( !doForceUpdate && str.IsEqual(prevStr) )
      return false;

    stringParam->SetValue(str);
  }

  /* ASCII string */

  else if ( paramType == Parameter_AsciiString ) // ASCII STRING Parameter.
  {
    Handle(ActData_AsciiStringParameter)
      stringParam = Handle(ActData_AsciiStringParameter)::DownCast(P);

    TCollection_AsciiString        str     = QStr2AsciiStr( value.toString() );
    const TCollection_AsciiString& prevStr = stringParam->GetValue();

    // No need to do anything in case if nothing has been really changed.
    if ( !doForceUpdate && str.IsEqual(prevStr) )
      return false;

    stringParam->SetValue(str);
  }

  /* Floating-point value */

  else if ( paramType == Parameter_Real ) // REAL Parameter.
  {
    Handle(ActData_RealParameter)
      realParam = Handle(ActData_RealParameter)::DownCast(P);

    const double dblValue  = value.toDouble();
    const double prevValue = realParam->GetValue();

    // No need to do anything in case if nothing has been really changed.
    if ( !doForceUpdate && (dblValue == prevValue) )
      return false;

    realParam->SetValue(dblValue);
  }

  /* Boolean */

  else if ( paramType == Parameter_Bool ) // Boolean Parameter.
  {
    Handle(ActData_BoolParameter)
      boolParam = Handle(ActData_BoolParameter)::DownCast(P);

    const bool bValue    = value.toBool();
    const bool prevValue = boolParam->GetValue();

    // No need to do anything in case if nothing has been really changed.
    if ( !doForceUpdate && (bValue == prevValue) )
      return false;

    boolParam->SetValue(bValue);
  }

  /* Integer */

  else if ( paramType == Parameter_Int ) // Integer Parameter.
  {
    Handle(ActData_IntParameter)
      intParam = Handle(ActData_IntParameter)::DownCast(P);

    const int iValue    = value.toInt();
    const int prevValue = intParam->GetValue();

    // No need to do anything in case if nothing has been really changed.
    if ( !doForceUpdate && (iValue == prevValue) )
      return false;

    intParam->SetValue(iValue);
  }

  /* Time stamp */

  else if ( paramType == Parameter_TimeStamp )
  {
    Handle(ActData_TimeStampParameter)
      timeParam = Handle(ActData_TimeStampParameter)::DownCast(P);

    Handle(ActAux_TimeStamp)
      tsVal = asiUI_Common::ToTimeStamp( value.toDateTime() );
    //
    Handle(ActAux_TimeStamp) tsValPrev = timeParam->GetValue();

    if ( !doForceUpdate && !tsValPrev.IsNull() && tsVal->IsEqual(tsValPrev) )
      return false;

    timeParam->SetValue(tsVal);
  }

  /* Reference */

  else if ( paramType == Parameter_Reference )
  {
    ActAPI_DataObjectId refNodeId = QStr2AsciiStr( value.toString() );

    Handle(ActAPI_IDataCursor) refNode = m_cf->Model->FindNode(refNodeId);

    if ( refNode.IsNull() )
      N->DisconnectReference(pid);
    else
      N->ConnectReference(pid, refNode);
  }

  /* Unknown */

  else
    return false;

  m_cf->Progress.SendLogMessage( LogInfo(Normal) << "Parameter (pid '%1') changed (new value is '%2')."
                                                 << pid << QStr2AsciiStr( value.toString() ) );

  // Notify listeners.
  emit parameterStored();
  return true;
}
