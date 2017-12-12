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

// asiUI includes
#include <asiUI_PickContourCallback.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

//-----------------------------------------------------------------------------

int ENGINE_StartContour(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Prepare data model.
  asiEngine_Part partAPI(cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  //
  cmdEngine::model->OpenCommand();
  {
    // First we build BVH for facets.
    partAPI.BuildBVH();

    // Clear existing contour.
    contour_n->Init();
  }
  cmdEngine::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Set picker type to world picker.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerType(PickType_World);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Add observer which takes responsibility to interact with the user.
  if ( !PM->HasObserver(EVENT_SELECT_WORLD_POINT) )
  {
    vtkSmartPointer<asiUI_PickContourCallback>
      cb = vtkSmartPointer<asiUI_PickContourCallback>::New();
    //
    cb->SetViewer( cmdEngine::cf->ViewerPart );
    cb->SetModel( cmdEngine::model );
    cb->SetBVH( part_n->GetBVH() );
    cb->SetDiagnosticTools( interp->GetProgress(), interp->GetPlotter() );

    // Add observer.
    PM->AddObserver(EVENT_SELECT_WORLD_POINT, cb);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_FinishContour(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  cmdEngine::model->OpenCommand();
  {
    // Finalize contour by setting it closed. Setting this flag does not
    // make any difference unless you ask the Contour Node to build a wire.
    contour_n->SetClosed(true);
  }
  cmdEngine::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Set picker type to cell picker.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerType(PickType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Face);

  // Remove observer.
  PM->RemoveObserver(EVENT_SELECT_WORLD_POINT);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ContourToWire(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Ask Contour Node to return a wire.
  TopoDS_Wire contourGeom = contour_n->AsShape(false);

  // Draw it to pass to the Data Model.
  interp->GetPlotter().REDRAW_SHAPE(argv[1], contourGeom, Color_Red, 1.0, true);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Interactive(const Handle(asiTcl_Interp)&      interp,
                                     const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("start-contour",
    //
    "start-contour \n"
    "\t Enables interactive contour picking.",
    //
    __FILE__, group, ENGINE_StartContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("finish-contour",
    //
    "finish-contour \n"
    "\t Finalizes interactive contour picking.",
    //
    __FILE__, group, ENGINE_FinishContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("contour-to-wire",
    //
    "contour-to-wire varName \n"
    "\t Converts contour to topological wire.",
    //
    __FILE__, group, ENGINE_ContourToWire);
}
