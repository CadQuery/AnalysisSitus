//-----------------------------------------------------------------------------
// Created on: 23 August 2017
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

// cmdMisc includes
#include <cmdMisc.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_CommonFacilities.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Model.h>

// OCCT includes
#include <BOPAlgo_Splitter.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GeomAPI.hxx>
#include <gp_Pln.hxx>
#include <IntTools_FClass2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

double SignedDistance(const gp_Pln& pln, const gp_Pnt& P)
{
  const gp_Pnt& loc = pln.Position().Location();
  const gp_Dir& dir = pln.Position().Direction();
  double D = ( dir.X() * ( P.X() - loc.X() ) +
               dir.Y() * ( P.Y() - loc.Y() ) +
               dir.Z() * ( P.Z() - loc.Z() ) );
  return D;
}

//-----------------------------------------------------------------------------

int MISC_Box(const Handle(asiTcl_Interp)& interp,
             int                          argc,
             const char**                 argv)
{
  TopoDS_Shape box = BRepPrimAPI_MakeBox(1, 1, 1);

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Box created");
  interp->GetPlotter().DRAW_SHAPE(box, "box");
  return 0;
}

//-----------------------------------------------------------------------------

int MISC_TestHexagonBops(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  /* =====================
   *  Build solid hexagon
   * ===================== */

  TIMER_NEW
  TIMER_GO

  // Get hexagon poles in 2D
  gp_XY Poles2d[6];
  asiAlgo_Utils::HexagonPoles(gp::Origin2d().XY(),
                              1.0,
                              Poles2d[0],
                              Poles2d[1],
                              Poles2d[2],
                              Poles2d[3],
                              Poles2d[4],
                              Poles2d[5]);

  // Choose working plane
  Handle(Geom_Plane) workPlane = new Geom_Plane( gp_Pln( gp::Origin(), gp::DZ() ) );

  // Populate wire builder
  BRepBuilderAPI_MakePolygon mkPoly;
  for ( int k = 0; k < 6; ++k )
    mkPoly.Add( workPlane->Value( Poles2d[k].X(), Poles2d[k].Y() ) );
  //
  mkPoly.Add( workPlane->Value( Poles2d[0].X(), Poles2d[0].Y() ) );

  // Build wire
  mkPoly.Build();
  const TopoDS_Wire& wire = mkPoly.Wire();

  // Build face
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  // Build prism
  TopoDS_Shape prism = BRepPrimAPI_MakePrism(face, gp_Vec( workPlane->Axis().Direction() )*0.25);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress().Access(), "Build hexagon solid")

  interp->GetPlotter().REDRAW_SHAPE("contour", wire, Color_Red, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("face", face);
  interp->GetPlotter().REDRAW_SHAPE("prism", prism);

  /* =================
   *  Build cylinders
   * ================= */

  TIMER_RESET
  TIMER_GO

  // Build bounding box for the hexa
  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(prism, xMin, yMin, zMin, xMax, yMax, zMax);

  const double r = 0.02;
  const double h = 0.25;
  const double step = 0.08;

  BRep_Builder BB;
  TopoDS_Compound cyls;
  BB.MakeCompound(cyls);

  // List for cutter
  TopTools_ListOfShape tools;

  // Prepare two-dimensional classifier
  IntTools_FClass2d classifier;
  classifier.Init( face, Precision::Confusion() );

  // Count number of primitives which pass the test
  int cylCount = 0;

  double xcurr = xMin;
  do
  {
    double ycurr = yMin;

    do
    {
      // Classify center point
      const TopAbs_State state = classifier.TestOnRestriction( gp_Pnt2d(xcurr, ycurr), 0.05 );
      //
      if ( state == TopAbs_IN )
      {
        gp_Pnt cylcenter(xcurr, ycurr, 0.0);
        TopoDS_Shape cyl = BRepPrimAPI_MakeCylinder( gp_Ax2(cylcenter, gp::DZ() ), r, 0.25 );
        //
        BB.Add(cyls, cyl);
        tools.Append(cyl);

        cylCount++;
      }

      ycurr += step;
    }
    while ( ycurr < yMax );

    xcurr += step;
  }
  while ( xcurr < xMax );

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress().Access(), "Build cylinders")

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of cylinders: %1" << cylCount);
  interp->GetPlotter().REDRAW_SHAPE("cyls", cyls, Color_Yellow);

  /* ================
   *  Cut primitives
   * ================ */

  TIMER_RESET
  TIMER_GO

  TopoDS_Shape result = asiAlgo_Utils::BooleanCut(prism, tools, false);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress().Access(), "Cut cylinders")

  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return 0;
}

