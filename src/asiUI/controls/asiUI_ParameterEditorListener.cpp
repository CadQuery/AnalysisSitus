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
: QObject        (),
  m_cf           (cf),
  m_obUpdateType (OB_UpdateType_Undefined)
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

    // Execute Tree Functions.
    const int execStatus = m_cf->Model->FuncExecuteAll();
    //
    if ( execStatus & ActAPI_IModel::Execution_Done )
      m_cf->Progress.SendLogMessage(LogNotice(Normal) << "Tree functions were successfully executed.");
    if ( execStatus & ActAPI_IModel::Execution_NoFunctions )
      m_cf->Progress.SendLogMessage(LogNotice(Normal) << "There are no tree functions in the model.");
    if ( execStatus & ActAPI_IModel::Execution_LoopsDetected )
      m_cf->Progress.SendLogMessage(LogErr(Normal) << "Tree functions cannot be executed because of dependency loop.");
    if ( execStatus & ActAPI_IModel::Execution_Failed )
      m_cf->Progress.SendLogMessage(LogErr(Normal) << "Tree function execution failed.");
  }
  m_cf->Model->CommitCommand();

  // Set default update type.
  if ( m_obUpdateType == OB_UpdateType_Undefined )
    m_obUpdateType = OB_UpdateType_SelectedNode;

  // Update Object Browser.
  if ( m_cf->ObjectBrowser )
  {
    if ( m_obUpdateType == OB_UpdateType_All )
      m_cf->ObjectBrowser->Populate();
    else if ( m_obUpdateType == OB_UpdateType_SelectedNode )
      m_cf->ObjectBrowser->UpdateSelectedNode();
  }

  // Reset update type (this gives the sub-classes the chance to redefine
  // the update type in callbacks).
  m_obUpdateType = OB_UpdateType_Undefined;

  // Actualize affected Nodes.
  Handle(ActAPI_HNodeMap)   affectedNodes   = new ActAPI_HNodeMap;
  Handle(ActAPI_HNodeIdMap) affectedNodeIds = m_cf->Model->GetModifiedNodes();
  TCollection_AsciiString   affectedNodeNames;
  //
  for ( ActAPI_HNodeIdMap::Iterator nit(*affectedNodeIds); nit.More(); nit.Next() )
  {
    const ActAPI_DataObjectId& nodeId = nit.Value();
    Handle(ActAPI_INode)       node   = m_cf->Model->FindNode(nodeId);

    affectedNodeNames += " ";
    affectedNodeNames += (nodeId + " [" + node->DynamicType()->Name() + "]" );

    // Add to the collection of Nodes for subsequent actualization.
    affectedNodes->Add(node);
  }

  // Actualize presentations.
  m_cf->ActualizeNodes(affectedNodes);

  m_cf->Progress.SendLogMessage( LogInfo(Normal) << "Actualizing Nodes: %1."
                                                 << affectedNodeNames );
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

  // Callback for direct pre-treatment.
  bool proceed = true;
  this->beforeParameterChanged(N, pid, value, proceed);

  /* =======================================
   *  Process different types of Parameters
   * ======================================= */

  if ( proceed ) // Callback above may decide that the default treatment should
                 // not be done (e.g., when a modification of a Parameter in
                 // the selected Node should actually result in a modification
                 // of a completely different Node).
  {
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
  }

  // Callback for direct post-treatment.
  this->afterParameterChanged(N, pid);

  // Notify listeners.
  emit parameterStored();
  return true;
}
