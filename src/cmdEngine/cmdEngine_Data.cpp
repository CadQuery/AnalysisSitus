//-----------------------------------------------------------------------------
// Created on: 24 August 2017
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

// cmdEngine includes
#include <cmdEngine.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_DialogOCAFDump.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

//-----------------------------------------------------------------------------

void onUndoRedo(const Handle(ActAPI_TxRes)& txRes,
                ActAPI_ProgressEntry        progress)
{
  if ( txRes.IsNull() )
    return;

  if ( !cmdEngine::cf )
    return;

  // Loop over the affected Parameters to get the affected Nodes. These Nodes
  // are placed into a map to have them unique.
  ActAPI_NodeIdMap modifiedNodes, deletedNodes;
  //
  for ( int k = 1; k <= txRes->parameterRefs.Extent(); ++k )
  {
    const ActAPI_TxRes::t_parameterRef& objRef = txRes->parameterRefs(k);

    // Get ID of the Node.
    ActAPI_NodeId nodeId = ActData_Common::NodeIdByParameterId(objRef.id);

    // Add Node ID.
    if ( objRef.isAlive )
    {
      if ( !modifiedNodes.Contains(nodeId) )
      {
        modifiedNodes.Add(nodeId);
        //
        progress.SendLogMessage(LogInfo(Normal) << "Modified Node: %1." << nodeId);
      }
    }
    else
    {
      if ( !deletedNodes.Contains(nodeId) )
      {
        deletedNodes.Add(nodeId);
        //
        progress.SendLogMessage(LogInfo(Normal) << "Deleted Node: %1." << nodeId);
      }
    }
  }

  // Get all presentation managers
  const vtkSmartPointer<asiVisu_PrsManager>& partPM   = cmdEngine::cf->ViewerPart->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& hostPM   = cmdEngine::cf->ViewerHost->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& domainPM = cmdEngine::cf->ViewerDomain->PrsMgr();

  // Loop over the deleted Nodes to derender them
  for ( int k = 1; k <= deletedNodes.Extent(); ++k )
  {
    const ActAPI_DataObjectId& id = deletedNodes(k);

    if ( partPM->IsPresented(id) )
      partPM->DeRenderPresentation(id);
    //
    if ( hostPM->IsPresented(id) )
      hostPM->DeRenderPresentation(id);
    //
    if ( domainPM->IsPresented(id) )
      domainPM->DeRenderPresentation(id);
  }

  // Loop over the modified Nodes to actualize them
  for ( int k = 1; k <= modifiedNodes.Extent(); ++k )
  {
    const ActAPI_DataObjectId& id = modifiedNodes(k);

    // Actualize
    if ( partPM->IsPresented(id) )
      partPM->Actualize( cmdEngine::model->FindNode(id) );
    //
    if ( hostPM->IsPresented(id) )
      hostPM->Actualize( cmdEngine::model->FindNode(id) );
    //
    if ( domainPM->IsPresented(id) )
      domainPM->Actualize( cmdEngine::model->FindNode(id) );
  }

  // Update object browser
  cmdEngine::cf->ObjectBrowser->Populate();
}

//-----------------------------------------------------------------------------

