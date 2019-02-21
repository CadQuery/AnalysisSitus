//-----------------------------------------------------------------------------
// Created on: 11 January 2017
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
