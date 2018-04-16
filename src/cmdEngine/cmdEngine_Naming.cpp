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
#include <asiAlgo_Naming.h>

//-----------------------------------------------------------------------------

int ENGINE_InitNaming(const Handle(asiTcl_Interp)& interp,
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

  // Initialize naming service.
  cmdEngine::model->OpenCommand();
  {
    asiEngine_Part(cmdEngine::model, NULL).InitializeNaming();
  }
  cmdEngine::model->CommitCommand();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Naming(const Handle(asiTcl_Interp)&      interp,
                                const Handle(Standard_Transient)& data)
{
  cmdEngine_NotUsed(data);
  //
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("init-naming",
    //
    "init-naming\n"
    "\t Initializes topological naming service for the active part.",
    //
    __FILE__, group, ENGINE_InitNaming);
}
