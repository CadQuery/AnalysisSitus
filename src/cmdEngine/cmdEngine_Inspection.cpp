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

// asiAlgo includes
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_CurvaturePlot.h>

// OCCT includes
#include <BRepExtrema_DistShapeShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

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

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get face
  const TopoDS_Face& face = part_n->GetAAG()->GetFace(fidx);

  // Output
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
  if ( argc != 1 && argc != 2 && argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  double scaleFactor = 1.0;
  int numPts = 100;
  //
  if ( argc >= 2 )
    numPts = atoi(argv[1]);
  if ( argc >= 3 )
    scaleFactor = atof(argv[2]);

  // Get Part Node to access the selected edge.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
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
                                                      params,
                                                      curvatures,
                                                      combs);
  }
  cmdEngine::model->CommitCommand();

  // Actualize.
  cmdEngine::cf->ObjectBrowser->Populate();
  cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(combsNode);

  // Open curvature plot.
  asiUI_CurvaturePlot*
    cPlot = new asiUI_CurvaturePlot( interp->GetProgress(),
                                     interp->GetPlotter() );
  cPlot->Render(params, curvatures);

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
    "\t Explodes active part to sub-shapes of interest. If no sub-shape \n"
    "\t qualifier is passed, this command explodes the part to its direct \n"
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
    "face-addr faceIndex \n"
    "\t Prints physical address of the given face.",
    //
    __FILE__, group, ENGINE_FaceAddr);

  //-------------------------------------------------------------------------//
  interp->AddCommand("dist",
    //
    "dist varName \n"
    "\t Computes distance between the part and the given topological object.",
    //
    __FILE__, group, ENGINE_Dist);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-curvature",
    //
    "check-curvature [numPts [scaleFactor]] \n"
    "\t Checks curvature of the selected edge. As a result, curvature combs \n"
    "\t plot is visualized in 3D. You can control its scale factor with \n"
    "\t <scaleFactor> argument and also its density with <numPts> argument.",
    //
    __FILE__, group, ENGINE_CheckCurvature);
}
