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

// cmdEngine includes
#include <cmdEngine.h>

// asiEngine includes
#include <asiEngine_Part.h>
#include <asiEngine_STEPReaderOutput.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiAlgo includes
#include <asiAlgo_ReadSTEPWithMeta.h>
#include <asiAlgo_STEP.h>
#include <asiAlgo_STEPReduce.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

//-----------------------------------------------------------------------------

void onModelLoaded(const TopoDS_Shape& loadedShape)
{
  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model).Update(loadedShape);
  }
  cmdEngine::model->CommitCommand();

  if ( cmdEngine::cf )
  {
    // Update viewer.
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

    // Update object browser.
    cmdEngine::cf->ObjectBrowser->Populate();
  }
}

//-----------------------------------------------------------------------------

void onModelLoaded(const Handle(Poly_Triangulation)& loadedMesh)
{
  // Modify Data Model.
  cmdEngine::model->OpenCommand();
  {
    cmdEngine::model->GetTriangulationNode()->SetTriangulation(loadedMesh);
  }
  cmdEngine::model->CommitCommand();

  if ( cmdEngine::cf )
  {
    // Update viewer.
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetTriangulationNode() );
  }
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

  // Prepare output
  Handle(asiEngine_STEPReaderOutput)
    output = new asiEngine_STEPReaderOutput(cmdEngine::model);

  // Prepare translator
  asiAlgo_ReadSTEPWithMeta reader( interp->GetProgress(),
                                   interp->GetPlotter() );
  reader.SetOutput(output);

  TIMER_NEW
  TIMER_GO

  // Load from STEP
  cmdEngine::model->OpenCommand(); // tx start
  {
    if ( !reader.Perform(filename) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "STEP reader failed.");
      //
      cmdEngine::model->AbortCommand();
      return TCL_ERROR;
    }
  }
  cmdEngine::model->CommitCommand();

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Load STEP file")

  // Update viewer.
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize( cmdEngine::model->GetPartNode() );

  // Update object browser.
  if ( cmdEngine::cf && cmdEngine::cf->ObjectBrowser )
    cmdEngine::cf->ObjectBrowser->Populate();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_SaveStep(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 && argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TCollection_AsciiString filename = ( argc == 2 ? argv[1] : argv[2] );

  TopoDS_Shape shape;
  if ( argc == 2 )
  {
    // Get Part Node to access shape.
    Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
    //
    if ( partNode.IsNull() || !partNode->IsWellFormed() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
      return TCL_ERROR;
    }
    //
    shape = partNode->GetShape();
  }
  else
  {
    // Get topological variable.
    Handle(asiData_IVTopoItemNode)
      topoItem = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
    //
    if ( topoItem.IsNull() || !topoItem->IsWellFormed() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
      return TCL_OK;
    }
    //
    shape = topoItem->GetShape();
  }

  // Save STEP.
  if ( !asiAlgo_STEP( interp->GetProgress() ).Write(shape, filename) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot save STEP file.");
    return TCL_ERROR;
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_LoadBRep(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TCollection_AsciiString filename(argv[1]);

  // Read BREP
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(filename, shape) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot read BREP file.");
    return TCL_ERROR;
  }

  onModelLoaded(shape);

  return TCL_OK;
}
//-----------------------------------------------------------------------------

int ENGINE_LoadSTL(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TCollection_AsciiString filename(argv[1]);

  // Read STL
  Handle(Poly_Triangulation) mesh;
  if ( !asiAlgo_Utils::ReadStl( filename, mesh, interp->GetProgress() ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot read STL file.");
    return TCL_ERROR;
  }

  onModelLoaded(mesh);

  return TCL_OK;
}


//-----------------------------------------------------------------------------

int ENGINE_DumpAAGJSON(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and its AAG.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_ERROR;
  }
  //
  Handle(asiAlgo_AAG) aag = partNode->GetAAG();
  //
  if ( aag.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "AAG is null.");
    return TCL_ERROR;
  }

  // Dump to file.
  std::ofstream filestream(argv[1]);
  //
  if ( !filestream.is_open() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "FILE_DEBUG: file cannot be opened.");
    return TCL_ERROR;
  }
  //
  aag->DumpJSON(filestream);
  filestream << "\n";
  //
  filestream.close();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_LoadPoints(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TCollection_AsciiString filename(argv[2]);

  // Load point cloud
  Handle(asiAlgo_BaseCloud<double>) cloud = new asiAlgo_BaseCloud<double>;
  //
  if ( !cloud->Load( filename.ToCString() ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot load point cloud.");
    return TCL_ERROR;
  }
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Point cloud was loaded successfully.");

  interp->GetPlotter().REDRAW_POINTS(argv[1], cloud->GetCoordsArray(), Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ReduceSTEP(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  std::string inFilename(argv[1]);
  std::string outFilename(argv[2]);

  TIMER_NEW
  TIMER_GO

  // Run the compression tool.
  asiAlgo_STEPReduce ReduceTool( interp->GetProgress(),
                                 interp->GetPlotter() );
  //
  if ( !ReduceTool.Peform(inFilename, outFilename) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "STEP reduction failed.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Reduce STEP")

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Interop(const Handle(asiTcl_Interp)&      interp,
                                 const Handle(Standard_Transient)& cmdEngine_NotUsed(data))
{
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("load-step",
    //
    "load-step filename\n"
    "\t Loads STEP file to the active part.",
    //
    __FILE__, group, ENGINE_LoadStep);

  //-------------------------------------------------------------------------//
  interp->AddCommand("save-step",
    //
    "save-step [varName] filename\n"
    "\t Save active part or variable (if specified) to a STEP file with the\n"
    "\t given name.",
    //
    __FILE__, group, ENGINE_SaveStep);

  //-------------------------------------------------------------------------//
  interp->AddCommand("load-brep",
    //
    "load-brep filename\n"
    "\t Loads BREP file to the active part.",
    //
    __FILE__, group, ENGINE_LoadBRep);

  //-------------------------------------------------------------------------//
  interp->AddCommand("load-stl",
    //
    "load-stl filename\n"
    "\t Loads STL file to the active triangulation.",
    //
    __FILE__, group, ENGINE_LoadSTL);

  //-------------------------------------------------------------------------//
  interp->AddCommand("dump-aag-json",
    //
    "dump-aag-json filename\n"
    "\t Dumps AAG of the active part to JSON file.",
    //
    __FILE__, group, ENGINE_DumpAAGJSON);

  //-------------------------------------------------------------------------//
  interp->AddCommand("load-points",
    //
    "load-points name filename\n"
    "\t Loads points from file to the point cloud with the given name.",
    //
    __FILE__, group, ENGINE_LoadPoints);

  //-------------------------------------------------------------------------//
  interp->AddCommand("reduce-step",
    //
    "reduce-step <inFlename> <outFilename>\n"
    "\t Applies STEP reduction procedure developed by Seth Hillbrand for KICAD.",
    //
    __FILE__, group, ENGINE_ReduceSTEP);
}
