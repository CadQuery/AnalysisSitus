//-----------------------------------------------------------------------------
// Created on: 11 January 2017
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

#ifndef asiAlgo_BorderTrihedron_h
#define asiAlgo_BorderTrihedron_h

// OCCT includes
#include <gp_Dir.hxx>

//-----------------------------------------------------------------------------

//! Border trihedron associated with a sheet face at connection zones.
class asiAlgo_BorderTrihedron
{
public:

  //! Default ctor.
  asiAlgo_BorderTrihedron() {}

  //! Creates border trihedron.
  //! \param[in] _V_origin local origin of the trihedron.
  //! \param[in] _V_x      local OX axis of the trihedron.
  //! \param[in] _V_y      local OY axis of the trihedron.
  //! \param[in] _V_z      local OZ axis of the trihedron.
  asiAlgo_BorderTrihedron(const gp_Pnt& _V_origin,
                          const gp_Dir& _V_x,
                          const gp_Dir& _V_y,
                          const gp_Dir& _V_z)
  {
    V_origin = _V_origin;
    V_x      = _V_x;
    V_y      = _V_y;
    V_z      = _V_z;
  }

public:

  gp_Pnt V_origin;
  gp_Dir V_x;
  gp_Dir V_y;
  gp_Dir V_z;

};

#endif
