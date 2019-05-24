//-----------------------------------------------------------------------------
// Created on: 26 December 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

// cmdRE includes
#include <cmdRE.h>

// asiAlgo includes
#include <asiAlgo_MeshInterPlane.h>
#include <asiAlgo_PlaneOnPoints.h>
#include <asiAlgo_PlateOnEdges.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_IV.h>
#include <asiEngine_RE.h>
#include <asiEngine_Triangulation.h>

#if defined USE_MOBIUS
  // Mobius includes
  #include <mobius/cascade.h>
  #include <mobius/geom_BuildAveragePlane.h>
  #include <mobius/geom_InterpolateMultiCurve.h>
  #include <mobius/geom_FairBCurve.h>
  #include <mobius/geom_SkinSurface.h>

  using namespace mobius;
#endif

// OCCT includes
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GeomAdaptor_Curve.hxx>

//-----------------------------------------------------------------------------

#if defined USE_MOBIUS

Handle(Geom_BSplineCurve) FairCurve(const Handle(Geom_BSplineCurve)& curve,
                                    const double                     lambda,
                                    ActAPI_ProgressEntry             progress)
{
  // Convert to Mobius curve.
  ptr<bcurve> mobCurve = cascade::GetMobiusBCurve(curve);

  // Perform fairing from Mobius.
  geom_FairBCurve fairing(mobCurve, lambda, NULL, NULL);
  //
  if ( !fairing.Perform() )
  {
    progress.SendLogMessage(LogErr(Normal) << "Fairing failed.");
    return TCL_OK;
  }

  // Get the faired curve.
  const ptr<bcurve>& mobResult = fairing.GetResult();

  // Convert to OpenCascade curve.
  Handle(Geom_BSplineCurve) result = cascade::GetOpenCascadeBCurve(mobResult);

  return result;
}

#endif

//-----------------------------------------------------------------------------