//-----------------------------------------------------------------------------

int MISC_PushPull(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode()->GetShape();

  int faceId = atoi(argv[1]); // 1-based

  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(partShape, TopAbs_FACE, M);
  TopoDS_Shape faceShape = M.FindKey(faceId);

  interp->GetPlotter().DRAW_SHAPE(faceShape, Color_Blue, 1.0, true, "faceShape");

  gp_Vec offset = gp::DX().XYZ() * atof(argv[2]);

  TopoDS_Shape prism = BRepPrimAPI_MakePrism(faceShape, offset);
  interp->GetPlotter().REDRAW_SHAPE("prism", prism, Color_Blue);

  TopoDS_Shape fused = BRepAlgoAPI_Fuse(prism, partShape);
  interp->GetPlotter().REDRAW_SHAPE("fused", fused, Color_Green);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestExtend(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ================
   *  Process inputs
   * ================ */

  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode()->GetShape();

  enum t_dim
  {
    dim_X, dim_Y, dim_Z
  };

  enum t_dir
  {
    dir_up, dir_down, dir_both
  };

  // Take dimension of interest
  t_dim dim;
  TCollection_AsciiString dimStr(argv[1]);
  //
  if ( dimStr == "X" )
    dim = dim_X;
  else if ( dimStr == "Y" )
    dim = dim_Y;
  else if ( dimStr == "Z" )
    dim = dim_Z;
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected dimension.");
    return TCL_ERROR;
  }

  // Take direction of interest
  t_dir dir;
  TCollection_AsciiString dirStr(argv[2]);
  //
  if ( dirStr == "up" )
    dir = dir_up;
  else if ( dirStr == "down" )
    dir = dir_down;
  else if ( dirStr == "both" )
    dir = dir_both;
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected direction.");
    return TCL_ERROR;
  }

  // Take offset value
  const double offset = atof(argv[3]);

  /* =================================
   *  Build AABB for the working part
   * ================================= */

  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(partShape, xMin, yMin, zMin, xMax, yMax, zMax);
  //
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Bounding box:\n"
                                                           "\t X min = %1\n"
                                                           "\t Y min = %2\n"
                                                           "\t Z min = %3\n"
                                                           "\t X max = %4\n"
                                                           "\t Y max = %5\n"
                                                           "\t Z max = %6"
                                                        << xMin << yMin << zMin
                                                        << xMax << yMax << zMax );

  // Create bounding box to draw it
  TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
  //
  interp->GetPlotter().DRAW_SHAPE(bndbox, Color_Yellow, 1.0, true, "bounding box");

  /* ======================
   *  Build section planes
   * ====================== */

  const double xMid = (xMin + xMax)*0.5;
  const double yMid = (yMin + yMax)*0.5;
  const double zMid = (zMin + zMax)*0.5;
  //
  gp_Pnt P_mid(xMid, yMid, zMid);

  // Build infinite planes for slicing
  Handle(Geom_Plane) nX_plane = new Geom_Plane( P_mid, gp::DX() );
  Handle(Geom_Plane) nY_plane = new Geom_Plane( P_mid, gp::DY() );
  Handle(Geom_Plane) nZ_plane = new Geom_Plane( P_mid, gp::DZ() );

  double maxDelta = Max( xMax - xMid, Max(yMax - yMid, zMax - zMid) );
  maxDelta += maxDelta*0.1;

  // Draw
  interp->GetPlotter().DRAW_SURFACE(nX_plane, maxDelta, maxDelta, Color_Red,   0.7, "nX_plane");
  interp->GetPlotter().DRAW_SURFACE(nY_plane, maxDelta, maxDelta, Color_Green, 0.7, "nY_plane");
  interp->GetPlotter().DRAW_SURFACE(nZ_plane, maxDelta, maxDelta, Color_Blue,  0.7, "nZ_plane");

  // Choose the working section plane
  Handle(Geom_Plane) sectionPlane;
  switch ( dim )
  {
    case dim_X: sectionPlane = nX_plane; break;
    case dim_Y: sectionPlane = nY_plane; break;
    case dim_Z: sectionPlane = nZ_plane; break;
    default: break;
  }

  TopoDS_Face sectionFace = BRepBuilderAPI_MakeFace(sectionPlane->Pln(),
                                                   -maxDelta,
                                                    maxDelta,
                                                   -maxDelta,
                                                    maxDelta);

  /* =============
   *  Split solid
   * ============= */

  // Prepare objects and tools
  BOPCol_ListOfShape objects;
  objects.Append(partShape);
  //
  BOPCol_ListOfShape tools;
  tools.Append(sectionFace);

  BOPAlgo_Splitter mkSplit;
  mkSplit.SetArguments(objects);
  mkSplit.SetTools(tools);
  mkSplit.Perform();
  mkSplit.SetGlue(BOPAlgo_GlueFull);
  //
  if ( mkSplit.HasErrors() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "BOP splitter failed.");
    return TCL_ERROR;
  }

  // Get result
  const TopoDS_Shape& resVols = mkSplit.Shape();
  //
  interp->GetPlotter().REDRAW_SHAPE("resVols", resVols);

  // Get output solids: there should be two
  std::vector<TopoDS_Solid> solids;
  //
  for ( TopExp_Explorer exp(resVols, TopAbs_SOLID); exp.More(); exp.Next() )
  {
    solids.push_back( TopoDS::Solid( exp.Current() ) );
  }
  //
  if ( solids.size() != 2 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "BOP splitter is expected to return 2 solids while returned %1."
                                                        << int ( solids.size() ) );
    return TCL_ERROR;
  }

  // Get image of the section face. This image is necessary to prepare the
  // tool object.
  const TopTools_ListOfShape& modifiedSectionList = mkSplit.Modified(sectionFace);
  //
  TopoDS_Compound modifiedSection;
  BRep_Builder().MakeCompound(modifiedSection);
  //
  for ( TopTools_ListIteratorOfListOfShape lit(modifiedSectionList); lit.More(); lit.Next() )
  {
    BRep_Builder().Add( modifiedSection, lit.Value() );
  }
  //
  interp->GetPlotter().REDRAW_SHAPE("modifiedSection", modifiedSection);

  /* ================================================
   *  Prepare deep copy to resolve non-manifold face
   * ================================================ */

  solids[0] = TopoDS::Solid( BRepBuilderAPI_Copy(solids[0]) );

  // Redraw
  for ( size_t s = 0; s < solids.size(); ++s )
  {
    interp->GetPlotter().DRAW_SHAPE(solids[s], "spart");
  }

  /* ======================================================
   *  Attribute solids as belonging to up/down half-spaces
   * ====================================================== */

  TopoDS_Solid upSolid, downSolid;

  for ( size_t s = 0; s < solids.size(); ++s )
  {
    // Get center of solid
    double xMin, yMin, zMin, xMax, yMax, zMax;
    asiAlgo_Utils::Bounds(solids[s], xMin, yMin, zMin, xMax, yMax, zMax);
    //
    gp_Pnt P_mid( 0.5*(xMin + xMax), 0.5*(yMin + yMax), 0.5*(zMin + zMax) );

    // Classify wrt half-space
    if ( SignedDistance(sectionPlane->Pln(), P_mid) > 0 )
      upSolid = solids[s];
    else
      downSolid = solids[s];
  }

  // Redraw
  interp->GetPlotter().REDRAW_SHAPE("upSolid", upSolid, Color_Red, 0.5);
  interp->GetPlotter().REDRAW_SHAPE("downSolid", downSolid, Color_Blue, 0.5);

  /* ======================================
   *  Move solids wrt the passed direction
   * ====================================== */

  if ( dir == dir_up )
  {
    // Prepare transformation
    gp_Trsf T; T.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() )*offset );

    upSolid = TopoDS::Solid( BRepBuilderAPI_Transform(upSolid, T, true) );
  }
  else if ( dir == dir_down )
  {
    // Prepare transformation
    gp_Trsf T; T.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset );

    downSolid = TopoDS::Solid( BRepBuilderAPI_Transform(downSolid, T, true) );
  }
  else if ( dir == dir_both )
  {
    // Prepare transformation
    gp_Trsf T_down; T_down.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset*0.5 );
    gp_Trsf T_up; T_up.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() )*offset*0.5 );

    downSolid = TopoDS::Solid( BRepBuilderAPI_Transform(downSolid, T_down, true) );
    upSolid = TopoDS::Solid( BRepBuilderAPI_Transform(upSolid, T_up, true) );
  }

  // Redraw
  interp->GetPlotter().REDRAW_SHAPE("upSolid_moved_shaded",   upSolid,   Color_Red,   0.5, false);
  interp->GetPlotter().REDRAW_SHAPE("downSolid_moved_shaded", downSolid, Color_Blue,  0.5, false);
  interp->GetPlotter().REDRAW_SHAPE("upSolid_moved",          upSolid,   Color_White, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("downSolid_moved",        downSolid, Color_White, 1.0, true);

  /* =====================
   *  Prepare tool object
   * ===================== */

  TopoDS_Shape toolObj;

  if ( dir == dir_up )
  {
    // Prepare extrusion vector
    gp_Vec vec = gp_Vec( sectionPlane->Axis().Direction() )*offset;
    //
    toolObj = BRepPrimAPI_MakePrism(modifiedSection, vec, true);
  }
  else if ( dir == dir_down )
  {
    // Prepare extrusion vector
    gp_Vec vec = gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset;
    //
    toolObj = BRepPrimAPI_MakePrism(modifiedSection, vec, true);
  }
  else if ( dir == dir_both )
  {
    // Prepare extrusion vector
    gp_Vec vecUp   = gp_Vec( sectionPlane->Axis().Direction() )*offset*0.5;
    gp_Vec vecDown = gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset*0.5;
    //
    TopoDS_Shape toolUp   = BRepPrimAPI_MakePrism(modifiedSection, vecUp, true);
    TopoDS_Shape toolDown = BRepPrimAPI_MakePrism(modifiedSection, vecDown, true);

    TopoDS_Compound toolComp;
    BRep_Builder().MakeCompound(toolComp);
    BRep_Builder().Add(toolComp, toolUp);
    BRep_Builder().Add(toolComp, toolDown);
    //
    toolObj = toolComp;
  }
  //
  interp->GetPlotter().REDRAW_SHAPE("toolObj", toolObj, Color_Magenta);

  // TODO: NYI

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdMisc::Factory(const Handle(asiTcl_Interp)&      interp,
                      const Handle(Standard_Transient)& data)
{
  static const char* group = "cmdMisc";

  // Add commands
  // ...

  //-------------------------------------------------------------------------//
  interp->AddCommand("box",
    //
    "box \n"
    "\t No arguments are expected. This command is a kind of 'hello world' \n"
    "\t just to draw something on the screen.",
    //
    __FILE__, group, MISC_Box);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-hexagon-bops",
    //
    "test-hexagon-bops \n"
    "\t No arguments are expected. This command demonstrates solid Booleans \n"
    "\t of OpenCascade. It cuts many cylinders from a hexagonal prism.",
    //
    __FILE__, group, MISC_TestHexagonBops);

  //-------------------------------------------------------------------------//
  interp->AddCommand("push-pull",
    //
    "push-pull faceId offset \n"
    "\t Push/pull operation.",
    //
    __FILE__, group, MISC_PushPull);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-extend",
    //
    "test-extend dim dir offset \n"
    "\t Shape extension operation. The following parameters are accepted: \n"
    "\t - dim: dimension to change (\"X\", \"Y\" or \"Z\"); \n"
    "\t - dir: in which direction to move (\"up\", \"down\" or \"both\"); \n"
    "\t - offset: offset to apply (can be positive or negative).",
    //
    __FILE__, group, MISC_TestExtend);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdMisc)
