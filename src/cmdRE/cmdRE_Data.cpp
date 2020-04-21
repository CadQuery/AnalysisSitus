//-----------------------------------------------------------------------------
// Created on: 05 July 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

// asiEngine includes
#include <asiEngine_Part.h>
#include <asiEngine_RE.h>

// OpenCascade includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Shell.hxx>

//-----------------------------------------------------------------------------

int RE_SetPatchSurf(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get patch Node.
  Handle(asiData_RePatchNode)
    patchNode = Handle(asiData_RePatchNode)::DownCast( cmdRE::model->FindNodeByName(argv[1]) );
  //
  if ( patchNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Data object '%1' is not a patch."
                                                        << argv[1]);
    return TCL_ERROR;
  }

  // Get surface Node.
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast( cmdRE::model->FindNodeByName(argv[2]) );
  //
  if ( surfNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Data object '%1' is not a surface."
                                                        << argv[2]);
    return TCL_ERROR;
  }

  // Set surface to patch.
  cmdRE::model->OpenCommand();
  {
    patchNode->SetSurface( surfNode->GetSurface() );
  }
  cmdRE::model->CommitCommand();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int RE_SetPatchesAsPart(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  TopoDS_Shell shell;
  BRep_Builder().MakeShell(shell);

  // Loop over the available patches.
  Handle(asiData_Partition<asiData_RePatchNode>) P = cmdRE::model->GetRePatchPartition();
  //
  for ( asiData_Partition<asiData_RePatchNode>::Iterator pit(P); pit.More(); pit.Next() )
  {
    Handle(asiData_RePatchNode)
      patchNode = Handle(asiData_RePatchNode)::DownCast( pit.Value() );

    Handle(Geom_Surface) surf = patchNode->GetSurface();
    //
    if ( surf.IsNull() )
    {
      interp->GetProgress().SendLogMessage( LogWarn(Normal) << "No surface for patch '%1'."
                                                            << patchNode->GetName() );
      continue;
    }

    // Add surface to shell.
    BRep_Builder().Add( shell, BRepBuilderAPI_MakeFace(surf, 1e-7) );
  }

  // Modify Data Model.
  Handle(asiData_PartNode) part_n;
  //
  cmdRE::model->OpenCommand();
  {
    part_n = asiEngine_Part(cmdRE::model).Update(shell);
  }
  cmdRE::model->CommitCommand();

  // Update UI.
  if ( cmdRE::cf && cmdRE::cf->ViewerPart )
    cmdRE::cf->ViewerPart->PrsMgr()->Actualize(part_n);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdRE::Commands_Data(const Handle(asiTcl_Interp)&      interp,
                          const Handle(Standard_Transient)& cmdRE_NotUsed(data))
{
  static const char* group = "cmdRE";

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-set-patch-surf",
    //
    "re-set-patch-surf <patchName> <surfName>\n"
    "\t Sets new surface for a patch with the given name.",
    //
    __FILE__, group, RE_SetPatchSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("re-set-patches-as-part",
    //
    "re-set-patches-as-part\n"
    "\t Sets all available patches as a geometry of Part Node.",
    //
    __FILE__, group, RE_SetPatchesAsPart);
}
