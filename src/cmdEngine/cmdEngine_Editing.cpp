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
#include <asiAlgo_TopoKill.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_MakeSimpleOffset.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>

//-----------------------------------------------------------------------------

int ENGINE_KillVertex(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int vidx = atoi(argv[1]);
  //
  if ( vidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Vertex index should be 1-based.");
    return TCL_OK;
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get map of vertices with respect to those the passed index is relevant
  const TopTools_IndexedMapOfShape& allVertices = part_n->GetAAG()->GetMapOfVertices();

  // Prepare killer
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( part_n->HasNaming() )
    killer.SetHistory( part_n->GetNaming()->GetHistory() );
  //
  if ( !killer.AskRemove( allVertices(vidx) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of vertex %1 was rejected." << vidx);
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_KillEdge(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int eidx = atoi(argv[1]);
  //
  if ( eidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge index should be 1-based.");
    return TCL_OK;
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get map of edges with respect to those the passed index is relevant
  const TopTools_IndexedMapOfShape& allEdges = part_n->GetAAG()->GetMapOfEdges();

  // Prepare killer
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( part_n->HasNaming() )
    killer.SetHistory( part_n->GetNaming()->GetHistory() );
  //
  if ( !killer.AskRemove( allEdges(eidx) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of edge %1 was rejected." << eidx);
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_KillFace(const Handle(asiTcl_Interp)& interp,
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

  // Get map of faces with respect to those the passed index is relevant
  const TopTools_IndexedMapOfShape& allFaces = part_n->GetAAG()->GetMapOfFaces();

  // Prepare killer
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( part_n->HasNaming() )
    killer.SetHistory( part_n->GetNaming()->GetHistory() );
  //
  if ( !killer.AskRemove( allFaces(fidx) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of face %1 was rejected." << fidx);
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_KillSolidByFace(const Handle(asiTcl_Interp)& interp,
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

  // Get map of faces with respect to those the passed index is relevant
  const TopTools_IndexedMapOfShape& allFaces = part_n->GetAAG()->GetMapOfFaces();

  // Get face in question
  TopoDS_Face face = TopoDS::Face( allFaces(fidx) );

  // Get owner solid
  TopTools_IndexedDataMapOfShapeListOfShape faceOwners;
  TopExp::MapShapesAndAncestors(part_n->GetShape(), TopAbs_FACE, TopAbs_SOLID, faceOwners);
  //
  if ( faceOwners.IsEmpty() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find parent solid for face %1." << fidx);
    return TCL_OK;
  }

  // Get owner shapes
  TopTools_ListOfShape owners = faceOwners.FindFromKey(face);
  //
  if ( owners.IsEmpty() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "There are no parents for face %1. Cannot proceed." << fidx);
    return TCL_OK;
  }

  // Get solid
  TopoDS_Solid ownerSolid = TopoDS::Solid( owners.First() );
  //
  if ( ownerSolid.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Owner solid is null. Cannot proceed.");
    return TCL_OK;
  }

  // Prepare killer
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( part_n->HasNaming() )
    killer.SetHistory( part_n->GetNaming()->GetHistory() );
  //
  if ( !killer.AskRemove(ownerSolid) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of solid was rejected.");
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MoveByFace(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get face index
  const int fidx = atoi(argv[1]);
  //
  if ( fidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face index should be 1-based.");
    return TCL_OK;
  }

  // Get offset value
  const double offset = atof(argv[2]);

  // Get part
  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast(interp->GetModel())->GetPartNode()->GetShape();

  // Get face
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(partShape, TopAbs_FACE, M);
  TopoDS_Face faceShape = TopoDS::Face( M.FindKey(fidx) );

  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(faceShape, uMin, uMax, vMin, vMax);

  double uMid = (uMin + uMax)*0.5;
  double vMid = (vMin + vMax)*0.5;

  BRepLProp_SLProps lprops( BRepAdaptor_Surface(faceShape), uMid, vMid, 1, Precision::Confusion() );
  //
  if ( !lprops.IsNormalDefined() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Normal undefined.");
    return TCL_OK;
  }
  //
  gp_Dir n = lprops.Normal();
  //
  if ( faceShape.Orientation() == TopAbs_REVERSED )
    n.Reverse();

  // Prepare transformation
  gp_Trsf T;
  T.SetTranslation(gp_Vec(n)*offset);

  // Move
  TopoDS_Shape moved = BRepBuilderAPI_Transform(partShape, T, true);

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(moved);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

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

int ENGINE_Imprint(const Handle(asiTcl_Interp)& interp,
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

  // Put all arguments to the list.
  TopTools_ListOfShape arguments;
  arguments.Append( part_n->GetShape() );
  arguments.Append( topoItem_n->GetShape() );

  // Initialize General Fuse tool for imprinting.
  BRepAlgoAPI_BuilderAlgo GeneralFuse;
  //
  GeneralFuse.SetArguments(arguments);
  GeneralFuse.SetRunParallel(true);
  //
  GeneralFuse.Build();
  //
  if ( GeneralFuse.HasErrors() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Boolean operation failed.");
    return TCL_OK;
  }

  // Take the result
  const TopoDS_Shape& fused = GeneralFuse.Shape();

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(fused);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Repair(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }

  // Get part shape.
  TopoDS_Shape partShape = part_n->GetShape();
  //
  if ( partShape.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part contains no B-Rep.");
    return TCL_OK;
  }

  // Fix shape.
  ShapeFix_Shape fix( part_n->GetShape() );
  fix.Perform();
  TopoDS_Shape result = fix.Shape();

  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_SetFaceTolerance(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }

  // Get part shape.
  TopoDS_Shape partShape = part_n->GetShape();
  //
  if ( partShape.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part contains no B-Rep.");
    return TCL_OK;
  }

  const int    faceId = atoi(argv[1]); // 1-based
  const double toler  = atof(argv[2]);

  TopTools_IndexedMapOfShape allFaces;
  TopExp::MapShapes(partShape, TopAbs_FACE, allFaces);
  TopoDS_Face faceShape = TopoDS::Face( allFaces.FindKey(faceId) );

  ShapeFix_ShapeTolerance fixToler;
  fixToler.SetTolerance(faceShape, toler);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_SetTolerance(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }

  // Get part shape.
  TopoDS_Shape partShape = part_n->GetShape();
  //
  if ( partShape.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part contains no B-Rep.");
    return TCL_OK;
  }

  const double toler = atof(argv[1]);

  ShapeFix_ShapeTolerance fixToler;
  fixToler.SetTolerance(partShape, toler);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Editing(const Handle(asiTcl_Interp)&      interp,
                                 const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("kill-vertex",
    //
    "kill-vertex vertexIndex\n"
    "\t Kills vertex with the passed 1-based index from the active part. \n"
    "\t This is a pure topological operation which does not attempt to \n"
    "\t modify geometry. Moreover, unlike Euler operator, this function \n"
    "\t does not preserve the topological consistency of the CAD part. \n"
    "\t We have introduced this function to ground Euler operators on it.",
    //
    __FILE__, group, ENGINE_KillVertex);

  //-------------------------------------------------------------------------//
  interp->AddCommand("kill-edge",
    //
    "kill-edge edgeIndex\n"
    "\t Kills edge with the passed 1-based index from the active part. \n"
    "\t This is a pure topological operation which does not attempt to \n"
    "\t modify geometry. Moreover, unlike Euler operator, this function \n"
    "\t does not preserve the topological consistency of the CAD part. \n"
    "\t We have introduced this function to ground Euler operators on it.",
    //
    __FILE__, group, ENGINE_KillEdge);

  //-------------------------------------------------------------------------//
  interp->AddCommand("kill-face",
    //
    "kill-face faceIndex\n"
    "\t Kills face with the passed 1-based index from the active part. \n"
    "\t This is a pure topological operation which does not attempt to \n"
    "\t modify geometry. Moreover, unlike Euler operator, this function \n"
    "\t does not preserve the topological consistency of the CAD part. \n"
    "\t We have introduced this function to ground Euler operators on it.",
    //
    __FILE__, group, ENGINE_KillFace);

  //-------------------------------------------------------------------------//
  interp->AddCommand("kill-solid-by-face",
    //
    "kill-solid-by-face faceIndex\n"
    "\t Kills a solid which contains a face with the passed 1-based index.",
    //
    __FILE__, group, ENGINE_KillSolidByFace);

  //-------------------------------------------------------------------------//
  interp->AddCommand("move-by-face",
    //
    "move-by-face faceIndex offset\n"
    "\t Moves part in direction determined by orientation of the given face \n."
    "\t The passed offset value can be positive or negative. In the case of \n"
    "\t positive offset, the movement is done along the face normal. In the \n"
    "\t case of negative offset, the direction of movement is reversed. \n"
    "\n"
    "\t CAUTION: this command makes deep copy of the part. Our experience shows \n"
    "\t that simply assigning location may cause very weird behaviour on subsequent \n"
    "\t modeling operations.",
    //
    __FILE__, group, ENGINE_MoveByFace);

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
  interp->AddCommand("imprint",
    //
    "imprint varName \n"
    "\t Imprints the passed topological object to the working part. \n"
    "\t This operation is essenially the General Fuse algorithm of OpenCascde.",
    //
    __FILE__, group, ENGINE_Imprint);

  //-------------------------------------------------------------------------//
  interp->AddCommand("repair",
    //
    "repair\n"
    "\t Performs automatic shape repair on the active part.",
    //
    __FILE__, group, ENGINE_Repair);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-face-tolerance",
    //
    "set-face-tolerance faceId toler \n"
    "\t Forces the face with the given index to have the passed tolerance. \n"
    "\t In OpenCascade, there is a rule that a tolerance of a face should be \n"
    "\t not greater than tolerances of its edges (the same rule applies to \n"
    "\t edges and vertices). Therefore, this function updates tolerance not \n"
    "\t only for face but also for its sub-shapes.",
    //
    __FILE__, group, MISC_SetFaceTolerance);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-tolerance",
    //
    "set-face-tolerance toler \n"
    "\t Forces the part to have the passed tolerance in all its sub-shapes.",
    //
    __FILE__, group, MISC_SetTolerance);
}
