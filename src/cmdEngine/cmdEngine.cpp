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

// asiAlgo includes
#include <asiAlgo_STEP.h>
#include <asiAlgo_TopoGraph.h>
#include <asiAlgo_TopoKill.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_DialogCommands.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// OCCT includes
#include <BRepLProp_SLProps.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepTools.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

//-----------------------------------------------------------------------------

Handle(asiEngine_Model)        cmdEngine::model = NULL;
Handle(asiUI_CommonFacilities) cmdEngine::cf    = NULL;

//-----------------------------------------------------------------------------

void onUndoRedo(const Handle(ActAPI_HParameterMap)& affectedParams)
{
  if ( affectedParams.IsNull() )
    return;

  // Loop over the affected Parameters to get the affected Nodes. These Nodes
  // are placed into a map to have them unique.
  Handle(ActAPI_HNodeMap) affectedNodes = new ActAPI_HNodeMap;
  //
  for ( ActAPI_HParameterMap::Iterator pit(*affectedParams); pit.More(); pit.Next() )
  {
    // Get Node
    Handle(ActAPI_INode) N = pit.Value()->GetNode();
    //
    affectedNodes->Add(N);
  }

  // Get all presentation managers
  const vtkSmartPointer<asiVisu_PrsManager>& partPM   = cmdEngine::cf->ViewerPart->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& hostPM   = cmdEngine::cf->ViewerHost->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& domainPM = cmdEngine::cf->ViewerDomain->PrsMgr();

  // Loop over the unique Nodes to actualize them
  for ( ActAPI_HNodeMap::Iterator nit(*affectedNodes); nit.More(); nit.Next() )
  {
    const Handle(ActAPI_INode)& N = nit.Value();

    // Actualize
    if ( partPM->IsPresented(N) )
      partPM->Actualize(N);
    //
    if ( hostPM->IsPresented(N) )
      hostPM->Actualize(N);
    //
    if ( domainPM->IsPresented(N) )
      domainPM->Actualize(N);
  }

  // Update object browser
  cmdEngine::cf->ObjectBrowser->Populate();
}

//-----------------------------------------------------------------------------