int ENGINE_SaveAs(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Print format for information.
  TCollection_AsciiString docFormat = cmdEngine::model->Document()->StorageFormat();
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Document format is %1."
                                                       << docFormat);

  // Save.
  if ( !cmdEngine::model->SaveAs( argv[1], interp->GetProgress() ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Saving failed.");
    return TCL_ERROR;
  }
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Model was saved to %1."
                                                       << argv[1]);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Load(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Release current project.
  cmdEngine::model->Release();

  // Open.
  if ( !cmdEngine::model->Open( argv[1], interp->GetProgress() ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Loading failed.");
    return TCL_ERROR;
  }
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Model was loaded from %1."
                                                       << argv[1]);

  // Find all presentable Nodes.
  Handle(ActAPI_HNodeList)
    nodes = asiEngine_Base(cmdEngine::model).FindPresentableNodes();

  // Update UI.
  cmdEngine::cf->ObjectBrowser->Populate();
  cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(nodes);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Undo(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Undo and process the affected Parameters
  onUndoRedo( cmdEngine::model->Undo(), interp->GetProgress() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Redo(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Redo and process the affected Parameters
  onUndoRedo( cmdEngine::model->Redo(), interp->GetProgress() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_SetAsVar(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Erase Part Node for convenience.
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderPresentation(part_n);

  // Draw.
  interp->GetPlotter().REDRAW_SHAPE( argv[1], part_n->GetShape() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_SetAsPart(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 2 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Extract shape to set as a part.
  TopoDS_Shape shapeToSet;
  //
  if ( argc == 2 )
  {
    Handle(asiData_IVTopoItemNode)
      node = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
    //
    if ( node.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
      return TCL_OK;
    }

    // Set shape to convert.
    shapeToSet = node->GetShape();

    // It is usually convenient to erase the source Node.
    if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
      cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderPresentation(node);
  }
  else
  {
    TCollection_AsciiString nodeIdStr, paramIdStr;
    //
    if ( !interp->GetKeyValue(argc, argv, "node", nodeIdStr) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected format of arguments.");
      return TCL_OK;
    }
    //
    if ( !interp->GetKeyValue(argc, argv, "param", paramIdStr) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected format of arguments.");
      return TCL_OK;
    }

    // Get Data Node.
    Handle(ActAPI_INode) node = cmdEngine::model->FindNode(nodeIdStr);
    //
    if ( node.IsNull() || !node->IsWellFormed() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Data object with ID %1 does not exist."
                                                          << nodeIdStr);
      return TCL_OK;
    }

    // Get Parameter.
    if ( !paramIdStr.IsIntegerValue() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Parameter ID %1 is not numerical."
                                                          << paramIdStr);
      return TCL_OK;
    }
    //
    const int                     PID   = paramIdStr.IntegerValue();
    Handle(ActAPI_IUserParameter) param = node->Parameter(PID);
    //
    if ( param.IsNull() || !param->IsWellFormed() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Parameter with ID %1 does not exist."
                                                          << PID);
      return TCL_OK;
    }

    // Convert to Shape Parameter.
    Handle(ActData_ShapeParameter)
      shapeParam = Handle(ActData_ShapeParameter)::DownCast(param);
    //
    if ( shapeParam.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Parameter with ID %1 is not of Shape type."
                                                          << PID);
      return TCL_OK;
    }

    // Set shape to convert.
    shapeToSet = shapeParam->GetShape();
  }

  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model).Update(shapeToSet);
  }
  cmdEngine::model->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetId(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get name of the target Node as a path
  std::string namePath(argv[1]);

  // Split by delimeter
  std::vector<std::string> names;
  //
  asiAlgo_Utils::Str::Split(namePath, "/", names);

  // Prepare a collection of object names for Active Data
  std::vector<TCollection_ExtendedString> adNames;
  //
  for ( size_t k = 0; k < names.size(); ++k )
  {
    TCollection_AsciiString adName( names[k].c_str() );
    adName.LeftAdjust();
    adName.RightAdjust();
    //
    adNames.push_back(adName);
  }

  // Find Node
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByNames(adNames);
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find object with name %1." << argv[1]);
    return TCL_OK;
  }

  // Get ID of the object
  ActAPI_DataObjectId id = node->GetId();
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Object ID: %1." << id);

  // Send to interpreter
  *interp << id;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Clear(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Clear viewers
  cmdEngine::ClearViewers();

  // Clear data
  cmdEngine::model->Clear();

  // Update object browser
  if ( cmdEngine::cf )
    cmdEngine::cf->ObjectBrowser->Populate();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_DumpProject(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  asiUI_DialogOCAFDump*
    pDumpProject = new asiUI_DialogOCAFDump( cmdEngine::model, interp->GetProgress() );
  //
  pDumpProject->show();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Data(const Handle(asiTcl_Interp)&      interp,
                              const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("save-as",
    //
    "save-as filename\n"
    "\t Saves model to file with the given name.",
    //
    __FILE__, group, ENGINE_SaveAs);

  //-------------------------------------------------------------------------//
  interp->AddCommand("load",
    //
    "load filename\n"
    "\t Loads model from file with the given name. The current project data\n"
    "\t will be lost.",
    //
    __FILE__, group, ENGINE_Load);

  //-------------------------------------------------------------------------//
  interp->AddCommand("undo",
    //
    "undo\n"
    "\t Undoes model changes.",
    //
    __FILE__, group, ENGINE_Undo);

  //-------------------------------------------------------------------------//
  interp->AddCommand("redo",
    //
    "redo\n"
    "\t Redoes model changes.",
    //
    __FILE__, group, ENGINE_Redo);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-as-var",
    //
    "set-as-var varName\n"
    "\t Copies part shape to a topological variable.",
    //
    __FILE__, group, ENGINE_SetAsVar);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-as-part",
    //
    "set-as-part [varName | -node id -param id]\n"
    "\t Sets the object with the given name as a part for analysis.\n"
    "\t The object is expected to exist as a topological item in\n"
    "\t imperative plotter.",
    //
    __FILE__, group, ENGINE_SetAsPart);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-id",
    //
    "get-id [parentObjectName / [parentObjectName / [...]]] objectName\n"
    "\t Finds a data object with the given name and returns its persistent ID.\n"
    "\t If the object name is not unique, you may specify a list of parents\n"
    "\t to narrow your request. The names of the parent objects are separated\n"
    "\t by direct slash. You should always specify a direct parent of an object.\n"
    "\t It is not allowed to leave intermediate parents unspecified.",
    //
    __FILE__, group, ENGINE_GetId);

  //-------------------------------------------------------------------------//
  interp->AddCommand("clear",
    //
    "clear\n"
    "\t Cleans up project data.",
    //
    __FILE__, group, ENGINE_Clear);

  //-------------------------------------------------------------------------//
  interp->AddCommand("dump-project",
    //
    "dump-project\n"
    "\t Dumps project contents as text.",
    //
    __FILE__, group, ENGINE_DumpProject);
}