int RE_BuildPatches(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  cmdRE_NotUsed(argc);
  cmdRE_NotUsed(argv);

  //const bool allPatches = (argc == 1);

  const bool isPlate = interp->HasKeyword(argc, argv, "plate");

  // Get surface fairing coefficient.
  double fairCoeff = 0.;
  TCollection_AsciiString fairCoeffStr;
  //
  if ( interp->GetKeyValue(argc, argv, "fair", fairCoeffStr) )
    fairCoeff = fairCoeffStr.RealValue();

  asiEngine_RE reApi( cmdRE::model,
                      interp->GetProgress(),
                      interp->GetPlotter() );

  // Find Patches Node.
  Handle(asiData_RePatchesNode) patchesNode = reApi.Get_Patches();
  //
  if ( patchesNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "No patches are available.");
    return TCL_ERROR;
  }

  /* =============================
   *  Collect patches of interest
   * ============================= */

  std::vector<Handle(asiData_RePatchNode)> patchNodes;

  if ( argc == 1 )
  {
    /* Collect all patches */

    for ( Handle(ActAPI_IChildIterator) eit = patchesNode->GetChildIterator(); eit->More(); eit->Next() )
    {
      Handle(asiData_RePatchNode)
        patchNode = Handle(asiData_RePatchNode)::DownCast( eit->Value() );
      //
      patchNodes.push_back(patchNode);
    }
  }
  else
  {
    /* Collect patches by names */

    for ( int k = 1; k < argc; ++k )
    {
      // Skip keywords.
      TCollection_AsciiString arg(argv[k]);
      if ( arg.IsRealValue() || arg == "fair" )
        continue;

      Handle(asiData_RePatchNode)
        patchNode = Handle(asiData_RePatchNode)::DownCast( cmdRE::model->FindNodeByName(arg) );
      //
      if ( patchNode.IsNull() || !patchNode->IsWellFormed() )
      {
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Object with name '%1' is not a patch."
                                                             << arg);
        continue;
      }
      //
      patchNodes.push_back(patchNode);
    }
  }

  /* ================
   *  Build surfaces
   * ================ */

  cmdRE::model->OpenCommand();

  // Reconstruct every patch individually.
  for ( size_t k = 0; k < patchNodes.size(); ++k )
  {
    const Handle(asiData_RePatchNode)& patchNode = patchNodes[k];

    interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Next patch: '%1'."
                                                          << patchNode->GetName() );

    std::vector<Handle(Geom_BSplineCurve)> curves;

    // Get all edges and approximate them.
    for ( Handle(ActAPI_IChildIterator) cit = patchNode->GetChildIterator();
          cit->More(); cit->Next() )
    {
      Handle(ActAPI_INode) childNode = cit->Value();
      //
      if ( !childNode->IsKind( STANDARD_TYPE(asiData_ReCoEdgeNode) ) )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected type of child Node.");
        //
        cmdRE::model->AbortCommand();
        return TCL_ERROR;
      }
      //
      const Handle(asiData_ReCoEdgeNode)&
        coedgeNode = Handle(asiData_ReCoEdgeNode)::DownCast(childNode);
      //
      Handle(asiData_ReEdgeNode) edgeNode = coedgeNode->GetEdge();

      // Dump coedge/edge.
      interp->GetProgress().SendLogMessage( LogInfo(Normal) <<  "\t CoEdge: %1; Orientation: %2; References edge: %3."
                                                            <<  coedgeNode->GetId()
                                                            << (coedgeNode->IsSameSense() ? "forward" : "reversed")
                                                            <<  edgeNode->GetId() );

      // Get B-curve from edge.
      Handle(Geom_BSplineCurve)
        bcurve = Handle(Geom_BSplineCurve)::DownCast( edgeNode->GetCurve() );
      //
      if ( bcurve.IsNull() )
      {
        interp->GetProgress().SendLogMessage( LogErr(Normal) << "There is no B-curve ready in edge %1."
                                                             << edgeNode->GetId() );
        cmdRE::model->AbortCommand();
        return TCL_ERROR;
      }

      // Add curve to the collection for filling.
      curves.push_back(bcurve);

      // Update scene.
      cmdRE::cf->ViewerPart->PrsMgr()->Actualize(edgeNode);
    }

    // Build patch.
    TopoDS_Face                 patchFace;
    Handle(Geom_BSplineSurface) patchSurf;
    //
    if ( isPlate || curves.size() != 4 )
    {
      asiAlgo_PlateOnEdges plateOnEdges( interp->GetProgress(),
                                         interp->GetPlotter() );

      if ( fairCoeff )
        plateOnEdges.SetFairingCoeff(fairCoeff);

      // Build collection of edges as required by the plate construction
      // algorithm. Those edges are artificial: natural extremities are used.
      Handle(TopTools_HSequenceOfShape) edges = new TopTools_HSequenceOfShape;
      //
      for ( size_t c = 0; c < curves.size(); ++c )
        edges->Append( BRepBuilderAPI_MakeEdge(curves[c]) );

      // Build patch.
      if ( !plateOnEdges.Build(edges, 0, patchSurf, patchFace) )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot approximate trimmed surface.");
        //
        cmdRE::model->AbortCommand();
        return TCL_ERROR;
      }
    }
    else if ( !asiAlgo_Utils::Fill4Contour(curves, patchSurf) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot approximate surface.");
      //
      cmdRE::model->AbortCommand();
      return TCL_ERROR;
    }

    // Construct face for the naturally bounded surface.
    if ( patchFace.IsNull() )
      patchFace = BRepBuilderAPI_MakeFace( patchSurf, Precision::Confusion() );

    TCollection_AsciiString patchName("patch_"); patchName += patchNode->GetId();
    //
    interp->GetPlotter().REDRAW_SHAPE(patchName, patchFace, Color_White);
  }

  cmdRE::model->CommitCommand();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_BuildContourLines(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  asiEngine_RE reApi( cmdRE::model,
                      interp->GetProgress(),
                      interp->GetPlotter() );

  // Find Edges Node.
  Handle(asiData_ReEdgesNode) edgesNode = reApi.Get_Edges();
  //
  if ( edgesNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "No edges are available.");
    return TCL_ERROR;
  }

  /* ===========================
   *  Collect edges of interest
   * =========================== */

  std::vector<Handle(asiData_ReEdgeNode)> edgeNodes;

  if ( argc == 1 )
  {
    /* Collect all edges */

    for ( Handle(ActAPI_IChildIterator) eit = edgesNode->GetChildIterator(); eit->More(); eit->Next() )
    {
      Handle(asiData_ReEdgeNode)
        edgeNode = Handle(asiData_ReEdgeNode)::DownCast( eit->Value() );
      //
      edgeNodes.push_back(edgeNode);
    }
  }
  else
  {
    /* Collect edges by names */

    for ( int k = 1; k < argc; ++k )
    {
      Handle(asiData_ReEdgeNode)
        edgeNode = Handle(asiData_ReEdgeNode)::DownCast( cmdRE::model->FindNodeByName(argv[k]) );
      //
      if ( edgeNode.IsNull() || !edgeNode->IsWellFormed() )
      {
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Object with name '%1' is not an edge."
                                                             << argv[k]);
        continue;
      }
      //
      edgeNodes.push_back(edgeNode);
    }
  }

  /* =================================
   *  Perform data model modification
   * ================================= */

  M->OpenCommand();

  // Approximate every edge individually.
  for ( size_t k = 0; k < edgeNodes.size(); ++k )
  {
    const Handle(asiData_ReEdgeNode)& edgeNode = edgeNodes[k];

    interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Next edge: '%1'."
                                                          << edgeNode->GetName() );

    std::vector<Handle(Geom_BSplineCurve)> curves;

    // Approximate with parametric curve.
    std::vector<gp_XYZ> pts;
    edgeNode->GetPolyline(pts);
    //
    Handle(Geom_BSplineCurve) curve;
    if ( !asiAlgo_Utils::ApproximatePoints(pts, 3, 3, 0.1, curve) )
    {
      interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot approximate edge '%1'."
                                                            << edgeNode->GetName() );
      //
      M->AbortCommand();
      return TCL_ERROR;
    }
    //
    interp->GetPlotter().REDRAW_CURVE("last-curve", curve, Color_Default);

    // Update Data Model.
    edgeNode->SetCurve(curve);

    // Add curve to the collection for filling.
    curves.push_back(curve);

    // Update scene.
    cmdRE::cf->ViewerPart->PrsMgr()->Actualize(edgeNode);
  }

  M->CommitCommand();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_FairContourLines(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get fairing coefficient.
  const double lambda = atof(argv[1]);

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  asiEngine_RE reApi( cmdRE::model,
                      interp->GetProgress(),
                      interp->GetPlotter() );

  // Find Edges Node.
  Handle(asiData_ReEdgesNode) edgesNode = reApi.Get_Edges();
  //
  if ( edgesNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "No edges are available.");
    return TCL_ERROR;
  }

  M->OpenCommand();

  // Approximate every edge individually.
  for ( Handle(ActAPI_IChildIterator) eit = edgesNode->GetChildIterator(); eit->More(); eit->Next() )
  {
    Handle(asiData_ReEdgeNode)
      edgeNode = Handle(asiData_ReEdgeNode)::DownCast( eit->Value() );

    interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Next edge: %1."
                                                          << edgeNode->GetId() );

    // Get B-curve from edge.
    Handle(Geom_BSplineCurve)
      bcurve = Handle(Geom_BSplineCurve)::DownCast( edgeNode->GetCurve() );
    //
    if ( bcurve.IsNull() )
    {
      interp->GetProgress().SendLogMessage( LogErr(Normal) << "There is no B-curve ready in edge %1."
                                                           << edgeNode->GetId() );
      return TCL_ERROR;
    }

    // Fair curve.
    Handle(Geom_BSplineCurve) fairedBCurve = FairCurve( bcurve,
                                                        lambda,
                                                        interp->GetProgress() );
    //
    if ( fairedBCurve.IsNull() )
    {
      interp->GetProgress().SendLogMessage( LogErr(Normal) << "Fairing failed for edge %1."
                                                           << edgeNode->GetId() );
      return TCL_ERROR;
    }

    // Update Data Model.
    edgeNode->SetCurve(fairedBCurve);

    // Update scene.
    cmdRE::cf->ViewerPart->PrsMgr()->Actualize(edgeNode);
  }

  M->CommitCommand();

  return TCL_OK;
