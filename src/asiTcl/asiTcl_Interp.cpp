//-----------------------------------------------------------------------------
// Created on: 22 August 2017
// Created by: Sergey SLYADNEV
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

// Own include
#include <asiTcl_Interp.h>

// Tcl includes
#include <tcl.h>

//-----------------------------------------------------------------------------

//! Function to pass to Tcl_CreateCommand as an entry point to any user-defined
//! command.
//! \param[in] pClientData callback.
//! \param[in] pInterpTcl  Tcl interpreter.
//! \param[in] argc        number of command line arguments.
//! \param[ni] argv        command line arguments.
//! \return Tcl error code.
static int TclProcInvoke(void*       pClientData,
                         Tcl_Interp* pInterpTcl,
                         int         argc,
                         const char* argv[])
{
  asiTcl_NotUsed(pInterpTcl);

  static int code;
  code = TCL_OK;
  asiTcl_Interp::t_tcl_callback* pCallback = (asiTcl_Interp::t_tcl_callback*) pClientData;

  // Invoke
  try
  {
    int fres = pCallback->Invoke(argc, argv);

    if ( fres != TCL_OK )
      code = TCL_ERROR;
  }
  catch ( ... )
  {
    std::cout << "Error: exception occurred" << std::endl;
    code = TCL_ERROR;
  }
  return code;
}

//-----------------------------------------------------------------------------

static void TclProcDelete(void* pClientData)
{
  asiTcl_Interp::t_tcl_callback* pCallback = (asiTcl_Interp::t_tcl_callback*) pClientData;
  delete pCallback;
}

//-----------------------------------------------------------------------------

asiTcl_Interp::asiTcl_Interp() : Standard_Transient()
{
  m_pInterp = NULL;
}

//-----------------------------------------------------------------------------

asiTcl_Interp::~asiTcl_Interp()
{
  if ( m_pInterp )
    Tcl_DeleteInterp(m_pInterp);
}

//-----------------------------------------------------------------------------

void asiTcl_Interp::Init()
{
  if ( m_pInterp )
    Tcl_DeleteInterp(m_pInterp);

  m_pInterp = Tcl_CreateInterp();
}

//-----------------------------------------------------------------------------

int asiTcl_Interp::Eval(const TCollection_AsciiString& cmd)
{
  if ( !m_pInterp )
    return TCL_ERROR;

  int ret = Tcl_Eval( m_pInterp, cmd.ToCString() );
  return ret;
}

//-----------------------------------------------------------------------------

bool asiTcl_Interp::AddCommand(const TCollection_AsciiString& name,
                               const TCollection_AsciiString& help,
                               const TCollection_AsciiString& filename,
                               t_user_func                    func)
{
  t_tcl_callback* pCallback = new t_tcl_callback(this, func);
  return this->addCommand(name, help, filename, pCallback);
}

//-----------------------------------------------------------------------------

bool asiTcl_Interp::addCommand(const TCollection_AsciiString& name,
                               const TCollection_AsciiString& help,
                               const TCollection_AsciiString& filename,
                               t_tcl_callback*                callback)
{
  asiTcl_NotUsed(help);
  asiTcl_NotUsed(filename);

  if ( !m_pInterp )
    return false;

  // TODO: "help" and "getsource" functions are not yet implemented.

  Tcl_Command cmd = Tcl_CreateCommand(m_pInterp,
                                      name.ToCString(),
                                      TclProcInvoke,
                                      (void*) callback,
                                      TclProcDelete);

  if ( cmd == NULL )
    return false;

  return true;
}
