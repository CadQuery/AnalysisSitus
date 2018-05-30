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

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiAlgo includes
#include <asiAlgo_EulerKEF.h>
#include <asiAlgo_EulerKEV.h>
#include <asiAlgo_TopoKill.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepOffset.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_MakeSimpleOffset.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

int ENGINE_OffsetShell(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc > 7 || argc < 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get part shape
  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode()->GetShape();

  // Get offset value
  const double offsetVal = atof(argv[1]);

  // Check whether topology of the result should be preserved
  bool isSimple = false, isSolid = false, isKeep = false;
  if ( argc > 2 )
  {
    for ( int k = 2; k < argc; ++k )
    {
      if ( !isSimple && interp->IsKeyword(argv[k], "simple") )
        isSimple = true;

      if ( !isSolid && interp->IsKeyword(argv[k], "solid") )
        isSolid = true;

      if ( !isKeep && interp->IsKeyword(argv[k], "keep") )
        isKeep = true;
    }
  }

  // Take tolerance for suppressing singularities
  double toler = 0.0;
  TCollection_AsciiString tolerStr;
  //
  if ( interp->GetKeyValue(argc, argv, "toler", tolerStr) )
  {
    toler = tolerStr.RealValue();

    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Passed tolerance value %1."
                                                         << toler);
  }

  // Make offset
  TopoDS_Shape offsetShape;
  if ( isSimple )
  {
    // Initialize
    BRepOffset_MakeSimpleOffset mkOffset;
    mkOffset.Initialize(partShape, offsetVal);
    mkOffset.SetBuildSolidFlag(isSolid);
    //
    if ( toler )
      mkOffset.SetTolerance(toler);

    // Perform
    mkOffset.Perform();
    // 
    if ( !mkOffset.IsDone() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Simple offset not done.");
      return TCL_OK;
    }
    offsetShape = mkOffset.GetResultShape();
  }
  else
  {
    BRepOffset_MakeOffset mkOffset;
    mkOffset.Initialize(partShape, offsetVal, 1.0e-3, BRepOffset_Skin, true, false, GeomAbs_Arc, isSolid);
    //
    if ( isSolid )
      mkOffset.MakeThickSolid();
    else
      mkOffset.MakeOffsetShape();
    //
    if ( !mkOffset.IsDone() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Offset not done.");
      return TCL_OK;
    }
    offsetShape = mkOffset.Shape();
  }

  TopoDS_Shape resultShape;

  // If this flag is enabled, the initial geometry is not substituted
  if ( isKeep )
  {
    TopoDS_Compound resComp;
    BRep_Builder().MakeCompound(resComp);
    //
    BRep_Builder().Add(resComp, partShape);
    BRep_Builder().Add(resComp, offsetShape);
    //
    resultShape = resComp;
  }
  else
    resultShape = offsetShape;

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(resultShape);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MakeEdge(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Curve Node.
  Handle(asiData_IVCurveNode)
    node = Handle(asiData_IVCurveNode)::DownCast( cmdEngine::model->FindNodeByName(argv[2]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a Curve object with name %1." << argv[2]);
    return TCL_OK;
  }

  // Get geometry of a curve.
  double f, l;
  Handle(Geom_Curve) curve = node->GetCurve(f, l);
  //
  if ( curve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Curve in question is null.");
    return TCL_OK;
  }

  // Make edge.
  TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);

  // Draw in IV.
  interp->GetPlotter().REDRAW_SHAPE(argv[1], E, Color_Red, 1.0, true);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MakeFace(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Surface Node.
  Handle(asiData_IVSurfaceNode)
    node = Handle(asiData_IVSurfaceNode)::DownCast( cmdEngine::model->FindNodeByName(argv[2]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a Surface object with name %1." << argv[2]);
    return TCL_OK;
  }

  // Get geometry of a surface.
  Handle(Geom_Surface) surface = node->GetSurface();
  //
  if ( surface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Surface in question is null.");
    return TCL_OK;
  }

  // Make face.
  TopoDS_Face F = BRepBuilderAPI_MakeFace( surface, Precision::Confusion() );

  // Draw in IV.
  interp->GetPlotter().REDRAW_SHAPE(argv[1], F, Color_White, 1.0, false);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MakeShell(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc < 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TopoDS_Shell result;
  BRep_Builder BB;
  BB.MakeShell(result);

  // Add faces to the resulting shell.
  for ( int k = 2; k < argc; ++k )
  {
    // Get Topology Item Node.
    Handle(asiData_IVTopoItemNode)
      node = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[k]) );
    //
    if ( node.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find object with name %1." << argv[k]);
      return TCL_OK;
    }

    // Get item shape.
    TopoDS_Shape itemShape = node->GetShape();
    //
    if ( itemShape.ShapeType() != TopAbs_FACE )
    {
      interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Object %1 is not face. Skipped." << argv[k]);
      continue;
    }
    //
    TopoDS_Face itemFace = TopoDS::Face(itemShape);

    // Add face to the shell being constructed.
    BB.Add(result, itemFace);
  }

  // Draw in IV.
  interp->GetPlotter().REDRAW_SHAPE(argv[1], result, Color_White, 1.0, false);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_AddSubShape(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get parent item Node.
  Handle(asiData_IVTopoItemNode)
    parentNode = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( parentNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find object with name %1." << argv[1]);
    return TCL_OK;
  }

  // Get child item Node.
  Handle(asiData_IVTopoItemNode)
    childNode = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[2]) );
  //
  if ( childNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find object with name %1." << argv[2]);
    return TCL_OK;
  }

  // Get parent shape.
  TopoDS_Shape parentShape = parentNode->GetShape();

  // Get child shape.
  TopoDS_Shape childShape = childNode->GetShape();

  // Add sub-shape.
  BRep_Builder().Add(parentShape, childShape);

  // Draw in IV.
  interp->GetPlotter().REDRAW_SHAPE(argv[1], parentShape, Color_White, 1.0, false);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MakeCurve(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node to access the selected edge.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  const TopTools_IndexedMapOfShape&
    subShapes = partNode->GetAAG()->GetMapOfSubShapes();

  // Curve Node is expected.
  Handle(asiData_CurveNode) curveNode = partNode->GetCurveRepresentation();
  //
  if ( curveNode.IsNull() || !curveNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Curve Node is null or ill-defined.");
    return TCL_OK;
  }

  // Get ID of the selected edge.
  const int edgeIdx = curveNode->GetSelectedEdge();
  //
  if ( edgeIdx <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select edge first.");
    return TCL_OK;
  }

  // Get host curve of the selected edge.
  const TopoDS_Shape& edgeShape = subShapes(edgeIdx);
  //
  if ( edgeShape.ShapeType() != TopAbs_EDGE )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected topological type of the selected edge.");
    return TCL_OK;
  }
  //
  double f, l;
  Handle(Geom_Curve) curve = BRep_Tool::Curve( TopoDS::Edge(edgeShape), f, l );

  // Set result.
  interp->GetPlotter().REDRAW_CURVE(argv[1], curve, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MakeSurf(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node to access the selected face.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  const TopTools_IndexedMapOfShape&
    subShapes = partNode->GetAAG()->GetMapOfSubShapes();

  // Surface Node is expected.
  Handle(asiData_SurfNode) surfNode = partNode->GetSurfaceRepresentation();
  //
  if ( surfNode.IsNull() || !surfNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Surface Node is null or ill-defined.");
    return TCL_OK;
  }

  // Get ID of the selected face.
  const int faceIdx = surfNode->GetSelectedFace();
  //
  if ( faceIdx <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select face first.");
    return TCL_OK;
  }

  // Get host surface of the selected face.
  const TopoDS_Shape& faceShape = subShapes(faceIdx);
  //
  if ( faceShape.ShapeType() != TopAbs_FACE )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected topological type of the selected face.");
    return TCL_OK;
  }
  //
  Handle(Geom_Surface) surf = BRep_Tool::Surface( TopoDS::Face(faceShape) );

  // Set result.
  interp->GetPlotter().REDRAW_SURFACE(argv[1], surf, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_InterpolatePoints(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv)
{
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Points Node.
  Handle(asiData_IVPointSetNode)
    pointsNode = Handle(asiData_IVPointSetNode)::DownCast( cmdEngine::model->FindNodeByName(argv[2]) );
  //
  if ( pointsNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find Points Node with name %1." << argv[2]);
    return TCL_OK;
  }

  // Get degree.
  const int degree = atoi(argv[3]);

  // Get Cartesian points to interpolate.
  Handle(asiAlgo_BaseCloud<double>) ptsCloud = pointsNode->GetPoints();

  // Interpolate.
  Handle(Geom_BSplineCurve) interpolant;
  //
  if ( !asiAlgo_Utils::InterpolatePoints(ptsCloud, degree, interpolant) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Interpolation failed.");
    return TCL_OK;
  }

  // Set result.
  interp->GetPlotter().REDRAW_CURVE(argv[1], interpolant, Color_Red);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Fuse(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get topological items which are the operands.
  Handle(asiData_IVTopoItemNode)
    topoItem1 = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[2]) );
  //
  Handle(asiData_IVTopoItemNode)
    topoItem2 = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[3]) );
  //
  if ( topoItem1.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[2]);
    return TCL_OK;
  }
  //
  if ( topoItem2.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[3]);
    return TCL_OK;
  }

  // Put all arguments to the list.
  TopTools_ListOfShape arguments;
  arguments.Append( topoItem1->GetShape() );
  arguments.Append( topoItem2->GetShape() );

  // Fuse.
  TopoDS_Shape fused = asiAlgo_Utils::BooleanFuse(arguments);
  //
  interp->GetPlotter().REDRAW_SHAPE("fused", fused, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_FuseGen(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 4 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get topological items which are the operands.
  Handle(asiData_IVTopoItemNode)
    topoItem1 = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[2]) );
  //
  Handle(asiData_IVTopoItemNode)
    topoItem2 = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[3]) );
  //
  if ( topoItem1.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[2]);
    return TCL_OK;
  }
  //
  if ( topoItem2.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[3]);
    return TCL_OK;
  }

  // Fuzzy value.
  const double fuzz = (argc == 5 ? Atof(argv[4]) : 0.0);

  // Put all arguments to the list.
  TopTools_ListOfShape arguments;
  arguments.Append( topoItem1->GetShape() );
  arguments.Append( topoItem2->GetShape() );

  // Fuse.
  TopoDS_Shape fused = asiAlgo_Utils::BooleanGeneralFuse(arguments, fuzz);
  //
  interp->GetPlotter().REDRAW_SHAPE("fused", fused, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Modeling(const Handle(asiTcl_Interp)&      interp,
                                  const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("offset-shell",
    //
    "offset-shell offset [-simple] [-solid] [-keep] [-toler <val>]\n"
    "\t Offsets part (it should be a topological shell) on the given offset\n"
    "\t value. Offsetting is performed in the direction of face normals. If the\n"
    "\t option '-simple' is passed, this operation will attempt to preserve\n"
    "\t the topology of the base shell. If the option '-solid' is passed, this\n"
    "\t operation will build a solid instead of an offset shell. If the option\n"
    "\t '-keep' is passed, the original part is not substituted with the offset\n"
    "\t shape, and the offset is added to the part. If the option '-toler' is\n"
    "\t passed and '-simple' key is used, an optional tolerance for suppressing\n"
    "\t singularities on triangular surface patches is used.",
    //
    __FILE__, group, ENGINE_OffsetShell);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-edge",
    //
    "make-edge result curveName\n"
    "\t Creates edge from a curve. The <curveName> variable should exist\n"
    "\t as a Curve object in the scene graph of imperative plotter.",
    //
    __FILE__, group, ENGINE_MakeEdge);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-face",
    //
    "make-face result surfaceName\n"
    "\t Creates face from a surface. The <surfaceName> variable should exist\n"
    "\t as a Surface object in the scene graph of imperative plotter.",
    //
    __FILE__, group, ENGINE_MakeFace);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-shell",
    //
    "make-shell result face1 [face2 [...]]\n"
    "\t Creates shell from the passed faces.",
    //
    __FILE__, group, ENGINE_MakeShell);

  //-------------------------------------------------------------------------//
  interp->AddCommand("add-subshape",
    //
    "add-subshape parent child\n"
    "\t Adds <child> to <parent>.",
    //
    __FILE__, group, ENGINE_AddSubShape);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-curve",
    //
    "make-curve curveName\n"
    "\t Creates a curve from the selected edge.",
    //
    __FILE__, group, ENGINE_MakeCurve);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-surf",
    //
    "make-surf surfName\n"
    "\t Creates a surface from the selected face.",
    //
    __FILE__, group, ENGINE_MakeSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("interpolate-points",
    //
    "interpolate-points curveName pointsName deg\n"
    "\t Creates a curve from the passed point series by interpolation.",
    //
    __FILE__, group, ENGINE_InterpolatePoints);

  //-------------------------------------------------------------------------//
  interp->AddCommand("fuse",
    //
    "fuse result op1 op2\n"
    "\t Fuses the passed two operands using Boolean Fuse operation.",
    //
    __FILE__, group, ENGINE_Fuse);

  //-------------------------------------------------------------------------//
  interp->AddCommand("fuse-gen",
    //
    "fuse-gen result op1 op2 [fuzz]\n"
    "\t Fuses the passed two operands using Boolean General Fuse operation.\n"
    "\t It is possible to affect the fusion tolerance with <fuzz> argument.",
    //
    __FILE__, group, ENGINE_FuseGen);

}
