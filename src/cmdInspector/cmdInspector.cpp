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

// cmdInspector includes
#include <cmdInspector.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiAlgo includes
#include <asiAlgo_TopoKill.h>

// OCCT includes
#include <ShapeFix_Shape.hxx>

//-----------------------------------------------------------------------------

Handle(exe_CommonFacilities) cmdInspector::cf = NULL;

//-----------------------------------------------------------------------------

int INSPECTOR_SetAsPart(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiData_IVTopoItemNode)
    node = Handle(asiData_IVTopoItemNode)::DownCast( cmdInspector::cf->Model->FindNodeByName(argv[1]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_ERROR;
  }

  // Modify Data Model
  cmdInspector::cf->Model->OpenCommand();
  {
    asiEngine_Part(cmdInspector::cf->Model, NULL).Update( node->GetShape() );
  }
  cmdInspector::cf->Model->CommitCommand();

  // Update UI
  cmdInspector::UpdateUI();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int INSPECTOR_FixPart(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdInspector::cf->Model->GetPartNode();

  // Fix shape
  ShapeFix_Shape fix( part_n->GetShape() );
  fix.Perform();
  TopoDS_Shape result = fix.Shape();

  // Modify Data Model
  cmdInspector::cf->Model->OpenCommand();
  {
    asiEngine_Part(cmdInspector::cf->Model, NULL).Update(result);
  }
  cmdInspector::cf->Model->CommitCommand();

  // Update UI
  cmdInspector::UpdateUI();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int INSPECTOR_KillEdge(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int eidx = atoi(argv[1]);
  //
  if ( eidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge index should be 1-based.");
    return TCL_ERROR;
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdInspector::cf->Model->GetPartNode();

  // Get map of edges with respect to those the passed index is relevant
  const TopTools_IndexedMapOfShape& allEdges = part_n->GetAAG()->GetMapOfEdges();

  // Prepare killer
  asiAlgo_TopoKill killer( cmdInspector::cf->Model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( !killer.AskRemove( allEdges(eidx) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of edge %1 was rejected." << eidx);
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model
  cmdInspector::cf->Model->OpenCommand();
  {
    asiEngine_Part(cmdInspector::cf->Model, NULL).Update(result);
  }
  cmdInspector::cf->Model->CommitCommand();

  // Update UI
  cmdInspector::UpdateUI();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int INSPECTOR_KillFace(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int fidx = atoi(argv[1]);
  //
  if ( fidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face index should be 1-based.");
    return TCL_ERROR;
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdInspector::cf->Model->GetPartNode();

  // Get map of faces with respect to those the passed index is relevant
  const TopTools_IndexedMapOfShape& allFaces = part_n->GetAAG()->GetMapOfFaces();

  // Prepare killer
  asiAlgo_TopoKill killer( cmdInspector::cf->Model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( !killer.AskRemove( allFaces(fidx) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of face %1 was rejected." << fidx);
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model
  cmdInspector::cf->Model->OpenCommand();
  {
    asiEngine_Part(cmdInspector::cf->Model, NULL).Update(result);
  }
  cmdInspector::cf->Model->CommitCommand();

  // Update UI
  cmdInspector::UpdateUI();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdInspector::UpdateUI()
{
  cf->ViewerPart->PrsMgr()->Actualize(cf->Model->GetPartNode(), false, false, true);
}

//-----------------------------------------------------------------------------

void cmdInspector::Factory(const Handle(asiTcl_Interp)&      interp,
                           const Handle(Standard_Transient)& data)
{
  static const char* group = "cmdInspector";

  /* ==============================
   *  Initialize common facilities
   * ============================== */

  cmdInspector::cf = Handle(exe_CommonFacilities)::DownCast(data);
  //
  if ( cf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[cmdInspector] Common facilities are NULL.");
    return;
  }

  /* =====================
   *  Add custom commands
   * ===================== */

  // Add commands
  interp->AddCommand("set-as-part", "", __FILE__, group, INSPECTOR_SetAsPart);
  interp->AddCommand("fix-part",    "", __FILE__, group, INSPECTOR_FixPart);
  interp->AddCommand("kill-edge",   "", __FILE__, group, INSPECTOR_KillEdge);
  interp->AddCommand("kill-face",   "", __FILE__, group, INSPECTOR_KillFace);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdInspector)