#else
  interp->GetProgress().SendLogMessage(LogErr(Normal) << "This feature is not available.");

  return false;
#endif
}

//-----------------------------------------------------------------------------

int RE_CutWithPlane(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 3 && argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const bool doSort = !interp->HasKeyword(argc, argv, "nosort");

  // Get Triangulaion.
  Handle(asiData_TriangulationNode) tris_n = cmdRE::model->GetTriangulationNode();
  //
  if ( tris_n.IsNull() || !tris_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mesh is not ready for cutting.");
    return TCL_ERROR;
  }
  //
  Handle(Poly_Triangulation) triangulation = tris_n->GetTriangulation();

  // Get cutting plane.
  Handle(ActAPI_INode) node = cmdRE::model->FindNodeByName(argv[2]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_ERROR;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfaceNode = Handle(asiData_IVSurfaceNode)::DownCast(node);
  //
  Handle(Geom_Plane)
    occtPlane = Handle(Geom_Plane)::DownCast( surfaceNode->GetSurface() );
  //
  if ( occtPlane.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is not a plane.");
    return TCL_ERROR;
  }

  // Intersect with plane.
  asiAlgo_MeshInterPlane algo( triangulation,
                               interp->GetProgress(),
                               interp->GetPlotter() );
  //
  if ( !algo.Perform(occtPlane, true) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to cut mesh with plane.");
    return TCL_ERROR;
  }

  // Get the result.
  interp->GetPlotter().REDRAW_POINTS(argv[1], algo.GetResult()->GetCoordsArray(),
                                     doSort ? Color_Green : Color_Red);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_ApproxPoints(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc < 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get point cloud Node.
  Handle(asiData_IVPointSetNode)
    ptsNode = Handle(asiData_IVPointSetNode)::DownCast( cmdRE::model->FindNodeByName(argv[2]) );
  //
  if ( ptsNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Data object '%1' is not a point cloud."
                                                        << argv[2]);
    return TCL_ERROR;
  }

  // Get points with reper (selected) points. The reper points (if any)
  // will be used as breakpoints to approximate separately.
  Handle(asiAlgo_BaseCloud<double>)   ptsCloud  = ptsNode->GetPoints();
  Handle(TColStd_HPackedMapOfInteger) ptsFilter = ptsNode->GetFilter();

  // Get precision.
  const double prec = atof(argv[3]);

  // Check is the curve is expected to be closed.
  const bool isClosed = interp->HasKeyword(argc, argv, "closed");
  //
  if ( isClosed && !ptsFilter.IsNull() && ptsFilter->Map().Extent() )
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "There is a filter on points. A closed curve cannot be constructed.");

  // Collect indices of breakpoints into an ordered collection. We do not
  // rely here on the order of packed map since this map is not ordered
  // in essence.
  std::vector<int> breakIndices;
  //
  if ( !ptsFilter.IsNull() )
  {
    const TColStd_PackedMapOfInteger& filter = ptsFilter->Map();
    //
    for ( TColStd_MapIteratorOfPackedMapOfInteger fit(filter); fit.More(); fit.Next() )
      breakIndices.push_back( fit.Key() );
    //
    std::sort( breakIndices.begin(), breakIndices.end() );
  }

  // Prepare a collection of point regions.
  std::vector< std::vector<gp_XYZ> > ptsRegions;

  // If no filter is set, there is only one region to approximate.
  if ( !breakIndices.size() )
  {
    std::vector<gp_XYZ> pts;
    for ( int k = 0; k < ptsCloud->GetNumberOfElements(); ++k )
      pts.push_back( ptsCloud->GetElement(k) );
    //
    ptsRegions.push_back(pts);
  }
  else
  {
    // Loop over the indices of breakpoints.
    for ( size_t b = 0; b < breakIndices.size(); ++b )
    {
      const int  b_first    = breakIndices[b];
      const int  b_next     = ( (b == breakIndices.size() - 1) ? breakIndices[0] : breakIndices[b + 1] );
      //const bool isReturing = (b_next == b_first);

      // Populate next region.
      std::vector<gp_XYZ> pts;
      int  k    = b_first;
      bool stop = false;
      //
      do
      {
        if ( pts.size() && (k == b_next) ) // Reached next breakpoint.
          stop = true;

        if ( k == ptsCloud->GetNumberOfElements() - 1 ) // Reached the end, cycling...
        {
          k = 0;
          //
          if ( pts.size() && (k == b_next) ) // Reached next breakpoint.
            stop = true;
        }

        pts.push_back( ptsCloud->GetElement(k++) );
      }
      while ( !stop );

      // Add region.
      ptsRegions.push_back(pts);
    }
  }

  // For unfiltered clouds, the curve can be closed.
  if ( (ptsRegions.size() == 1) && isClosed )
    ptsRegions[0].push_back( ptsRegions[0][0] );

  // Approximate.
  for ( size_t k = 0; k < ptsRegions.size(); ++k )
  {
    // Run approximation algorithm.
    Handle(Geom_BSplineCurve) resCurve;
    if ( !asiAlgo_Utils::ApproximatePoints(ptsRegions[k], 3, 3, prec, resCurve) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Approximation failed.");
      return TCL_ERROR;
    }

    // Set result.
    interp->GetPlotter().DRAW_CURVE(resCurve, Color_Red, argv[1]);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_SkinSurface(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc < 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Check if fairing is requested.
  bool   isFairing    = false;
  double fairingCoeff = 0.0;
  //
  for ( int k = 2; k < argc-1; ++k )
  {
    if ( interp->IsKeyword(argv[k], "fair-isos") )
    {
      isFairing    = true;
      fairingCoeff = Atof(argv[k + 1]);
      break;
    }
  }

  // Get degree in V curvilinear direction.
  const int vDegree = atoi(argv[2]);

  // Convert OCCT B-curves to Mobius B-curves.
  std::vector< ptr<bcurve> > bCurves;
  //
  for ( int k = (isFairing ? 5 : 3); k < argc; ++k )
  {
    // Get Node.
    Handle(ActAPI_INode) node = cmdRE::model->FindNodeByName(argv[k]);
    //
    if ( node.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a data object with name '%1'."
                                                          << argv[k]);
      return TCL_OK;
    }

    // Get parametric curve.
    Handle(Geom_Curve) curveBase;
    //
    if ( node->IsInstance( STANDARD_TYPE(asiData_IVCurveNode) ) )
    {
      double f, l;
      curveBase = Handle(asiData_IVCurveNode)::DownCast(node)->GetCurve(f, l);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected type of Node with ID %1."
                                                          << argv[k]);
      return TCL_OK;
    }

    // Convert curve using a dedicated Mobius utility.
    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(curveBase);
    //
    ptr<bcurve> mobiusBCurve = cascade::GetMobiusBCurve(occtBCurve);
    //
    bCurves.push_back(mobiusBCurve);

#if defined COUT_DEBUG
    // Print knot vector for information.
    const std::vector<double>& U = mobiusBCurve->Knots();
    //
    TCollection_AsciiString Ustr;
    //
    Ustr += "[";
    for ( size_t s = 0; s < U.size(); ++s )
    {
      Ustr += U[s];
      if ( s < U.size() - 1 )
        Ustr += " ";
    }
    Ustr += "]";
    //
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "... U[%1] = %2."
                                                         << argv[k] << Ustr);
#endif
  }

  // Skin surface.
  geom_SkinSurface skinner(bCurves, vDegree, true);
  //
  if ( !skinner.PrepareSections() )
  {
    interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot prepare sections (error code %1)."
                                                         << skinner.GetErrorCode() );
    return TCL_OK;
  }
  //
  if ( !skinner.BuildIsosU() )
  {
    interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot build U isos (error code %1)."
                                                         << skinner.GetErrorCode() );
    return TCL_OK;
  }

  // Fair skinning curves.
  for ( size_t k = 0; k < skinner.IsoU_Curves.size(); ++k )
  {
    // Convert Mobius curve to OpenCascade curve.
    {
      Handle(Geom_BSplineCurve)
        occtIsoU = cascade::GetOpenCascadeBCurve(skinner.IsoU_Curves[k]);

#if defined DRAW_DEBUG
      interp->GetPlotter().DRAW_CURVE(occtIsoU, Color_White, "Iso_U");
#endif
    }

    // Perform fairing if requested.
    if ( isFairing )
    {
      geom_FairBCurve fairing(skinner.IsoU_Curves[k], fairingCoeff, NULL, NULL);
      //asiAlgo_FairBCurve fairing( occtIsoU,
      //                            fairingCoeff, // Fairing coefficient.
      //                            interp->GetProgress(),
      //                            interp->GetPlotter() );
      //
      if ( !fairing.Perform() )
      {
        interp->GetProgress().SendLogMessage( LogErr(Normal) << "Fairing failed for iso-U curve %1."
                                                             << int(k) );
        return TCL_OK;
      }

      // Override U iso in the skinner.
      skinner.IsoU_Curves[k] = fairing.GetResult();

      // Convert Mobius curve to OpenCascade curve.
      {
        Handle(Geom_BSplineCurve)
          occtIsoUFaired = cascade::GetOpenCascadeBCurve(skinner.IsoU_Curves[k]);

#if defined DRAW_DEBUG
        interp->GetPlotter().DRAW_CURVE(occtIsoUFaired, Color_White, "Iso_U_faired");
#endif
      }
    }
  }

  // Build final surface.
  if ( !skinner.BuildSurface() )
  {
    interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot build interpolant surface (error code %1)."
                                                         << skinner.GetErrorCode() );
    return TCL_OK;
  }

  // Get skinned surface.
  const mobius::ptr<mobius::bsurf>& mobiusRes = skinner.GetResult();

  /* ==========================================
   *  Convert interpolant to OpenCascade shape
   * ========================================== */

  Handle(Geom_BSplineSurface)
    occtRes = cascade::GetOpenCascadeBSurface(mobiusRes);

  interp->GetPlotter().REDRAW_SURFACE(argv[1], occtRes, Color_Default);

  return TCL_OK;
