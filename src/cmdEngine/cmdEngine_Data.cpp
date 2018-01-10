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

// cmdEngine includes
#include <cmdEngine.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

//-----------------------------------------------------------------------------

void onUndoRedo(const Handle(ActAPI_HParameterMap)& affectedParams)
{
  if ( affectedParams.IsNull() )
    return;

  // Loop over the affected Parameters to get the affected Nodes. These Nodes
  // are placed into a map to have them unique.
  Handle(ActAPI_HNodeMap) affectedNodes = new ActAPI_HNodeMap;
  //
  for ( ActAPI_HParameterMap::Iterator pit(*affectedParams); pit.More(); pit.Next() )
  {
    // Get Node
    Handle(ActAPI_INode) N = pit.Value()->GetNode();
    //
    affectedNodes->Add(N);
  }

  // Get all presentation managers
  const vtkSmartPointer<asiVisu_PrsManager>& partPM   = cmdEngine::cf->ViewerPart->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& hostPM   = cmdEngine::cf->ViewerHost->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& domainPM = cmdEngine::cf->ViewerDomain->PrsMgr();

  // Loop over the unique Nodes to actualize them
  for ( ActAPI_HNodeMap::Iterator nit(*affectedNodes); nit.More(); nit.Next() )
  {
    const Handle(ActAPI_INode)& N = nit.Value();

    // Actualize
    if ( partPM->IsPresented(N) )
      partPM->Actualize(N);
    //
    if ( hostPM->IsPresented(N) )
      hostPM->Actualize(N);
    //
    if ( domainPM->IsPresented(N) )
      domainPM->Actualize(N);
  }

  // Update object browser
  cmdEngine::cf->ObjectBrowser->Populate();
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
  onUndoRedo( cmdEngine::model->Undo() );

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
  onUndoRedo( cmdEngine::model->Redo() );

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

  // Draw.
  interp->GetPlotter().REDRAW_SHAPE( argv[1], part_n->GetShape() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_SetAsPart(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiData_IVTopoItemNode)
    node = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_OK;
  }

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update( node->GetShape() );
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
  {
    cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderPresentation( node );
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );
  }

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
  asiAlgo_Utils::SplitStr(namePath, "/", names);

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

void cmdEngine::Commands_Data(const Handle(asiTcl_Interp)&      interp,
                              const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("undo",
    //
    "undo \n"
    "\t Undoes model changes.",
    //
    __FILE__, group, ENGINE_Undo);

  //-------------------------------------------------------------------------//
  interp->AddCommand("redo",
    //
    "redo \n"
    "\t Redoes model changes.",
    //
    __FILE__, group, ENGINE_Redo);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-as-var",
    //
    "set-as-var varName \n"
    "\t Copies part shape to a topological variable.",
    //
    __FILE__, group, ENGINE_SetAsVar);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-as-part",
    //
    "set-as-part varName\n"
    "\t Sets the object with the given name as a part for analysis. \n"
    "\t The object is expected to exist as a topological item in \n"
    "\t imperative plotter.",
    //
    __FILE__, group, ENGINE_SetAsPart);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-id",
    //
    "get-id [parentObjectName / [parentObjectName / [...]]] objectName\n"
    "\t Finds a data object with the given name and returns its persistent ID. \n"
    "\t If the object name is not unique, you may specify a list of parents \n"
    "\t to narrow your request. The names of the parent objects are separated \n"
    "\t by direct slash. You should always specify a direct parent of an object. \n"
    "\t It is not allowed to leave intermediate parents unspecified.",
    //
    __FILE__, group, ENGINE_GetId);
}
