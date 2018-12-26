//-----------------------------------------------------------------------------
// Created on: 26 December 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <cmdRE.h>

// asiAlgo includes
#include <asiAlgo_InterpolateSurfMesh.h>

// asiEngine includes
#include <asiEngine_Part.h>
#include <asiEngine_RE.h>
#include <asiEngine_Triangulation.h>

// asiUI includes
#include <asiUI_PickContourCallback.h>

//-----------------------------------------------------------------------------

int RE_DefineContour(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part & Triangulaion Nodes.
  Handle(asiData_PartNode)          part_n = cmdRE::model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = cmdRE::model->GetTriangulationNode();

  // Contour Node.
  Handle(asiData_ContourNode) contour_n;

  // Prepare data model.
  asiEngine_Part          partAPI ( cmdRE::model, interp->GetProgress(), interp->GetPlotter() );
  asiEngine_Triangulation trisAPI ( cmdRE::model, interp->GetProgress(), interp->GetPlotter() );
  asiEngine_RE            reAPI   ( cmdRE::model, interp->GetProgress(), interp->GetPlotter() );
  //
  cmdRE::model->OpenCommand();
  {
    // Build BVH for CAD-agnostic mesh.
    trisAPI.BuildBVH();
  }
  cmdRE::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdRE::cf->ViewerPart->PrsMgr();

  // Set picker types.
  cmdRE::cf->ViewerPart->GetPickCallback()->SetPickerTypes(PickerType_World | PickerType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Prepare callback which will manage the interaction process with user.
  vtkSmartPointer<asiUI_PickContourCallback>
    cb = vtkSmartPointer<asiUI_PickContourCallback>::New();
  //
  cb->SetViewer        ( cmdRE::cf->ViewerPart );
  cb->SetModel         ( cmdRE::model );
  cb->SetObjectBrowser ( cmdRE::cf->ObjectBrowser );
  //
  if ( !tris_n->GetTriangulation().IsNull() )
    cb->SetBVH( tris_n->GetBVH() );
  //
  cb->SetDiagnosticTools ( interp->GetProgress(), interp->GetPlotter() );

  // Remove previously defined observers.
  if ( PM->HasObserver(EVENT_SELECT_WORLD_POINT) )
    PM->RemoveObservers(EVENT_SELECT_WORLD_POINT);
  //
  if ( PM->HasObserver(EVENT_SELECT_CELL) )
    PM->RemoveObservers(EVENT_SELECT_CELL);

  // Add observer which takes responsibility to interact with the user.
  PM->AddObserver(EVENT_SELECT_WORLD_POINT, cb);
  PM->AddObserver(EVENT_SELECT_CELL,        cb);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_ContourToWire(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdRE::model->GetPartNode();

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

int RE_InterpMesh(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get arguments.
  const int    degU       = atoi(argv[2]);
  const int    degV       = atoi(argv[3]);
  const double grainCoeff = atof(argv[4]);

  // Get Part and Triangulation Nodes.
  Handle(asiData_PartNode)          part_n = cmdRE::model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = cmdRE::model->GetTriangulationNode();

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Get contour.
  std::vector<gp_XYZ> poles;
  contour_n->GetPoints(poles);

  // Prepare interpolation tool.
  asiAlgo_InterpolateSurfMesh algo( tris_n->GetTriangulation(),
                                    interp->GetProgress(),
                                    interp->GetPlotter() );

  // Build interpolant.
  Handle(Geom_BSplineSurface) surf;
  //
  if ( !algo.Perform(poles, grainCoeff, degU, degV, surf) )
  {
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Cannot interpolate mesh.");
    return TCL_OK;
  }

  // Set result.
  interp->GetPlotter().REDRAW_SURFACE(argv[1], surf, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdRE::Commands_Interaction(const Handle(asiTcl_Interp)&      interp,
                                     const Handle(Standard_Transient)& data)
{
  cmdRE_NotUsed(data);
  //
  static const char* group = "cmdRE";

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-define-contour",
    //
    "re-define-contour\n"
    "\t Enables interactive contour picking.",
    //
    __FILE__, group, RE_DefineContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-contour-to-wire",
    //
    "re-contour-to-wire varName\n"
    "\t Converts contour to topological wire.",
    //
    __FILE__, group, RE_ContourToWire);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-interp-mesh",
    //
    "re-interp-mesh surfName udeg vdeg grain_factor\n"
    "\t Interpolates mesh.",
    //
    __FILE__, group, RE_InterpMesh);
}
