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

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Curve.h>
#include <asiEngine_Editing.h>
#include <asiEngine_Part.h>
#include <asiEngine_TolerantShapes.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_CurvaturePlot.h>
#include <asiUI_DialogEuler.h>

#ifdef USE_MOBIUS
  #include <mobius/cascade_BSplineCurve3D.h>
  #include <mobius/cascade_BSplineSurface.h>
#endif

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//-----------------------------------------------------------------------------

int ENGINE_Explode(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  TopoDS_Shape partShape = partNode->GetShape();

  if ( argc == 1 )
  {
    // Explode
    for ( TopoDS_Iterator it(partShape); it.More(); it.Next() )
    {
      const TopoDS_Shape& subShape = it.Value();

      // Generate name
      TCollection_AsciiString name = asiAlgo_Utils::ShapeTypeStr(subShape).c_str();

      // Draw imperatively (populates Data Model)
      interp->GetPlotter().DRAW_SHAPE(subShape, name);
    }
  }
  else
  {
    // Get qualifier of sub-shape
    TopAbs_ShapeEnum subshapeType;

    if ( TCollection_AsciiString(argv[1]) == "-vertex" )
      subshapeType = TopAbs_VERTEX;
    //
    else if ( TCollection_AsciiString(argv[1]) == "-edge" )
      subshapeType = TopAbs_EDGE;
    //
    else if ( TCollection_AsciiString(argv[1]) == "-wire" )
      subshapeType = TopAbs_WIRE;
    //
    else if ( TCollection_AsciiString(argv[1]) == "-face" )
      subshapeType = TopAbs_FACE;
    //
    else if ( TCollection_AsciiString(argv[1]) == "-shell" )
      subshapeType = TopAbs_SHELL;
    //
    else if ( TCollection_AsciiString(argv[1]) == "-solid" )
      subshapeType = TopAbs_SOLID;
    //
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected sub-shape qualifier.");
      return TCL_OK;
    }

    // Explode
    for ( TopExp_Explorer exp(partShape, subshapeType); exp.More(); exp.Next() )
    {
      const TopoDS_Shape& subShape = exp.Current();

      // Generate name
      TCollection_AsciiString name = asiAlgo_Utils::ShapeTypeStr(subShape).c_str();

      // Draw imperatively (populates Data Model)
      interp->GetPlotter().DRAW_SHAPE(subShape, name);
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Summary(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }
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

  // Check naming.
  Handle(asiAlgo_Naming) naming = partNode->GetNaming();
  //
  if ( !naming.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\t... Naming initialized.");
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_FaceAddr(const Handle(asiTcl_Interp)& interp,
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
    return TCL_OK;
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get face.
  const TopoDS_Face& face = part_n->GetAAG()->GetFace(fidx);

  // Output.
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Face %1: %2"
                                                        << fidx
                                                        << asiAlgo_Utils::ShapeAddr(face).c_str() );
  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Dist(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get topological item to imprint.
  Handle(asiData_IVTopoItemNode)
    topoItem_n = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( topoItem_n.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_OK;
  }

  TIMER_NEW
  TIMER_GO

  BRepExtrema_DistShapeShape distSS(part_n->GetShape(),
                                    topoItem_n->GetShape(),
                                    Extrema_ExtFlag_MIN);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress().Access(), "Shape-shape distance")

  if ( !distSS.IsDone() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Extrema not done.");
    return TCL_OK;
  }

  // Get number of solutions.
  const int numSol = distSS.NbSolution();
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of extrema solutions: %1." << numSol);

  for ( int k = 1; k <= numSol; ++k )
  {
    const gp_Pnt& P1 = distSS.PointOnShape1(k);
    const gp_Pnt& P2 = distSS.PointOnShape2(k);

    interp->GetPlotter().DRAW_POINT(P1, Color_Red, "dist_P1");
    interp->GetPlotter().DRAW_POINT(P2, Color_Red, "dist_P2");
    interp->GetPlotter().DRAW_LINK(P1, P2, Color_Red, "dist_P1P2");
  }

  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Distance: %1."
                                                        << distSS.Value() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckCurvature(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ================
   *  Prepare inputs
   * ================ */

  // Find control keys.
  bool noPlot           = false;
  bool noAlongCurvature = false;
  //
  for ( int i = 1; i < argc; ++i )
  {
    if ( interp->IsKeyword(argv[i], "noplot") )
      noPlot = true;

    if ( interp->IsKeyword(argv[i], "noalong") )
      noAlongCurvature = true;
  }

  // Get numerical values.
  int    numPts      = 100;
  double scaleFactor = 1.0;
  double amplFactor  = 1.0;
  //
  if ( argc >= 2 )
    numPts = atoi(argv[1]);
  if ( argc >= 3 )
    scaleFactor = atof(argv[2]);
  if ( argc >= 4 )
    amplFactor = atof(argv[3]);

  // Get Part Node to access the selected edge.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape                      partShape = partNode->GetShape();
  const TopTools_IndexedMapOfShape& subShapes = partNode->GetAAG()->GetMapOfSubShapes();

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

  /* ==========================
   *  Evaluate curvature combs
   * ========================== */

  // Create curvature combs.
  asiEngine_Curve CurveAPI( cmdEngine::model, interp->GetProgress(), interp->GetPlotter() );
  //
  Handle(asiData_CurvatureCombsNode) combsNode;
  //
  std::vector<gp_Pnt> points;
  std::vector<double> params;
  std::vector<double> curvatures;
  std::vector<gp_Vec> combs;
  std::vector<bool>   combsOk;
  //
  cmdEngine::model->OpenCommand();
  {
    // Calculate curvature field.
    if ( !asiAlgo_Utils::CalculateCurvatureCombs(curve,
                                                 f,
                                                 l,
                                                 numPts,
                                                 amplFactor,
                                                 points,
                                                 params,
                                                 curvatures,
                                                 combs,
                                                 combsOk) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot calculate curvature field.");
      return TCL_OK;
    }

    // Create persistent object.
    combsNode = CurveAPI.CreateOrUpdateCurvatureCombs(curveNode,
                                                      scaleFactor,
                                                      points,
                                                      combsOk,
                                                      params,
                                                      curvatures,
                                                      combs);
  }
  cmdEngine::model->CommitCommand();

  // Actualize.
  cmdEngine::cf->ObjectBrowser->Populate();
  cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(combsNode);

  /* ======================
   *  Build curvature plot
   * ====================== */

  if ( !noPlot )
  {
    // Open curvature plot.
    asiUI_CurvaturePlot*
      cPlot = new asiUI_CurvaturePlot( interp->GetProgress(),
                                       interp->GetPlotter() );
    cPlot->Render(params, curvatures);
  }

  /* ========================================
   *  Check along-curvature (if faces exist)
   * ======================================== */

  if ( !noAlongCurvature )
  {
    // Get owner faces.
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(partShape, TopAbs_EDGE, TopAbs_FACE, M);
    //
    const TopTools_ListOfShape& edgeFaces = M.FindFromKey(edgeShape);
    //
    for ( TopTools_ListIteratorOfListOfShape fit(edgeFaces); fit.More(); fit.Next() )
    {
      const TopoDS_Face& faceShape = TopoDS::Face( fit.Value() );

      // Evaluate along curvature.
      double k;
      if ( !asiAlgo_Utils::EvaluateAlongCurvature(faceShape, TopoDS::Edge(edgeShape), k) )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot evaluate along-curvature.");
        continue;
      }
      else
      {
        interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Along curvature: %1." << k);

        if ( Abs(k) < 1.e-5 )
          interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Curvature radius is infinite." );
        else
          interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Curvature radius: %1." << Abs(1.0 / k) );
      }
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckContinuity(const Handle(asiTcl_Interp)& interp,
                           int                          argc,
                           const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ================
   *  Prepare inputs
   * ================ */

  // Get Part Node to access the selected face.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape                      partShape = partNode->GetShape();
  const TopTools_IndexedMapOfShape& subShapes = partNode->GetAAG()->GetMapOfSubShapes();

  // Surf Node is expected.
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
  Handle(Geom_Surface)
    surface = BRep_Tool::Surface( TopoDS::Face(faceShape) );

  // Only B-surfaces are allowed.
  Handle(Geom_BSplineSurface)
    bsurf = Handle(Geom_BSplineSurface)::DownCast(surface);
  //
  if ( bsurf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Only B-surfaces are allowed in this function.");
    return TCL_OK;
  }

  /* =====================
   *  Evaluate continuity
   * ===================== */

  const TColStd_Array1OfReal&    uKnots = bsurf->UKnots();
  const TColStd_Array1OfReal&    vKnots = bsurf->VKnots();
  const TColStd_Array1OfInteger& uMults = bsurf->UMultiplicities();
  const TColStd_Array1OfInteger& vMults = bsurf->VMultiplicities();

  // Draw U defects.
  for ( int i = 1; i <= uKnots.Length(); ++i )
  {
    const double u     = uKnots(i);
    const int    uMult = uMults(i);

    if ( uMult > 1 )
    {
      Handle(Geom_Curve) iso = bsurf->UIso(u);

      TCollection_AsciiString isoName("mult=");
      isoName += uMult;
      isoName += " u=";
      isoName += u;

      interp->GetPlotter().REDRAW_CURVE(isoName, iso, Color_Red);
    }
  }

  // Draw V defects.
  for ( int i = 1; i <= vKnots.Length(); ++i )
  {
    const double v     = vKnots(i);
    const int    vMult = vMults(i);

    if ( vMult > 1 )
    {
      Handle(Geom_Curve) iso = bsurf->VIso(v);

      TCollection_AsciiString isoName("mult=");
      isoName += vMult;
      isoName += " v=";
      isoName += v;

      interp->GetPlotter().REDRAW_CURVE(isoName, iso, Color_Red);
    }
  }

  // TODO

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckEuler(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  if ( argc == 1 )
  {
    asiUI_DialogEuler* pEuler = new asiUI_DialogEuler( cmdEngine::model,
                                                       interp->GetProgress() );
    //
    pEuler->show();
  }
  else
  {
    const int genus = atoi(argv[1]);

    // Calculate the Euler-Poincare property for the active part.
    asiEngine_Editing( cmdEngine::model,
                       interp->GetProgress(),
                       interp->GetPlotter() ).CheckEulerPoincare(genus);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_DumpTopograph(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Access or create topograph.
  Handle(asiAlgo_TopoGraph) topograph;
  //
  if ( part_n->HasNaming() )
    topograph = part_n->GetNaming()->GetTopoGraph(); // With names.
  else
    topograph = new asiAlgo_TopoGraph( part_n->GetShape() ); // Without names.

  // Dump to buffer.
  std::stringstream buff;
  topograph->Dump(buff);

  // Dump to file.
  std::ofstream FILE(argv[1]);
  if ( !FILE.is_open() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot open file for writing.");
    return TCL_OK;
  }
  //
  FILE << buff.str().c_str();
  //
  FILE.close();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EvalCurve(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc < 4 || argc > 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Curve Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

  // Get curve.
  double f, l;
  Handle(Geom_Curve) occtCurve = curveNode->GetCurve(f, l);
  //
  if ( occtCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is NULL.");
    return TCL_OK;
  }

  // Get parameter value.
  const double u = atof(argv[2]);

  // Get order.
  const int order = atoi(argv[3]);
  //
  if ( order < 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Negative order is meaningless.");
    return TCL_OK;
  }

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  // Evaluate curve.
  TCollection_AsciiString Message("\n========================================");
  Message                      += "\n Curve evaluation";
  Message                      += "\n----------------------------------------";
  //
  if ( !isMobius )
  {
    Message += "\nMethod: OpenCascade";

    if ( order == 0 )
    {
      gp_Pnt eval_P;
      occtCurve->D0(u, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
    }
    else if ( order == 1 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1;
      occtCurve->D1(u, eval_P, eval_D1);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1", eval_P, eval_D1, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += eval_D1.X();
      Message += ", ";
      Message += eval_D1.Y();
      Message += ", ";
      Message += eval_D1.Z();
      Message += ")";
    }
    else if ( order == 2 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1, eval_D2;
      occtCurve->D2(u, eval_P, eval_D1, eval_D2);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1", eval_P, eval_D1, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2", eval_P, eval_D2, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += eval_D1.X();
      Message += ", ";
      Message += eval_D1.Y();
      Message += ", ";
      Message += eval_D1.Z();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += eval_D2.X();
      Message += ", ";
      Message += eval_D2.Y();
      Message += ", ";
      Message += eval_D2.Z();
      Message += ")";
    }
    else if ( order == 3 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1, eval_D2, eval_D3;
      occtCurve->D3(u, eval_P, eval_D1, eval_D2, eval_D3);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1", eval_P, eval_D1, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2", eval_P, eval_D2, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D3", eval_P, eval_D3, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += eval_D1.X();
      Message += ", ";
      Message += eval_D1.Y();
      Message += ", ";
      Message += eval_D1.Z();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += eval_D2.X();
      Message += ", ";
      Message += eval_D2.Y();
      Message += ", ";
      Message += eval_D2.Z();
      Message += ")";
      //
      Message += "\nD3(X, Y, Z): (";
      Message += eval_D3.X();
      Message += ", ";
      Message += eval_D3.Y();
      Message += ", ";
      Message += eval_D3.Z();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for OpenCascade method."
                                                          << order);
      return TCL_OK;
    }
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else
    Message += "\nMethod: Mobius";

    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(occtCurve);
    //
    if ( occtBCurve.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is not a B-curve.");
      return TCL_OK;
    }

    // Convert to Mobius curve.
    mobius::cascade_BSplineCurve3D converter(occtBCurve);
    converter.DirectConvert();
    //
    const mobius::Ptr<mobius::bcurve>&
      mobCurve = converter.GetMobiusCurve();

    // Evaluate.
    if ( order == 0 )
    {
      mobius::xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
    }
    else if ( order == 1 )
    {
      mobius::xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      mobius::xyz eval_D1;
      mobCurve->Eval_Dk(u, 1, eval_D1);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D1.X(), eval_D1.Y(), eval_D1.Z() ),
                                            Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += eval_D1.X();
      Message += ", ";
      Message += eval_D1.Y();
      Message += ", ";
      Message += eval_D1.Z();
      Message += ")";
    }
    else if ( order == 2 )
    {
      mobius::xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      mobius::xyz eval_D1;
      mobCurve->Eval_Dk(u, 1, eval_D1);
      //
      mobius::xyz eval_D2;
      mobCurve->Eval_Dk(u, 2, eval_D2);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D1.X(), eval_D1.Y(), eval_D1.Z() ),
                                            Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D2.X(), eval_D2.Y(), eval_D2.Z() ),
                                            Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += eval_D1.X();
      Message += ", ";
      Message += eval_D1.Y();
      Message += ", ";
      Message += eval_D1.Z();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += eval_D2.X();
      Message += ", ";
      Message += eval_D2.Y();
      Message += ", ";
      Message += eval_D2.Z();
      Message += ")";
    }
    else if ( order == 3 )
    {
      mobius::xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      mobius::xyz eval_D1;
      mobCurve->Eval_Dk(u, 1, eval_D1);
      //
      mobius::xyz eval_D2;
      mobCurve->Eval_Dk(u, 2, eval_D2);
      //
      mobius::xyz eval_D3;
      mobCurve->Eval_Dk(u, 3, eval_D3);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D1.X(), eval_D1.Y(), eval_D1.Z() ),
                                            Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D2.X(), eval_D2.Y(), eval_D2.Z() ),
                                            Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D3",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D3.X(), eval_D3.Y(), eval_D3.Z() ),
                                            Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += eval_D1.X();
      Message += ", ";
      Message += eval_D1.Y();
      Message += ", ";
      Message += eval_D1.Z();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += eval_D2.X();
      Message += ", ";
      Message += eval_D2.Y();
      Message += ", ";
      Message += eval_D2.Z();
      Message += ")";
      //
      Message += "\nD3(X, Y, Z): (";
      Message += eval_D3.X();
      Message += ", ";
      Message += eval_D3.Y();
      Message += ", ";
      Message += eval_D3.Z();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for Mobius method."
                                                          << order);
      return TCL_OK;
    }
#endif
  }
  Message += "\n----------------------------------------";
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << Message);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EvalSurf(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc < 5 || argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is NULL.");
    return TCL_OK;
  }

  // Get parameter values.
  const double u = atof(argv[2]);
  const double v = atof(argv[3]);

  // Get order.
  const int order = atoi(argv[4]);
  //
  if ( order < 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Negative order is meaningless.");
    return TCL_OK;
  }

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  // Evaluate surface.
  TCollection_AsciiString Message("\n========================================");
  Message                      += "\n Surface evaluation";
  Message                      += "\n----------------------------------------";
  //
  if ( !isMobius )
  {
    Message += "\nMethod: OpenCascade";

    if ( order == 0 )
    {
      gp_Pnt eval_P;
      occtSurface->D0(u, v, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
    }
    else if ( order == 1 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1u, eval_D1v;
      occtSurface->D1(u, v, eval_P, eval_D1u, eval_D1v);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1u", eval_P, eval_D1u, Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1v", eval_P, eval_D1v, Color_Green);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1u(X, Y, Z): (";
      Message += eval_D1u.X();
      Message += ", ";
      Message += eval_D1u.Y();
      Message += ", ";
      Message += eval_D1u.Z();
      Message += ")";
      //
      Message += "\nD1v(X, Y, Z): (";
      Message += eval_D1v.X();
      Message += ", ";
      Message += eval_D1v.Y();
      Message += ", ";
      Message += eval_D1v.Z();
      Message += ")";
    }
    else if ( order == 2 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1u, eval_D1v, eval_D2u, eval_D2v, eval_D2uv;
      occtSurface->D2(u, v, eval_P, eval_D1u, eval_D1v, eval_D2u, eval_D2v, eval_D2uv);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1u", eval_P, eval_D1u, Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1v", eval_P, eval_D1v, Color_Green);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2u", eval_P, eval_D2u, Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2v", eval_P, eval_D2v, Color_Green);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2uv", eval_P, eval_D2uv, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
      //
      Message += "\nD1u(X, Y, Z): (";
      Message += eval_D1u.X();
      Message += ", ";
      Message += eval_D1u.Y();
      Message += ", ";
      Message += eval_D1u.Z();
      Message += ")";
      //
      Message += "\nD1v(X, Y, Z): (";
      Message += eval_D1v.X();
      Message += ", ";
      Message += eval_D1v.Y();
      Message += ", ";
      Message += eval_D1v.Z();
      Message += ")";
      //
      Message += "\nD2u(X, Y, Z): (";
      Message += eval_D2u.X();
      Message += ", ";
      Message += eval_D2u.Y();
      Message += ", ";
      Message += eval_D2u.Z();
      Message += ")";
      //
      Message += "\nD2v(X, Y, Z): (";
      Message += eval_D2v.X();
      Message += ", ";
      Message += eval_D2v.Y();
      Message += ", ";
      Message += eval_D2v.Z();
      Message += ")";
      //
      Message += "\nD2uv(X, Y, Z): (";
      Message += eval_D2uv.X();
      Message += ", ";
      Message += eval_D2uv.Y();
      Message += ", ";
      Message += eval_D2uv.Z();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for OpenCascade method."
                                                          << order);
      return TCL_OK;
    }
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else
    Message += "\nMethod: Mobius";

    Handle(Geom_BSplineSurface)
      occtBSurface = Handle(Geom_BSplineSurface)::DownCast(occtSurface);
    //
    if ( occtBSurface.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is not a B-surface.");
      return TCL_OK;
    }

    // Convert to Mobius surface.
    mobius::cascade_BSplineSurface converter(occtBSurface);
    converter.DirectConvert();
    //
    const mobius::Ptr<mobius::bsurf>&
      mobSurface = converter.GetMobiusSurface();

    // Evaluate.
    if ( order == 0 )
    {
      mobius::xyz eval_P;
      mobSurface->Eval(u, v, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += eval_P.X();
      Message += ", ";
      Message += eval_P.Y();
      Message += ", ";
      Message += eval_P.Z();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for Mobius method."
                                                          << order);
      return TCL_OK;
    }
#endif
  }
  Message += "\n----------------------------------------";
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << Message);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckToler(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int numRanges = ((argc == 2) ? atoi(argv[1]) : 10);

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Attempt to get the highlighted sub-shapes. If no sub-shapes are selected,
  // we work on the full part.
  TopTools_IndexedMapOfShape selectedSubShapes;
  //
  if ( cmdEngine::cf->ViewerPart )
  {
    asiEngine_Part PartAPI( M,
                            cmdEngine::cf->ViewerPart->PrsMgr(),
                            interp->GetProgress(),
                            interp->GetPlotter() );
    //
    PartAPI.GetHighlightedSubShapes(selectedSubShapes);
  }

  // Choose shape to check.
  TopoDS_Shape shape2Check;
  //
  if ( selectedSubShapes.Extent() )
  {
    TopoDS_Compound comp;
    BRep_Builder().MakeCompound(comp);
    //
    for ( int k = 1; k <= selectedSubShapes.Extent(); ++k )
      BRep_Builder().Add( comp, selectedSubShapes(k) );

    shape2Check = comp;
  }
  else
  {
    shape2Check = M->GetPartNode()->GetShape();
  }

  // Prepare API to analyze tolerances.
  asiEngine_TolerantShapes TolInfo( M,
                                    cmdEngine::cf->ViewerPart->PrsMgr(),
                                    interp->GetProgress(),
                                    interp->GetPlotter() );

  // Perform tolerance analysis.
  M->OpenCommand();
  {
    TolInfo.Clean_All();
    TolInfo.Populate(shape2Check, numRanges);
  }
  M->CommitCommand();

  // Update UI.
  cmdEngine::cf->ObjectBrowser->Populate();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckLength(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedEdgeIds;
  //
  if ( cmdEngine::cf->ViewerPart )
  {
    asiEngine_Part PartAPI( M,
                            cmdEngine::cf->ViewerPart->PrsMgr(),
                            interp->GetProgress(),
                            interp->GetPlotter() );
    //
    PartAPI.GetHighlightedEdges(selectedEdgeIds);
  }

  // Get total length.
  double len = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(selectedEdgeIds); eit.More(); eit.Next() )
  {
    const int edgeId = eit.Key();

    // Get edge.
    const TopoDS_Shape&
      edge = M->GetPartNode()->GetAAG()->GetMapOfEdges()(edgeId);

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::LinearProperties(edge, props);
    len += props.Mass();
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Length: %1." << len);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckArea(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedFaceIds;
  //
  if ( cmdEngine::cf->ViewerPart )
  {
    asiEngine_Part PartAPI( M,
                            cmdEngine::cf->ViewerPart->PrsMgr(),
                            interp->GetProgress(),
                            interp->GetPlotter() );
    //
    PartAPI.GetHighlightedFaces(selectedFaceIds);
  }

  // Get total area.
  double area = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selectedFaceIds); fit.More(); fit.Next() )
  {
    const int faceId = fit.Key();

    // Get face.
    const TopoDS_Shape&
      face = M->GetPartNode()->GetAAG()->GetMapOfFaces()(faceId);

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::SurfaceProperties(face, props);
    area += props.Mass();
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Area: %1." << area);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Inspection(const Handle(asiTcl_Interp)&      interp,
                                    const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("explode",
    //
    "explode [-vertex|-edge|-wire|-face|-shell|-solid]\n"
    "\t Explodes active part to sub-shapes of interest. If no sub-shape\n"
    "\t qualifier is passed, this command explodes the part to its direct\n"
    "\t children (e.g. edges for wire, wires for face, etc.).",
    //
    __FILE__, group, ENGINE_Explode);

  //-------------------------------------------------------------------------//
  interp->AddCommand("summary",
    //
    "summary\n"
    "\t Shows summary (number of sub-shapes) for the active part.",
    //
    __FILE__, group, ENGINE_Summary);

  //-------------------------------------------------------------------------//
  interp->AddCommand("face-addr",
    //
    "face-addr faceIndex\n"
    "\t Prints physical address of the given face.",
    //
    __FILE__, group, ENGINE_FaceAddr);

  //-------------------------------------------------------------------------//
  interp->AddCommand("dist",
    //
    "dist varName\n"
    "\t Computes distance between the part and the given topological object.",
    //
    __FILE__, group, ENGINE_Dist);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-curvature",
    //
    "check-curvature [numPts [scaleFactor [curvAmpl]]] [-noplot] [-noalong]\n"
    "\t Checks curvature of the selected edge. As a result, curvature combs\n"
    "\t are visualized in 3D. You can control its scale factor with\n"
    "\t <scaleFactor> argument and also its density with <numPts> argument.\n"
    "\t To bring out the salient features of the comb, <curvAmpl> amplification\n"
    "\t factor can be used. If -noplot key is passed, the curvature plot is not\n"
    "\t constructed. If -noalong key is passed, the along-curvature value for\n"
    "\t the selected edges is not computed.",
    //
    __FILE__, group, ENGINE_CheckCurvature);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-continuity",
    //
    "check-continuity\n"
    "\t Checks continuity of the selected face.",
    //
    __FILE__, group, ENGINE_CheckContinuity);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-euler",
    //
    "check-euler [genus]\n"
    "\t Opens dialog to check Euler-Poincare property of the Part geometry.\n"
    "\t If <genus> parameter is not specified, this command will open a prompt\n"
    "\t dialog to ask the user to type genus.",
    //
    __FILE__, group, ENGINE_CheckEuler);

  //-------------------------------------------------------------------------//
  interp->AddCommand("dump-topograph",
    //
    "dump-topograph filename\n"
    "\t Dumps topology graph to file.",
    //
    __FILE__, group, ENGINE_DumpTopograph);

  //-------------------------------------------------------------------------//
  interp->AddCommand("eval-curve",
    //
    "eval-curve curveName u order [-mobius]\n"
    "\t Evaluates curve <curveName> for the given parameter value <u>.\n"
    "\t If <-mobius> keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <order> specifies the order of derivatives\n"
    "\t to evaluate (0 for value evaluation).",
    //
    __FILE__, group, ENGINE_EvalCurve);

  //-------------------------------------------------------------------------//
  interp->AddCommand("eval-surf",
    //
    "eval-surf surfName u v order [-mobius]\n"
    "\t Evaluates surface <surfName> for the given parameter pair <u, v>.\n"
    "\t If <-mobius> keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <order> specifies the order of derivatives\n"
    "\t to evaluate (0 for value evaluation).",
    //
    __FILE__, group, ENGINE_EvalSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-toler",
    //
    "check-toler [numRanges]\n"
    "\t Checks local tolerances in the part shape and distributes sub-shapes\n."
    "\t by tolerance ranges. You can control the fineness of tolerance ranges\n"
    "\t using <numRanges> optional argument.",
    //
    __FILE__, group, ENGINE_CheckToler);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-length",
    //
    "check-length\n"
    "\t Checks length of the selected edges.",
    //
    __FILE__, group, ENGINE_CheckLength);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-area",
    //
    "check-area\n"
    "\t Checks area of the selected faces.",
    //
    __FILE__, group, ENGINE_CheckArea);
}
