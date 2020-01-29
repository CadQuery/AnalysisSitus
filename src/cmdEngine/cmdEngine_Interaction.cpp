//-----------------------------------------------------------------------------
// Created on: 24 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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
#include <asiEngine_IV.h>
#include <asiEngine_Part.h>
#include <asiEngine_Triangulation.h>

// asiUI includes
#include <asiUI_AddCurveHandleCallback.h>
#include <asiUI_MoveCurveHandleCallback.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

//-----------------------------------------------------------------------------

int ENGINE_AddCurveHandle(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // API to manage primitives of imperative plotter.
  asiEngine_IV IV(cmdEngine::model);

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Enable interactive picking.
  IV.ActivateCurveHandles(true, true, PM);

  // Set picker types.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerTypes(PickerType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Prepare callback which will manage the interaction process with user.
  vtkSmartPointer<asiUI_AddCurveHandleCallback>
    cb = vtkSmartPointer<asiUI_AddCurveHandleCallback>::New();
  //
  cb->SetViewer          ( cmdEngine::cf->ViewerPart );
  cb->SetModel           ( cmdEngine::model );
  cb->SetDiagnosticTools ( interp->GetProgress(), interp->GetPlotter() );

  // Remove previously defined observers.
  if ( PM->HasObserver(EVENT_SELECT_CELL) )
    PM->RemoveObservers(EVENT_SELECT_CELL);

  // Add observer which takes responsibility to interact with the user.
  PM->AddObserver(EVENT_SELECT_CELL, cb);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MoveCurveHandle(const Handle(asiTcl_Interp)& interp,
                           int                          argc,
                           const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // API to manage primitives of imperative plotter.
  asiEngine_IV IV(cmdEngine::model);

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Enable interactive picking.
  IV.ActivateCurveHandles(true, false, PM);

  // Set picker types.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerTypes(PickerType_World | PickerType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Prepare callback which will manage the interaction process with user.
  vtkSmartPointer<asiUI_MoveCurveHandleCallback>
    cb = vtkSmartPointer<asiUI_MoveCurveHandleCallback>::New();
  //
  cb->SetViewer          ( cmdEngine::cf->ViewerPart );
  cb->SetModel           ( cmdEngine::model );
  cb->SetDiagnosticTools ( interp->GetProgress(), interp->GetPlotter() );

  // Remove previously defined observers.
  if ( PM->HasObserver(EVENT_SELECT_WORLD_POINT) )
    PM->RemoveObservers(EVENT_SELECT_WORLD_POINT);
  //
  if ( PM->HasObserver(EVENT_SELECT_CELL) )
    PM->RemoveObservers(EVENT_SELECT_CELL);
  //
  if ( PM->HasObserver(EVENT_DETECT_CELL) )
    PM->RemoveObservers(EVENT_DETECT_CELL);

  // Add observer which takes responsibility to interact with the user.
  PM->AddObserver(EVENT_SELECT_WORLD_POINT, cb);
  PM->AddObserver(EVENT_SELECT_CELL,        cb);

  // Prepare data model.
  asiEngine_Triangulation trisAPI( cmdEngine::model, interp->GetProgress(), interp->GetPlotter() );
  //
  cmdEngine::model->OpenCommand();
  {
    // Build BVH for CAD-agnostic mesh.
    trisAPI.BuildBVH();
  }
  cmdEngine::model->CommitCommand();

  // Get Triangulaion Node.
  Handle(asiData_TriangulationNode) tris_n = cmdEngine::model->GetTriangulationNode();
  //
  if ( !tris_n->GetTriangulation().IsNull() )
    cb->SetBVH( tris_n->GetBVH() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Interaction(const Handle(asiTcl_Interp)&      interp,
                                     const Handle(Standard_Transient)& cmdEngine_NotUsed(data))
{
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("add-curve-handle",
    //
    "add-curve-handle\n"
    "\t Allows to add interactively on-curve handles.",
    //
    __FILE__, group, ENGINE_AddCurveHandle);

  //-------------------------------------------------------------------------//
  interp->AddCommand("move-curve-handle",
    //
    "move-curve-handle\n"
    "\t Allows to move the selected curve handle to another (picked) position.",
    //
    __FILE__, group, ENGINE_MoveCurveHandle);
}
