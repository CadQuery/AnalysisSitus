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
#include <asiEngine_Triangulation.h>

// asiUI includes
#include <asiUI_PickContourCallback.h>

// asiAlgo includes
#include <asiAlgo_PlaneOnPoints.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// OCCT includes
#include <Geom_Plane.hxx>
#include <ShapeAnalysis_Surface.hxx>

//-----------------------------------------------------------------------------

int ENGINE_StartContour(const Handle(asiTcl_Interp)& interp,
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

  // Get Contour Node.
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "No persistent data for contour available.");
    return TCL_OK;
  }

  // Prepare data model.
  asiEngine_Part          partAPI ( cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  asiEngine_Triangulation trisAPI ( cmdEngine::model, NULL, interp->GetProgress(), interp->GetPlotter() );
  //
  cmdEngine::model->OpenCommand();
  {
    // First we build BVH for facets (both part and CAD-agnostic mesh).
    partAPI.BuildBVH();
    trisAPI.BuildBVH();

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
    cb->SetViewer ( cmdEngine::cf->ViewerPart );
    cb->SetModel  ( cmdEngine::model );
    //
    if ( !part_n->GetShape().IsNull() )
      cb->AddBVH( part_n->GetBVH() );
    if ( !tris_n->GetTriangulation().IsNull() )
      cb->AddBVH( tris_n->GetBVH() );
    //
    cb->SetDiagnosticTools ( interp->GetProgress(), interp->GetPlotter() );

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

  // Update contour.
  PM->Actualize(contour_n);

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

int ENGINE_MakeContourPlane(const Handle(asiTcl_Interp)& interp,
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

  // Get coordinates of the contour's poles.
  std::vector<gp_XYZ> points;
  gp_XYZ P_center;
  //
  contour_n->GetPoints(points, P_center);

  // Get characteristic size.
  double size = 0.0;
  //
  for ( size_t k = 0; k < points.size(); ++k )
    size = Max( size, (points[k] - P_center).Modulus() );
  //
  size *= 1.1;

  // Prepare utility to build average plane.
  gp_Pln plane;
  asiAlgo_PlaneOnPoints planeBuilder( interp->GetProgress(), interp->GetPlotter() );
  //
  if ( !planeBuilder.Build(points, plane) )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Cannot build average plane.");
    return TCL_OK;
  }
  //
  interp->GetPlotter().REDRAW_SURFACE(argv[1], new Geom_Plane(plane), size, size, Color_Green, 0.5);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ProjectContourOnPlane(const Handle(asiTcl_Interp)& interp,
                                 int                          argc,
                                 const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Surface Node.
  Handle(asiData_IVSurfaceNode)
    node = Handle(asiData_IVSurfaceNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a Surface object with name %1." << argv[2]);
    return TCL_OK;
  }

  // Get geometry of a surface.
  Handle(Geom_Surface) surface = node->GetSurface();
  //
  if ( surface.IsNull() || !surface->IsKind( STANDARD_TYPE(Geom_Plane) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Surface in question is null or not a plane.");
    return TCL_OK;
  }
  //
  Handle(Geom_Plane) plane = Handle(Geom_Plane)::DownCast(surface);

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

  /* ==============================================================
   *  Get image of the contour in (U,V) space of the average plane
   * ============================================================== */

  // Get coordinates of the contour's poles.
  std::vector<gp_XYZ> points;
  gp_XYZ P_center;
  //
  contour_n->GetPoints(points, P_center);

  // Get characteristic size.
  double size = 0.0;
  //
  for ( size_t k = 0; k < points.size(); ++k )
    size = Max( size, (points[k] - P_center).Modulus() );
  //
  size *= 1.1;

  // Project points on surface.
  std::vector<gp_XY> contourPolesUV;
  //
  for ( size_t k = 0; k < points.size(); ++k )
  {
    ShapeAnalysis_Surface SAS(plane);
    gp_Pnt2d UV = SAS.ValueOfUV(points[k], 1.0e-1);
    //
    contourPolesUV.push_back( UV.XY() );

    interp->GetPlotter().DRAW_POINT(UV,                      Color_Yellow, "UV");
    //interp->GetPlotter().DRAW_POINT(gp_XY( UV.X(), 0      ), Color_Red,    "U");
    //interp->GetPlotter().DRAW_POINT(gp_XY( 0,      UV.Y() ), Color_Green,  "V");

    interp->GetPlotter().DRAW_LINK(gp_XY( UV.X(), 0      ), gp_XY( UV.X(),  size   ), Color_White, "U_iso_p");
    interp->GetPlotter().DRAW_LINK(gp_XY( UV.X(), 0      ), gp_XY( UV.X(), -size   ), Color_White, "U_iso_m");
    interp->GetPlotter().DRAW_LINK(gp_XY( 0,      UV.Y() ), gp_XY( size,    UV.Y() ), Color_White, "V_iso_p");
    interp->GetPlotter().DRAW_LINK(gp_XY( 0,      UV.Y() ), gp_XY(-size,    UV.Y() ), Color_White, "V_iso_m");
  }

  /* ====================================================================
   *  For each image point, generate all possible variants of grid nodes
   * ==================================================================== */

  std::vector<gp_XY> generated;
  for ( size_t k = 0; k < contourPolesUV.size(); ++k )
  {
    const gp_XY& uv_k = contourPolesUV[k];
    const double u_k  = uv_k.X();
    const double v_k  = uv_k.Y();

    for ( size_t s = 0; s < contourPolesUV.size(); ++s )
    {
      if ( s == k ) continue;

      const gp_XY& uv_s = contourPolesUV[s];
      const double u_s  = uv_s.X();
      const double v_s  = uv_s.Y();

      gp_XY gen_uv_horisontal(u_k, v_s);
      gp_XY gen_uv_vertical(u_s, v_k);
      //
      generated.push_back(gen_uv_horisontal);
      generated.push_back(gen_uv_vertical);
    }
  }

  for ( size_t k = 0; k < generated.size(); ++k )
    interp->GetPlotter().DRAW_POINT(generated[k], Color_Blue, "generated");

  // TODO: NYI

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
    "start-contour\n"
    "\t Enables interactive contour picking.",
    //
    __FILE__, group, ENGINE_StartContour);

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
  interp->AddCommand("make-contour-plane",
    //
    "make-contour-plane planeName\n"
    "\t Creates average plane for the active contour.",
    //
    __FILE__, group, ENGINE_MakeContourPlane);

  //-------------------------------------------------------------------------//
  interp->AddCommand("project-contour-on-plane",
    //
    "project-contour-on-plane planeName\n"
    "\t Projects active contour to the plane with the given name.",
    //
    __FILE__, group, ENGINE_ProjectContourOnPlane);
}
