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

// cmdRE includes
#include <cmdRE.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_RE.h>

#if defined USE_MOBIUS
  // Mobius includes
  #include <mobius/cascade.h>
  #include <mobius/geom_FairBCurve.h>

  using namespace mobius;
#endif

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
  const bool allPatches = (argc == 1);

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

  cmdRE::model->OpenCommand();

  // Reconstruct every patch individually.
  for ( Handle(ActAPI_IChildIterator) pit = patchesNode->GetChildIterator(); pit->More(); pit->Next() )
  {
    Handle(asiData_RePatchNode)
      patchNode = Handle(asiData_RePatchNode)::DownCast( pit->Value() );

    interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Next patch: %1."
                                                          << patchNode->GetId() );

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
        return TCL_ERROR;
      }

      // Add curve to the collection for filling.
      curves.push_back(bcurve);

      // Update scene.
      cmdRE::cf->ViewerPart->PrsMgr()->Actualize(edgeNode);
    }

    // Build patch.
    Handle(Geom_BSplineSurface) patchSurf;
    //
    if ( !asiAlgo_Utils::Fill4Contour(curves, patchSurf) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot approximate surface.");
      //
      cmdRE::model->AbortCommand();
      return TCL_ERROR;
    }
    //
    TCollection_AsciiString patchName("patch_"); patchName += patchNode->GetId();
    //
    interp->GetPlotter().REDRAW_SURFACE(patchName, patchSurf, Color_White);
  }

  cmdRE::model->CommitCommand();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_BuildContourLines(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

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

    std::vector<Handle(Geom_BSplineCurve)> curves;

    // Approximate with parametric curve.
    std::vector<gp_XYZ> pts;
    edgeNode->GetPolyline(pts);
    //
    Handle(Geom_BSplineCurve) curve;
    if ( !asiAlgo_Utils::ApproximatePoints(pts, 3, 3, 0.1, curve) )
    {
      interp->GetProgress().SendLogMessage( LogErr(Normal) << "Cannot approximate edge %1."
                                                            << edgeNode->GetId() );
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

void cmdRE::Commands_Modeling(const Handle(asiTcl_Interp)&      interp,
                              const Handle(Standard_Transient)& data)
{
  cmdRE_NotUsed(data);
  //
  static const char* group = "cmdRE";

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-build-patches",
    //
    "re-build-patches [patchName1 [patchName2 ...]]\n"
    "\t Constructs surface patched for the passed data object(s).",
    //
    __FILE__, group, RE_BuildPatches);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-build-contour-lines",
    //
    "re-build-contour-lines\n"
    "\t Constructs contour lines for all patches.",
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
}