#else
  interp->GetProgress().SendLogMessage(LogErr(Normal) << "This feature is not available.");

  return false;
#endif
}

//-----------------------------------------------------------------------------

int RE_InterpMulticurve(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc < 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* =====================
   *  Stage 1: get inputs
   * ===================== */

  // Get number of discretization points on each curve.
  const int numPts = atoi(argv[1]);

  // Get degree of interpolation.
  const int degree = atoi(argv[2]);

  // Get B-curves to interpolate.
  std::vector<Handle(Geom_BSplineCurve)> bCurves;
  std::vector<TCollection_AsciiString>   bCurveNames;
  //
  for ( int k = 3; k < argc; ++k )
  {
    // Get Node.
    Handle(ActAPI_INode) node = cmdRE::model->FindNodeByName(argv[k]);
    //
    if ( node.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a data object with name '%1'."
                                                          << argv[k]);
      return TCL_OK;
    }

    // Get parametric curve.
    Handle(Geom_Curve) curveBase;
    //
    if ( node->IsInstance( STANDARD_TYPE(asiData_IVCurveNode) ) )
    {
      double f, l;
      curveBase = Handle(asiData_IVCurveNode)::DownCast(node)->GetCurve(f, l);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected type of Node with ID %1."
                                                          << argv[k]);
      return TCL_OK;
    }

    // Add curve to the collection of curves for synchronous interpolation.
    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(curveBase);
    //
    if ( !occtBCurve.IsNull() )
    {
      bCurves.push_back(occtBCurve);
      bCurveNames.push_back(argv[k]);
    }
  }

  /* ==============================================================
   *  Stage 2: discretize curves to have the same number of points
   * ============================================================== */

  TIMER_NEW
  TIMER_GO

  // Prepare interpolation tool.
  geom_InterpolateMultiCurve interpTool(degree,
                                        ParamsSelection_Centripetal,
                                        KnotsSelection_Average);

  for ( size_t k = 0; k < bCurves.size(); ++k )
  {
    // Discretize with a uniform curvilinear step.
    GeomAdaptor_Curve gac(bCurves[k]);
    GCPnts_QuasiUniformAbscissa Defl(gac, numPts);
    //
    if ( !Defl.IsDone() )
      return false;

    // Fill row of points.
    std::vector<xyz> ptsRow;
    //
    for ( int i = 1; i <= numPts; ++i )
    {
      const double param = Defl.Parameter(i);
      xyz P = cascade::GetMobiusPnt( bCurves[k]->Value(param) );
      //
      ptsRow.push_back(P);
    }

    // Add points to the interpolation tool.
    interpTool.AddRow(ptsRow);
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Discretize curves")

  /* =================================
   *  Stage 3: interpolate multicurve
   * ================================= */

  TIMER_RESET
  TIMER_GO

  if ( !interpTool.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Interpolation failed.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Interpolate curves")

  for ( int k = 0; k < interpTool.GetNumRows(); ++k )
  {
    Handle(Geom_BSplineCurve)
      resCurve = cascade::GetOpenCascadeBCurve( interpTool.GetResult(k) );

    interp->GetPlotter().REDRAW_CURVE(bCurveNames[k], resCurve, Color_Default);
  }

  return TCL_OK;
#else
  interp->GetProgress().SendLogMessage(LogErr(Normal) << "This feature is not available.");

  return false;
#endif
}

//-----------------------------------------------------------------------------

int RE_JoinCurves(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc < 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get B-curves to join.
  std::vector<Handle(Geom_BSplineCurve)> bCurves;
  //
  for ( int k = 2; k < argc; ++k )
  {
    // Get Node.
    Handle(ActAPI_INode) node = cmdRE::model->FindNodeByName(argv[k]);
    //
    if ( node.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find a data object with name '%1'."
                                                          << argv[k]);
      return TCL_OK;
    }

    // Get parametric curve.
    Handle(Geom_Curve) curveBase;
    //
    if ( node->IsInstance( STANDARD_TYPE(asiData_IVCurveNode) ) )
    {
      double f, l;
      curveBase = Handle(asiData_IVCurveNode)::DownCast(node)->GetCurve(f, l);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected type of Node with ID %1."
                                                          << argv[k]);
      return TCL_OK;
    }

    // Add curve to the collection of curves for synchronous interpolation.
    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(curveBase);
    //
    if ( !occtBCurve.IsNull() )
      bCurves.push_back(occtBCurve);
  }

  // Join curves.
  Handle(Geom_BSplineCurve) jointCurve;
  //
  if ( !asiAlgo_Utils::JoinCurves( bCurves,
                                   2, // C2
                                   jointCurve,
                                   interp->GetProgress() ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot join curves.");
    return TCL_OK;
  }

  // Set result.
  interp->GetPlotter().REDRAW_CURVE(argv[1], jointCurve, Color_Default);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_CheckSurfDeviation(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc < 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get surface to analyze.
  Handle(ActAPI_INode) node = cmdRE::model->FindNodeByName(argv[2]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[2]);
    return TCL_ERROR;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfaceNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Prepare Deviation Node.
  Handle(asiData_SurfDeviationNode) sdNode;
  //
  cmdRE::model->OpenCommand();
  {
    // If there is no BVH for mesh, build one.
    if ( cmdRE::model->GetTriangulationNode()->GetBVH().IsNull() )
      asiEngine_Triangulation( cmdRE::model, interp->GetProgress(), NULL ).BuildBVH();

    // Create Surface Deviation Node.
    sdNode = asiEngine_IV(cmdRE::model, interp->GetProgress(), NULL).Create_SurfaceDeviation(argv[1], surfaceNode);
  }
  cmdRE::model->CommitCommand();

  // Update UI.
  cmdRE::cf->ObjectBrowser->Populate();
  cmdRE::cf->ViewerPart->PrsMgr()->Actualize(sdNode);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_MakeAveragePlane(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 3 && argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  // Get point cloud Node.
  Handle(asiData_IVPointSetNode)
    ptsNode = Handle(asiData_IVPointSetNode)::DownCast( cmdRE::model->FindNodeByName(argv[2]) );
  //
  if ( ptsNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Data object '%1' is not a point cloud."
                                                        << argv[2]);
    return TCL_ERROR;
  }

  // Get points.
  Handle(asiAlgo_BaseCloud<double>) ptsCloud = ptsNode->GetPoints();

  // Build average plane.
  gp_Pln resPln;
  //
  if ( isMobius )
  {
    mobius::ptr<mobius::plane> mobPlane;

    // Prepare point cloud.
    mobius::ptr<pcloud> mobPts = new mobius::pcloud( ptsCloud->GetCoords() );

    // Build average plane.
    geom_BuildAveragePlane planeAlgo;
    //
    if ( !planeAlgo.Build(mobPts, mobPlane) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build average plane.");
      return TCL_ERROR;
    }

    resPln = mobius::cascade::GetOpenCascadePlane(mobPlane)->Pln();
  }
  else
  {
    asiAlgo_PlaneOnPoints planeOnPoints( interp->GetProgress(), interp->GetPlotter() );
    //
    if ( !planeOnPoints.Build(ptsCloud, resPln) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to build average plane.");
      return TCL_ERROR;
    }
  }

  // Set the result.
  interp->GetPlotter().REDRAW_SURFACE(argv[1], new Geom_Plane(resPln), Color_Default);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdRE::Commands_Modeling(const Handle(asiTcl_Interp)&      interp,
                              const Handle(Standard_Transient)& data)
{
  cmdRE_NotUsed(data);
  //
  static const char* group = "cmdRE";

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-build-patches",
    //
    "re-build-patches [patchName1 [patchName2 ...]] [-plate] [-fair <coeff>]\n"
    "\t Constructs surface patched for the passed data object(s).",
    //
    __FILE__, group, RE_BuildPatches);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-build-contour-lines",
    //
    "re-build-contour-lines [edgeName1 [edgeName2 ...]]\n"
    "\t Constructs contour lines either for all patches or for the passed\n"
    "\t edges only.",
    //
    __FILE__, group, RE_BuildContourLines);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-fair-contour-lines",
    //
    "re-fair-contour-lines coeff\n"
    "\t Fairs (smooths) contour lines for all patches with the given fairing\n"
    "\t coefficient.",
    //
    __FILE__, group, RE_FairContourLines);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-cut-with-plane",
    //
    "re-cut-with-plane res p [-nosort]\n"
    "\t Cuts triangulation with plane. If '-nosort' key is passed, the"
    "\t resulting points are not post-processed with K-neighbors hull"
    "\t algorithm thus remaining disordered.",
    //
    __FILE__, group, RE_CutWithPlane);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-approx-points",
    //
    "re-approx-points resCurve points prec [-closed]\n"
    "\t Attempts to approximate the given point cloud with a curve.",
    //
    __FILE__, group, RE_ApproxPoints);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-skin-surface",
    //
    "re-skin-surface resSurf vDegree [-fair-isos lambda] curveName1 ... curveNameK \n"
    "\t Interpolates surface by skinning the passed B-curves.",
    //
    __FILE__, group, RE_SkinSurface);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-interp-multicurve",
    //
    "re-interp-multicurve numpts degree curveName1 [curveName2 [...]]\n"
    "\t Interpolates a set of curves on the same knot sequence.",
    //
    __FILE__, group, RE_InterpMulticurve);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-join-curves",
    //
    "re-join-curves res curveName1 curveName2 [curveName3 [...]]\n"
    "\t Joins curves into a single curve.",
    //
    __FILE__, group, RE_JoinCurves);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-check-surf-deviation",
    //
    "re-check-surf-deviation res surfName\n"
    "\t Checks deviation between the given surface and the reference mesh.",
    //
    __FILE__, group, RE_CheckSurfDeviation);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-make-average-plane",
    //
    "re-make-average-plane res pointsName [-mobius]\n"
    "\t Approximates the given point cloud with plane.",
    //
    __FILE__, group, RE_MakeAveragePlane);
}
