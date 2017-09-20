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

// OCCT includes
#include <OSD_Path.hxx>

//-----------------------------------------------------------------------------

//! Splits the passed string by the given delimiter. Note that the
//! passed output vector is not cleaned up beforehand.
//! \param source_str [in]  input string to split.
//! \param delim_str  [in]  delimiter string.
//! \param result     [out] resulting collection of tokens after split.
static void splitstr(const std::string&        source_str,
                     const std::string&        delim_str,
                     std::vector<std::string>& result)
{
  // Initialize collection of strings to split
  std::vector<std::string> chunks;
  chunks.push_back(source_str);

  // Split by each delimiter consequently
  for ( size_t delim_idx = 0; delim_idx < delim_str.length(); ++delim_idx )
  {
    std::vector<std::string> new_chunks;
    const char delim = delim_str[delim_idx];

    // Split each chunk
    for ( size_t chunk_idx = 0; chunk_idx < chunks.size(); ++chunk_idx )
    {
      const std::string& source = chunks[chunk_idx];
      std::string::size_type currPos = 0, prevPos = 0;
      while ( (currPos = source.find(delim, prevPos) ) != std::string::npos )
      {
        std::string item = source.substr(prevPos, currPos - prevPos);
        if ( item.size() > 0 )
        {
          new_chunks.push_back(item);
        }
        prevPos = currPos + 1;
      }
      new_chunks.push_back( source.substr(prevPos) );
    }

    // Set new collection of chunks for splitting by the next delimiter
    chunks = new_chunks;
  }

  // Set result
  result = chunks;
}

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
    std::cerr << "Error: exception occurred" << std::endl;
    code = TCL_ERROR;
    //
    std::cerr << "\t*** " << Tcl_GetStringResult(pInterpTcl) << std::endl;
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
  {
    this->GetProgress().SendLogMessage(LogErr(Normal) << "Tcl command has finished with TCL_ERROR");
    return TCL_ERROR;
  }

  int ret = Tcl_Eval( m_pInterp, cmd.ToCString() );
  return ret;
}

//-----------------------------------------------------------------------------

bool asiTcl_Interp::AddCommand(const TCollection_AsciiString& name,
                               const TCollection_AsciiString& help,
                               const TCollection_AsciiString& filename,
                               const TCollection_AsciiString& group,
                               t_user_func                    func)
{
  t_tcl_callback* pCallback = new t_tcl_callback(this, func);
  return this->addCommand(name, help, filename, group, pCallback);
}

//-----------------------------------------------------------------------------

void asiTcl_Interp::GetAvailableCommands(std::vector<asiTcl_CommandInfo>& commands) const
{
  for ( size_t p = 0; p < m_plugins.size(); ++p )
  {
    std::string
      commandsInGroupStr = Tcl_GetVar2(m_pInterp, "asi_Groups", m_plugins[p].ToCString(),
                                       TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);

    std::vector<std::string> commandsInGroup;
    splitstr(commandsInGroupStr, " ", commandsInGroup);

    for ( size_t c = 0; c < commandsInGroup.size(); ++c )
    {
      asiTcl_CommandInfo cmdInfo;
      cmdInfo.Group = m_plugins[p].ToCString();
      cmdInfo.Name  = commandsInGroup[c];

      // Help
      std::string
        commandHelp = Tcl_GetVar2(m_pInterp, "asi_Help", cmdInfo.Name.c_str(),
                                  TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
      //
      cmdInfo.Help = commandHelp;

      // Source
      std::string
        commandSrc = Tcl_GetVar2(m_pInterp, "asi_Files", cmdInfo.Name.c_str(),
                                 TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
      //
      cmdInfo.Filename = commandSrc;

      // Set to output
      commands.push_back(cmdInfo);
    }
  }
}

//-----------------------------------------------------------------------------

int asiTcl_Interp::ErrorOnWrongArgs(const char* cmd)
{
  std::cerr << "Error: wrong number of arguments in command " << cmd;
  this->GetProgress().SendLogMessage(LogErr(Normal) << "Wrong number of arguments in command %1." << cmd);

  return TCL_ERROR;
}

//-----------------------------------------------------------------------------

bool asiTcl_Interp::addCommand(const TCollection_AsciiString& name,
                               const TCollection_AsciiString& help,
                               const TCollection_AsciiString& filename,
                               const TCollection_AsciiString& group,
                               t_tcl_callback*                callback)
{
  if ( !m_pInterp )
    return false;

  Tcl_Command cmd = Tcl_CreateCommand(m_pInterp,
                                      name.ToCString(),
                                      TclProcInvoke,
                                      (void*) callback,
                                      TclProcDelete);

  if ( cmd == NULL )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Tcl_CreateCommand has returned NULL.");
    return false;
  }

  // Each custom Tcl command of Analysis Situs is supported with the following
  // additional information:
  //
  // - help string;
  // - filename of the source file where the command is implemented;
  // - group name to organize Tcl commands logically.
  //
  // All this is stored by means of dedicated Tcl variables (correspondingly):
  //
  // - asi_Help
  // - asi_Files
  // - asi_Groups

  // Help line
  Tcl_SetVar2(m_pInterp, "asi_Help", name.ToCString(), help.ToCString(),
              TCL_GLOBAL_ONLY);

  // Source filename
  if ( !filename.IsEmpty() )
  {
    // The following code for manipulation with paths is taken from
    // OpenCascade's Draw package.

    OSD_Path path(filename);
    for ( int i = 2; i < path.TrekLength(); ++i )
    {
      path.RemoveATrek(1);
    }
    path.SetDisk("");
    path.SetNode("");
    //
    TCollection_AsciiString srcPath;
    path.SystemName(srcPath);
    //
    if ( srcPath.Value(1) == '/' )
      srcPath.Remove(1);

    // Filename
    Tcl_SetVar2(m_pInterp, "asi_Files", name.ToCString(), srcPath.ToCString(),
                TCL_GLOBAL_ONLY);
  }

  // Group
  Tcl_SetVar2(m_pInterp, "asi_Groups", group.ToCString(), name.ToCString(),
              TCL_GLOBAL_ONLY | TCL_APPEND_VALUE | TCL_LIST_ELEMENT);

  return true;
}
