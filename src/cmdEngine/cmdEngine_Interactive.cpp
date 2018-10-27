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
#include <asiEngine_RE.h>
#include <asiEngine_Triangulation.h>

// asiUI includes
#include <asiUI_PickContourCallback.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_InterpolateSurfMesh.h>
#include <asiAlgo_ReapproxContour.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

#ifdef USE_MOBIUS
  // Mobius includes
  #include <mobius/cascade_BSplineSurface.h>
  #include <mobius/geom_InterpolateSurface.h>
#endif

// OCCT includes
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Plane.hxx>
#include <ShapeAnalysis_Surface.hxx>

//-----------------------------------------------------------------------------

int ENGINE_ProjectContour(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part & Triangulaion Nodes.
  Handle(asiData_PartNode)          part_n = cmdEngine::model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = cmdEngine::model->GetTriangulationNode();

  // Contour Node.
  Handle(asiData_ContourNode) contour_n;

  // Prepare data model.
  asiEngine_Part          partAPI ( cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  asiEngine_Triangulation trisAPI ( cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  asiEngine_RE            reAPI   ( cmdEngine::model, interp->GetProgress(), interp->GetPlotter() );
  //
  cmdEngine::model->OpenCommand();
  {
    // First we build BVH for facets (both part and CAD-agnostic mesh).
    partAPI.BuildBVH();
    trisAPI.BuildBVH();

    // Clean up the currently active contour.
    contour_n = reAPI.GetOrCreate_Contour();
    contour_n->Init();
  }
  cmdEngine::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Set picker type to world picker.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerTypes(PickerType_World | PickerType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Workpiece);

  // Add observer which takes responsibility to interact with the user.
  if ( PM->HasObserver(EVENT_SELECT_WORLD_POINT) )
    PM->RemoveObservers(EVENT_SELECT_WORLD_POINT);

  vtkSmartPointer<asiUI_PickContourCallback>
    cb = vtkSmartPointer<asiUI_PickContourCallback>::New();
  //
  cb->SetViewer      ( cmdEngine::cf->ViewerPart );
  cb->SetModel       ( cmdEngine::model );
  cb->SetContourNode ( contour_n );
  //
  if ( !part_n->GetShape().IsNull() )
    cb->AddBVH( part_n->GetBVH() );
  if ( !tris_n->GetTriangulation().IsNull() )
    cb->AddBVH( tris_n->GetBVH() );
  //
  cb->SetDiagnosticTools ( interp->GetProgress(), interp->GetPlotter() );

  // Add observer.
  PM->AddObserver(EVENT_SELECT_WORLD_POINT, cb);

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

  // Contour Node.
  Handle(asiData_ContourNode) contour_n;

  cmdEngine::model->OpenCommand();
  {
    contour_n = asiEngine_RE( cmdEngine::model,
                              interp->GetProgress(),
                              interp->GetPlotter() ).GetOrCreate_Contour();

    // Finalize contour by setting it closed. Setting this flag does not
    // make any difference unless you ask the Contour Node to build a wire.
    contour_n->SetClosed(true);

    // Get contour as a topological wire.
    TopoDS_Wire contourWire = contour_n->AsShape();

    // Beautify contour.
    asiAlgo_ReapproxContour reapproxContour( contourWire,
                                             1.0e-6,
                                             5.0,
                                             interp->GetProgress(),
                                             interp->GetPlotter() );
    //
    if ( !reapproxContour(true, true, contourWire) )
    {
      interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Cannot reapproximate contour.");
    }
    else
    {
      contour_n->SetShape(contourWire);
    }
  }
  cmdEngine::model->CommitCommand();

  // Get Part presentation manager.
  const vtkSmartPointer<asiVisu_PrsManager>& PM = cmdEngine::cf->ViewerPart->PrsMgr();

  // Set picker type to cell picker.
  cmdEngine::cf->ViewerPart->GetPickCallback()->SetPickerTypes(PickerType_Cell);

  // Set selection mode.
  PM->SetSelectionMode(SelectionMode_Face);

  // Remove observer.
  PM->RemoveObserver(EVENT_SELECT_WORLD_POINT);

  // Update contour.
  PM->Actualize(contour_n);

  // Update UI.
  cmdEngine::cf->ObjectBrowser->Populate();

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

int ENGINE_InterpMesh(const Handle(asiTcl_Interp)& interp,
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
  Handle(asiData_PartNode)          part_n = cmdEngine::model->GetPartNode();
  Handle(asiData_TriangulationNode) tris_n = cmdEngine::model->GetTriangulationNode();

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

void cmdEngine::Commands_Interactive(const Handle(asiTcl_Interp)&      interp,
                                     const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("project-contour",
    //
    "project-contour\n"
    "\t Enables interactive contour picking.",
    //
    __FILE__, group, ENGINE_ProjectContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("finish-contour",
    //
    "finish-contour\n"
    "\t Finalizes interactive contour picking.",
    //
    __FILE__, group, ENGINE_FinishContour);

  //-------------------------------------------------------------------------//
  interp->AddCommand("contour-to-wire",
    //
    "contour-to-wire varName\n"
    "\t Converts contour to topological wire.",
    //
    __FILE__, group, ENGINE_ContourToWire);

  //-------------------------------------------------------------------------//
  interp->AddCommand("interp-mesh",
    //
    "interp-mesh surfName udeg vdeg grain_factor\n"
    "\t Interpolates mesh.",
    //
    __FILE__, group, ENGINE_InterpMesh);
}
