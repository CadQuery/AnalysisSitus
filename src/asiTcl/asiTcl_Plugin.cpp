//-----------------------------------------------------------------------------
// Created on: 23 August 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
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
  OSD_SharedLibrary SharedLibrary( pluginName.ToCString() );

  if ( !SharedLibrary.DlOpen(OSD_RTLD_LAZY) )
  {
    std::cout << "Error: cannot load " << pluginName.ToCString() << std::endl;
    return false;
  }

  OSD_Function f = SharedLibrary.DlSymb("PLUGINFACTORY");
  if ( f == NULL )
  {
    std::cout << "Error: cannot find factory (function PLUGINFACTORY) in " << pluginName.ToCString() << std::endl;
    return false;
  }

  // Cast
  void (*fp) (const Handle(asiTcl_Interp)&, const Handle(Standard_Transient)& ) = NULL;
  fp = (void (*)(const Handle(asiTcl_Interp)&, const Handle(Standard_Transient)&)) f;

  // Let interpreter store all loaded plugins
  interp->m_plugins.push_back(pluginName);

  // Call
  (*fp) (interp, data);
  return true;
}
