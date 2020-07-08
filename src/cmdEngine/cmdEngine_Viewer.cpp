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

// asiUI includes
#include <asiUI_HistoryGraph.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

//-----------------------------------------------------------------------------

int ENGINE_Erase(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
  bool erasePart = (argc == 1);

  for ( int k = 1; k < argc; ++k )
  {
    // Find node
    Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[k]);
    //
    if ( node.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[k]);
      continue;
    }

    // Hide
    if ( cmdEngine::cf->ViewerPart->PrsMgr()->IsPresented(node) )
    {
      cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderPresentation(node);
    }
    else
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "There is no presentable object with name %1." << argv[k]);
  }

  if ( erasePart )
    cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderPresentation( cmdEngine::model->GetPartNode() );

  // Repaint
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ShowHistory(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and its history (from naming)
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();
  Handle(asiAlgo_Naming)   naming = part_n->GetNaming();
  //
  if ( naming.IsNull() || naming->GetHistory().IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "History is not initialized.");
    return TCL_OK;
  }

  // Visualize history in graph view
  asiUI_HistoryGraph*
    pGraph = new asiUI_HistoryGraph( cmdEngine::model,
                                     naming,
                                     interp->GetProgress(),
                                     interp->GetPlotter() );
  //
  pGraph->Render();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Show(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( !cmdEngine::cf || !cmdEngine::cf->ViewerPart )
    return TCL_OK;

  if ( argc > 1 )
  {
    for ( int k = 1; k < argc; ++k )
    {
      // Find Node
      Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[k]);
      //
      if ( node.IsNull() )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find object with name %1." << argv[k]);
        continue;
      }

      // Display
      if ( cmdEngine::cf->ViewerPart->PrsMgr()->IsPresented(node) )
      {
        cmdEngine::cf->ViewerPart->PrsMgr()->RenderPresentation(node);
      }
      else
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "There is no presentable object with name %1." << argv[k]);
    }
  }
  else
    cmdEngine::cf->ViewerPart->PrsMgr()->RenderPresentation( cmdEngine::model->GetPartNode() );

  // Repaint
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ShowOnly(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( !cmdEngine::cf || !cmdEngine::cf->ViewerPart )
    return TCL_OK;

  cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderAllPresentations();
  //
  if ( argc > 1 )
  {
    for ( int k = 1; k < argc; ++k )
    {
      // Find Node
      Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[k]);
      //
      if ( node.IsNull() )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find object with name %1." << argv[k]);
        continue;
      }

      // Display only
      if ( cmdEngine::cf->ViewerPart->PrsMgr()->IsPresented(node) )
        cmdEngine::cf->ViewerPart->PrsMgr()->RenderPresentation(node);
      else
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "There is no presentable object with name %1." << argv[k]);
    }
  }
  else
    cmdEngine::cf->ViewerPart->PrsMgr()->RenderPresentation( cmdEngine::model->GetPartNode() );

  // Repaint
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EraseAll(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderAllPresentations();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Fit(const Handle(asiTcl_Interp)& interp,
               int                          argc,
               const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  if ( cmdEngine::cf.IsNull() )
    return TCL_OK;

  asiVisu_Utils::AdjustCamera( cmdEngine::cf->ViewerPart->PrsMgr()->GetRenderer(),
                               cmdEngine::cf->ViewerPart->PrsMgr()->PropsByTrihedron() );
  //
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_SelectFaces(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc < 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  if ( cmdEngine::cf.IsNull() )
    return TCL_OK;

  asiAlgo_Feature feature;
  //
  for ( int k = 1; k < argc; ++k )
    feature.Add( atoi(argv[k]) );

  asiEngine_Part partApi( cmdEngine::model, cmdEngine::cf->ViewerPart->PrsMgr() );
  partApi.HighlightFaces(feature);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Viewer(const Handle(asiTcl_Interp)&      interp,
                                const Handle(Standard_Transient)& cmdEngine_NotUsed(data))
{
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("erase",
    //
    "erase [<varName1> [<varName2> ...]]\n"
    "\t Hides object in viewer.",
    //
    __FILE__, group, ENGINE_Erase);

  //-------------------------------------------------------------------------//
  interp->AddCommand("hide",
    //
    "hide <varName1> [<varName2> ...]\n"
    "\t Hides object in viewer.",
    //
    __FILE__, group, ENGINE_Erase);

  //-------------------------------------------------------------------------//
  interp->AddCommand("show-history",
    //
    "show-history\n"
    "\t Shows modification history associated with the active part.",
    //
    __FILE__, group, ENGINE_ShowHistory);

  //-------------------------------------------------------------------------//
  interp->AddCommand("show",
    //
    "show <varName1> [<varName2> ...]\n"
    "\t Shows the given object in viewer.",
    //
    __FILE__, group, ENGINE_Show);

  //-------------------------------------------------------------------------//
  interp->AddCommand("display",
    //
    "display <varName1> [<varName2> ...]\n"
    "\t Shows the given object in viewer.",
    //
    __FILE__, group, ENGINE_Show);

  //-------------------------------------------------------------------------//
  interp->AddCommand("show-only",
    //
    "show-only [<varName1> [<varName2> ...]]\n"
    "\t Shows only the given objects in viewer. If no objects are passed, the\n"
    "\t working part will be exclusively shown.",
    //
    __FILE__, group, ENGINE_ShowOnly);

  //-------------------------------------------------------------------------//
  interp->AddCommand("donly",
    //
    "donly [<varName1> [<varName2> ...]]\n"
    "\t Shows only the given objects in viewer. If no objects are passed, the\n"
    "\t working part will be exclusively shown.",
    //
    __FILE__, group, ENGINE_ShowOnly);

  //-------------------------------------------------------------------------//
  interp->AddCommand("erase-all",
    //
    "erase-all\n"
    "\t Erases all objects from the scene.",
    //
    __FILE__, group, ENGINE_EraseAll);

  //-------------------------------------------------------------------------//
  interp->AddCommand("fit",
    //
    "fit\n"
    "\t Fits camera to the scene contents.",
    //
    __FILE__, group, ENGINE_Fit);

  //-------------------------------------------------------------------------//
  interp->AddCommand("select-faces",
    //
    "select-faces <fid1> [<fid2> ...]\n"
    "\t Selects faces specified with their 1-based IDs.",
    //
    __FILE__, group, ENGINE_SelectFaces);
}
