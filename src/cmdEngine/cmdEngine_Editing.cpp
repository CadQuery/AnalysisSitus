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
#include <asiAlgo_TopoAttrOrientation.h>
#include <asiAlgo_TopoKill.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>

//-----------------------------------------------------------------------------

int ENGINE_KEV(const Handle(asiTcl_Interp)& interp,
               int                          argc,
               const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get edge index.
  const int eidx = atoi(argv[1]);
  //
  if ( eidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge index should be 1-based.");
    return TCL_OK;
  }

  // Get vertex index.
  const int vidx = atoi(argv[2]);
  //
  if ( vidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Vertex index should be 1-based.");
    return TCL_OK;
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get maps of sub-shapes with respect to those the passed indices are relevant.
  const TopTools_IndexedMapOfShape& allEdges    = part_n->GetAAG()->GetMapOfEdges();
  const TopTools_IndexedMapOfShape& allVertices = part_n->GetAAG()->GetMapOfVertices();

  // Get topological elements.
  const TopoDS_Edge&   E = TopoDS::Edge( allEdges(eidx) );
  const TopoDS_Vertex& V = TopoDS::Vertex( allVertices(vidx) );

  // Prepare KEV utility.
  asiAlgo_EulerKEV KEV( part_n->GetShape(),
                        E,
                        V,
                        false,
                        interp->GetProgress(),
                        interp->GetPlotter() );
  //
  if ( !KEV.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "KEV failed.");
    return TCL_OK;
  }
  //
  const TopoDS_Shape& result = KEV.GetResult();

  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_KEF(const Handle(asiTcl_Interp)& interp,
               int                          argc,
               const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get edge index.
  const int eidx = atoi(argv[1]);
  //
  if ( eidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge index should be 1-based.");
    return TCL_OK;
  }

  // Get face index.
  const int fidx = atoi(argv[2]);
  //
  if ( fidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face index should be 1-based.");
    return TCL_OK;
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get maps of sub-shapes with respect to those the passed indices are relevant.
  const TopTools_IndexedMapOfShape& allEdges = part_n->GetAAG()->GetMapOfEdges();
  const TopTools_IndexedMapOfShape& allFaces = part_n->GetAAG()->GetMapOfFaces();

  // Get topological elements.
  const TopoDS_Edge& E = TopoDS::Edge( allEdges(eidx) );
  const TopoDS_Face& F = TopoDS::Face( allFaces(fidx) );

  // Prepare KEF utility.
  asiAlgo_EulerKEF KEF( part_n->GetShape(),
                        F,
                        E,
                        interp->GetProgress(),
                        interp->GetPlotter() );
  //
  if ( !KEF.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "KEF failed.");
    return TCL_OK;
  }
  //
  const TopoDS_Shape& result = KEF.GetResult();

  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MergeSubShapes(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv,
                          const TopAbs_ShapeEnum       ssType)
{
  if ( argc < 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get naming service.
  Handle(asiAlgo_Naming) naming = part_n->GetNaming();

  // Check whether naming service is active.
  const bool hasNaming = !naming.IsNull();
  //
  if ( !hasNaming )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "This function is available "
                                                           "for named shapes only.");
    return TCL_OK;
  }

  // Loop over the arguments to collect sub-shapes to merge.
  TopTools_ListOfShape subShapes;
  //
  for ( int k = 1; k < argc - 1; k += 2 )
  {
    if ( !interp->IsKeyword(argv[k], "name") )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Keyword '-name' is expected.");
      return TCL_ERROR;
    }

    // Get named sub-shape.
    TopoDS_Shape subshape = naming->GetShape(argv[k + 1]);
    //
    if ( subshape.IsNull() || subshape.ShapeType() != ssType )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The passed sub-shape is null "
                                                              "or not of a proper type.");
      return TCL_OK;
    }

    subShapes.Append(subshape);
  }

  // Prepare a replacement shape as the first shape.
  const TopoDS_Shape& replacement = subShapes.First();

  // Prepare killer.
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  killer.SetHistory( part_n->GetNaming()->GetHistory() );

  // Put requests on replacement.
  for ( TopTools_ListIteratorOfListOfShape lit(subShapes); lit.More(); lit.Next() )
  {
    const TopoDS_Shape& operand = lit.Value();

    // Notice that the substitution sub-shape is oriented like the operand.
    if ( !killer.AskReplace( operand, replacement.Oriented( operand.Orientation() ) ) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on replacement rejected.");
      return TCL_OK;
    }
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result.
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_KillSubShape(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv,
                        const TopAbs_ShapeEnum       ssType)
{
  if ( argc != 2 && argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Check whether naming service is active.
  const bool hasNaming = !part_n->GetNaming().IsNull();

  // Sub-shape in question.
  TopoDS_Shape subshape;

  // Check if naming service is active. If so, the user may ask to access
  // a sub-shape in question by its unique name.
  if ( hasNaming && argc == 3 )
  {
    if ( !interp->IsKeyword(argv[1], "name") )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Keyword '-name' is expected.");
      return TCL_ERROR;
    }
    else
    {
      TCollection_AsciiString name(argv[2]);
      //
      subshape = part_n->GetNaming()->GetShape(name);
      //
      if ( subshape.IsNull() || subshape.ShapeType() != ssType )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "The passed sub-shape is null "
                                                               "or not of a proper type.");
        return TCL_OK;
      }
    }
  }
  else // Naming is not used to access the argument.
  {
    const int ssidx = atoi(argv[1]);
    //
    if ( ssidx < 1 )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Sub-shape index should be 1-based.");
      return TCL_OK;
    }

    // Get map of sub-shapes with respect to those the passed index is relevant.
    TopTools_IndexedMapOfShape subShapesOfType;
    part_n->GetAAG()->GetMapOf(ssType, subShapesOfType);

    // Get sub-shape in question.
    subshape = subShapesOfType(ssidx);
  }

  // Prepare killer.
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( part_n->HasNaming() )
    killer.SetHistory( part_n->GetNaming()->GetHistory() );
  //
  if ( !killer.AskRemove(subshape) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal rejected.");
    return TCL_OK;
  }

  if ( !killer.Apply() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Topological killer failed.");
    return TCL_OK;
  }

  // Get result.
  const TopoDS_Shape& result = killer.GetResult();

  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_MergeEdges(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  return ENGINE_MergeSubShapes(interp, argc, argv, TopAbs_EDGE);
}

