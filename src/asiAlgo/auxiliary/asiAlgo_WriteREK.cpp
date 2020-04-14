//-----------------------------------------------------------------------------
// Created on: 11 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiAlgo_WriteREK.h>

//-----------------------------------------------------------------------------

asiAlgo_WriteREK::asiAlgo_WriteREK(const std::string& filename)
{
  m_pFILE = new std::ofstream(filename, std::ios::binary);
  if ( !m_pFILE->is_open() )
  {
    m_pFILE = nullptr;
    throw std::runtime_error("Cannot open file for writing.");
  }
}

//-----------------------------------------------------------------------------

asiAlgo_WriteREK::~asiAlgo_WriteREK()
{
  if ( m_pFILE != nullptr )
    m_pFILE->close();

  delete m_pFILE;
}

//-----------------------------------------------------------------------------

bool asiAlgo_WriteREK::Write(const Handle(asiAlgo_UniformGrid<float>)& grid)
{
  // Contract check.
  if ( (m_pFILE == nullptr) || !m_pFILE->is_open() )
    return false;

  // Prepare a header for the REK file.
  t_header rekHeader;
  rekHeader.SizeX     = (uint16_t) grid->Nx + 1;
  rekHeader.SizeY     = (uint16_t) grid->Ny + 1;
  rekHeader.Pixel     = 32; // float.
  rekHeader.SizeZ     = (uint16_t) grid->Nz + 1;
  rekHeader.PixelSize = grid->CellSize;
  rekHeader.SliceDist = grid->CellSize;

  // Write header.
  m_pFILE->write( (char*) &rekHeader.SizeX,     sizeof(uint16_t) );
  m_pFILE->write( (char*) &rekHeader.SizeY,     sizeof(uint16_t) );
  m_pFILE->write( (char*) &rekHeader.Pixel,     sizeof(uint16_t) );
  m_pFILE->write( (char*) &rekHeader.SizeZ,     sizeof(uint16_t) );
  m_pFILE->write( (char*) &rekHeader.Res1,      sizeof(char[572]) );
  m_pFILE->write( (char*) &rekHeader.SomeValue, sizeof(float) );
  m_pFILE->write( (char*) &rekHeader.PixelSize, sizeof(float) );
  m_pFILE->write( (char*) &rekHeader.SliceDist, sizeof(float) );
  m_pFILE->write( (char*) &rekHeader.Res1,      sizeof(char[1456]) );

  /*
    Write raw data. Notice that the X coordinates should go first.
    Therefore, we render the raw data plan-by-plan, i.e., the XOY
    slices are stacked along the OZ direction, hence the order of
    the nested loops.

    The order is important for non-cubic domains, i.e., in the
    situations when Nx, Ny, Nz are not equal.
   */
  for ( int k = 0; k <= grid->Nz; ++k )
  {
    for ( int j = 0; j <= grid->Ny; ++j )
    {
      for ( int i = 0; i <= grid->Nx; ++i )
      {
        m_pFILE->write( (char*) &grid->pArray[i][j][k], sizeof(float) );
      }
    }
  }

  return true;
}
