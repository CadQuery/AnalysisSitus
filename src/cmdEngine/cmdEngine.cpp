//-----------------------------------------------------------------------------
// Created on: 24 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// cmdEngine includes
#include <cmdEngine.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiAlgo includes
#include <asiAlgo_TopoGraph.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_DialogCommands.h>

// OCCT includes
#include <TopExp_Explorer.hxx>

//-----------------------------------------------------------------------------

Handle(asiEngine_Model) cmdEngine::model = NULL;

//-----------------------------------------------------------------------------

int ENGINE_Commands(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // TODO: NYI

  asiUI_DialogCommands*
    pDlg = new asiUI_DialogCommands(interp, interp->GetProgress(), NULL);
  //
  pDlg->show();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_PartExplode(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Explode
  TCollection_AsciiString basename = "exp ";
  for ( TopoDS_Iterator it(partShape); it.More(); it.Next() )
  {
    const TopoDS_Shape& subShape = it.Value();

    // Generate name
    TCollection_AsciiString name = basename + asiAlgo_Utils::ShapeTypeStr(subShape).c_str();

    // Draw imperatively (populates Data Model)
    interp->GetPlotter().DRAW_SHAPE(subShape, name);
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\t%1" << name);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_PartSummary(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Build explicit topology graph
  asiAlgo_TopoGraph topograph(partShape);

  int numCompounds  = 0,
      numCompSolids = 0,
      numSolids     = 0,
      numShells     = 0,
      numFaces      = 0,
      numWires      = 0,
      numEdges      = 0,
      numVertices   = 0;
  //
  topograph.CalculateSummary(numCompounds,
                             numCompSolids,
                             numSolids,
                             numShells,
                             numFaces,
                             numWires,
                             numEdges,
                             numVertices);

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "=============================================");
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Summary of part's contents");
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "---------------------------------------------");
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. compounds:  %1" << numCompounds);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. compsolids: %1" << numCompSolids);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. solids:     %1" << numSolids);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. shells:     %1" << numShells);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. faces:      %1" << numFaces);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. wires:      %1" << numWires);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. edges:      %1" << numEdges);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. vertices:   %1" << numVertices);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Factory(const Handle(asiTcl_Interp)&      interp,
                        const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);

  static const char* group = "cmdEngine";

  /* ================================
   *  Initialize Data Model instance
   * ================================ */

  model = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  if ( model.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[cmdEngine] Data Model instance is NULL or not of asiEngine_Model kind.");
    return;
  }

  /* =====================
   *  Add custom commands
   * ===================== */

  // Add commands
  interp->AddCommand("commands",     "", __FILE__, group, ENGINE_Commands);
  interp->AddCommand("part-explode", "", __FILE__, group, ENGINE_PartExplode);
  interp->AddCommand("part-summary", "", __FILE__, group, ENGINE_PartSummary);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdEngine)