//-----------------------------------------------------------------------------

int ENGINE_MergeVertices(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  return ENGINE_MergeSubShapes(interp, argc, argv, TopAbs_VERTEX);
}

//-----------------------------------------------------------------------------

int ENGINE_KillVertex(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  return ENGINE_KillSubShape(interp, argc, argv, TopAbs_VERTEX);
}

//-----------------------------------------------------------------------------

int ENGINE_KillEdge(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  return ENGINE_KillSubShape(interp, argc, argv, TopAbs_EDGE);
}

//-----------------------------------------------------------------------------

int ENGINE_KillFace(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  return ENGINE_KillSubShape(interp, argc, argv, TopAbs_FACE);
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

int ENGINE_SetFaceTolerance(const Handle(asiTcl_Interp)& interp,
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

int ENGINE_SetTolerance(const Handle(asiTcl_Interp)& interp,
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
  interp->AddCommand("kev",
    //
    "kev edgeIndex vertexIndex \n"
    "\t KEV (Kill-Edge-Vertex) Euler operator.",
    //
    __FILE__, group, ENGINE_KEV);

  //-------------------------------------------------------------------------//
  interp->AddCommand("kef",
    //
    "kef edgeIndex faceIndex \n"
    "\t KEF (Kill-Edge-Face) Euler operator.",
    //
    __FILE__, group, ENGINE_KEF);

  //-------------------------------------------------------------------------//
  interp->AddCommand("merge-vertices",
    //
    "merge-vertices <-name 'vertexName1' ... -name 'vertexNameK'>\n"
    "\t Merges several vertices into one.",
    //
    __FILE__, group, ENGINE_MergeVertices);

  //-------------------------------------------------------------------------//
  interp->AddCommand("merge-edges",
    //
    "merge-edges <-name 'edgeName1' ... -name 'edgeNameK'>\n"
    "\t Merges several edges into one.",
    //
    __FILE__, group, ENGINE_MergeEdges);

  //-------------------------------------------------------------------------//
  interp->AddCommand("kill-vertex",
    //
    "kill-vertex <vertexIndex|-name 'vertexName'>\n"
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
    "kill-edge <edgeIndex|-name 'edgeName'>\n"
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
    "kill-face <faceIndex|-name 'faceName'>\n"
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
    __FILE__, group, ENGINE_SetFaceTolerance);

  //-------------------------------------------------------------------------//
  interp->AddCommand("set-tolerance",
    //
    "set-face-tolerance toler \n"
    "\t Forces the part to have the passed tolerance in all its sub-shapes.",
    //
    __FILE__, group, ENGINE_SetTolerance);
}