int ENGINE_Commands(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  asiUI_DialogCommands*
    pDlg = new asiUI_DialogCommands(interp, interp->GetProgress(), NULL);
  //
  pDlg->show();

  return TCL_OK;
}

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
      return TCL_ERROR;
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

  // Get Part Node and shape
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
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
    return TCL_ERROR;
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
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

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
    return TCL_ERROR;
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
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

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
    return TCL_ERROR;
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
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Owner solid is null. Cannot proceed." << fidx);
    return TCL_OK;
  }

  // Prepare killer
  asiAlgo_TopoKill killer( cmdEngine::model->GetPartNode()->GetShape(),
                           interp->GetProgress(),
                           interp->GetPlotter() );
  //
  if ( !killer.AskRemove(ownerSolid) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Request on removal of solid was rejected." << fidx);
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
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

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
    return TCL_ERROR;
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
    return TCL_ERROR;
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
  TopoDS_Shape moved = partShape.Moved(T);

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

int ENGINE_ThickenShell(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get part shape
  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast(interp->GetModel())->GetPartNode()->GetShape();

  // Get offset value
  const double offsetVal = atof(argv[1]);

  // Make offset
  BRepOffsetAPI_MakeThickSolid mkOffset;
  mkOffset.MakeThickSolidBySimple(partShape, offsetVal);
  //
  if ( !mkOffset.IsDone() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Offset not done.");
    return TCL_ERROR;
  }

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update( mkOffset.Shape() );
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_SetAsPart(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiData_IVTopoItemNode)
    node = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_ERROR;
  }

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update( node->GetShape() );
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

  // Get Part Node
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Fix shape
  ShapeFix_Shape fix( part_n->GetShape() );
  fix.Perform();
  TopoDS_Shape result = fix.Shape();

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(result);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_LoadStep(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TCollection_AsciiString filename(argv[1]);

  // Read STEP
  TopoDS_Shape shape;
  if ( !asiAlgo_STEP::Read(filename, false, shape) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot read STEP file.");
    return TCL_OK;
  }

  // Modify Data Model
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).Update(shape);
  }
  cmdEngine::model->CommitCommand();

  // Update UI
  if ( cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

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
    return TCL_ERROR;
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

int ENGINE_Erase(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find node
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_ERROR;
  }

  // Hide
  if ( cmdEngine::cf->ViewerPart->PrsMgr()->IsPresented(node) )
  {
    cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderPresentation(node);
  }
  else
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "There is no presentable object with name %1." << argv[1]);

  // Repaint
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_DOnly(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find node
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_ERROR;
  }

  // Display only
  cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderAllPresentations();
  //
  if ( cmdEngine::cf->ViewerPart->PrsMgr()->IsPresented(node) )
  {
    cmdEngine::cf->ViewerPart->PrsMgr()->RenderPresentation(node);
  }
  else
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "There is no presentable object with name %1." << argv[1]);

  // Repaint
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EraseAll(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  cmdEngine::cf->ViewerPart->PrsMgr()->DeRenderAllPresentations();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Fit(const Handle(asiTcl_Interp)& interp,
               int                          argc,
               const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  asiVisu_Utils::ResetCamera( cmdEngine::cf->ViewerPart->PrsMgr()->GetRenderer(),
                              cmdEngine::cf->ViewerPart->PrsMgr()->PropsByTrihedron() );
  //
  asiVisu_Utils::AdjustCamera( cmdEngine::cf->ViewerPart->PrsMgr()->GetRenderer(),
                               cmdEngine::cf->ViewerPart->PrsMgr()->PropsByTrihedron() );
  //
  cmdEngine::cf->ViewerPart->Repaint();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Undo(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Undo and process the affected Parameters
  onUndoRedo( cmdEngine::model->Undo() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_Redo(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Redo and process the affected Parameters
  onUndoRedo( cmdEngine::model->Redo() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Factory(const Handle(asiTcl_Interp)&      interp,
                        const Handle(Standard_Transient)& data)
{
  static const char* group = "cmdEngine";

  /* ==========================
   *  Initialize UI facilities
   * ========================== */

  // Get common facilities
  Handle(asiUI_CommonFacilities)
    passedCF = Handle(asiUI_CommonFacilities)::DownCast(data);
  //
  if ( passedCF.IsNull() )
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "[cmdEngine] No UI facilities are available. GUI will not be updated!");
  else
    cf = passedCF;

  /* ================================
   *  Initialize Data Model instance
   * ================================ */

  model = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  if ( model.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[cmdEngine] Data Model instance is NULL or not of asiEngine_Model kind.");
    return;
  }

  /* =====================
   *  Add custom commands
   * ===================== */

  //-------------------------------------------------------------------------//
  interp->AddCommand("commands",
    //
    "commands\n"
    "\t Opens dialog which lists all available commands for analysis.",
    //
    __FILE__, group, ENGINE_Commands);

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
    "\t Kills solid which contains a face with the passed 1-based index.",
    //
    __FILE__, group, ENGINE_KillSolidByFace);

  //-------------------------------------------------------------------------//
  interp->AddCommand("move-by-face",
    //
    "move-by-face faceIndex offset\n"
    "\t Moves part in direction determined by orientation of the given face \n."
    "\t The passed offset value can be positive or negative. In the case of \n"
    "\t positive offset, the movement is done along the face normal. In the \n"
    "\t case of negative offset, the direction of movement is reversed.",
    //
    __FILE__, group, ENGINE_MoveByFace);

  //-------------------------------------------------------------------------//
  interp->AddCommand("thicken-shell",
    //
    "thicken-shell offset\n"
    "\t Thickens part (it should be a topological shell) on the given offset \n"
    "\t value. Thickenning is performed in direction of face normals.",
    //
    __FILE__, group, ENGINE_ThickenShell);

    //-------------------------------------------------------------------------//
  interp->AddCommand("set-as-part",
    //
    "set-as-part varName\n"
    "\t Sets the object with the given name as a part for analysis. \n"
    "\t The object is expected to exist as a topological item in \n"
    "\t imperative plotter.",
    //
    __FILE__, group, ENGINE_SetAsPart);

  //-------------------------------------------------------------------------//
  interp->AddCommand("repair",
    //
    "repair\n"
    "\t Performs automatic shape repair on the active part.",
    //
    __FILE__, group, ENGINE_Repair);

  //-------------------------------------------------------------------------//
  interp->AddCommand("load-step",
    //
    "load-step filename\n"
    "\t Loads STEP file to the active part.",
    //
    __FILE__, group, ENGINE_LoadStep);

  //-------------------------------------------------------------------------//
  interp->AddCommand("face-addr",
    //
    "face-addr faceIndex \n"
    "\t Prints physical address of the given face.",
    //
    __FILE__, group, ENGINE_FaceAddr);

  //-------------------------------------------------------------------------//
  interp->AddCommand("erase",
    //
    "erase varName \n"
    "\t Hides object in viewer.",
    //
    __FILE__, group, ENGINE_Erase);

  //-------------------------------------------------------------------------//
  interp->AddCommand("donly",
    //
    "donly varName \n"
    "\t Shows only the given object in viewer.",
    //
    __FILE__, group, ENGINE_DOnly);

  //-------------------------------------------------------------------------//
  interp->AddCommand("erase-all",
    //
    "erase-all \n"
    "\t Erases all objects from the scene.",
    //
    __FILE__, group, ENGINE_EraseAll);

  //-------------------------------------------------------------------------//
  interp->AddCommand("fit",
    //
    "fit \n"
    "\t Fits camera to the scene contents.",
    //
    __FILE__, group, ENGINE_Fit);

  //-------------------------------------------------------------------------//
  interp->AddCommand("undo",
    //
    "undo \n"
    "\t Undoes model changes.",
    //
    __FILE__, group, ENGINE_Undo);

  //-------------------------------------------------------------------------//
  interp->AddCommand("redo",
    //
    "redo \n"
    "\t Redoes model changes.",
    //
    __FILE__, group, ENGINE_Redo);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdEngine)
