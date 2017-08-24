//-----------------------------------------------------------------------------
// Created on: 24 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// cmdInspector includes
#include <cmdInspector.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Part.h>

//-----------------------------------------------------------------------------

Handle(exe_CommonFacilities) cmdInspector::cf = NULL;

//-----------------------------------------------------------------------------

int INSPECTOR_SetAsPart(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiData_IVTopoItemNode)
    node = Handle(asiData_IVTopoItemNode)::DownCast( cmdInspector::cf->Model->FindNodeByName(argv[1]) );
  //
  if ( node.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_ERROR;
  }

  // Modify Data Model
  cmdInspector::cf->Model->OpenCommand();
  {
    asiEngine_Part(cmdInspector::cf->Model, NULL).Update( node->GetShape() );
  }
  cmdInspector::cf->Model->CommitCommand();

  // Update UI
  cmdInspector::UpdateUI();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdInspector::UpdateUI()
{
  cf->ViewerPart->PrsMgr()->Actualize(cf->Model->GetPartNode(), false, true);
}

//-----------------------------------------------------------------------------

void cmdInspector::Factory(const Handle(asiTcl_Interp)&      interp,
                           const Handle(Standard_Transient)& data)
{
  /* ==============================
   *  Initialize common facilities
   * ============================== */

  cmdInspector::cf = Handle(exe_CommonFacilities)::DownCast(data);
  //
  if ( cf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[cmdInspector] Common facilities are NULL.");
    return;
  }

  /* =====================
   *  Add custom commands
   * ===================== */

  // Add commands
  interp->AddCommand("set-as-part", "", __FILE__, INSPECTOR_SetAsPart);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdInspector)
