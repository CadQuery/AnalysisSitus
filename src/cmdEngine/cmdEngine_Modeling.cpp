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

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_MakeSimpleOffset.hxx>
#include <Precision.hxx>

//-----------------------------------------------------------------------------

int ENGINE_OffsetShell(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 2 && argc != 3 && argc != 4 && argc != 5 )
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

  // Make offset
  TopoDS_Shape offsetShape;
  if ( isSimple )
  {
    BRepOffset_MakeSimpleOffset mkOffset;
    mkOffset.Initialize(partShape, offsetVal);
    mkOffset.SetBuildSolidFlag(isSolid);
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

void cmdEngine::Commands_Modeling(const Handle(asiTcl_Interp)&      interp,
                                  const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("offset-shell",
    //
    "offset-shell offset [-simple] [-solid] [-keep]\n"
    "\t Offsets part (it should be a topological shell) on the given offset \n"
    "\t value. Offsetting is performed in the direction of face normals. If the \n"
    "\t option '-simple' is passed, this operation will attempt to preserve \n"
    "\t the topology of the base shell. If the option '-solid' is passed, this \n"
    "\t operation will build a solid instead of an offset shell. If the option \n"
    "\t '-keep' is passed, the original part is not substituted with the offset \n"
    "\t shape, and the offset is added to the part.",
    //
    __FILE__, group, ENGINE_OffsetShell);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-edge",
    //
    "make-edge result curveName\n"
    "\t Creates edge from a curve. The <curveName> variable should exist \n"
    "\t as a Curve object in the scene graph of imperative plotter.",
    //
    __FILE__, group, ENGINE_MakeEdge);

  //-------------------------------------------------------------------------//
  interp->AddCommand("make-face",
    //
    "make-face result surfaceName\n"
    "\t Creates face from a surface. The <surfaceName> variable should exist \n"
    "\t as a Surface object in the scene graph of imperative plotter.",
    //
    __FILE__, group, ENGINE_MakeFace);

}
