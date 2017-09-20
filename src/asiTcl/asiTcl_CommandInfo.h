//-----------------------------------------------------------------------------
// Created on: 19 September 2017
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

#ifndef asiTcl_CommandInfo_h
#define asiTcl_CommandInfo_h

// asiTcl includes
#include <asiTcl.h>

// Standard includes
#include <string>

//! This structure organizes information available for a single Tcl command
//! as it is seen from caller code.
struct asiTcl_CommandInfo
{
  std::string Group;    //!< Plugin the command belongs to.
  std::string Name;     //!< Command name.
  std::string Help;     //!< Help line.
  std::string Filename; //!< Filename which points to the source of command.
};

#endif
