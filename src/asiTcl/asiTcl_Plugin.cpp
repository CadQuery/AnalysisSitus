//-----------------------------------------------------------------------------
// Created on: 23 August 2017
// Created by: Sergey SLYADNEV
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

// Own include
#include <asiTcl_Plugin.h>

// OCCT includes
#include <OSD_SharedLibrary.hxx>

//-----------------------------------------------------------------------------

bool asiTcl_Plugin::Load(const Handle(asiTcl_Interp)&      interp,
                         const Handle(Standard_Transient)& data,
                         const TCollection_AsciiString&    pluginName)
{
  TCollection_AsciiString libFilename;

#ifdef _WIN32
  libFilename = pluginName;
#else
  libFilename = "lib"; libFilename += pluginName; libFilename += ".so";
#endif

  OSD_SharedLibrary SharedLibrary( libFilename.ToCString() );

  if ( !SharedLibrary.DlOpen(OSD_RTLD_LAZY) )
  {
    std::cout << "Error: cannot load " << libFilename.ToCString() << std::endl;
    return false;
  }

  OSD_Function f = SharedLibrary.DlSymb("PLUGINFACTORY");
  if ( f == nullptr )
  {
    std::cout << "Error: cannot find factory (function PLUGINFACTORY) in " << libFilename.ToCString() << std::endl;
    return false;
  }

  // Cast
  void (*fp) (const Handle(asiTcl_Interp)&, const Handle(Standard_Transient)& ) = nullptr;
  fp = (void (*)(const Handle(asiTcl_Interp)&, const Handle(Standard_Transient)&)) f;

  // Let interpreter store all loaded plugins
  interp->m_plugins.push_back(pluginName);

  // Call
  (*fp) (interp, data);
  return true;
}
