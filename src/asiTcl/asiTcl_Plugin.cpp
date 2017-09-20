//-----------------------------------------------------------------------------
// Created on: 23 August 2017
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
