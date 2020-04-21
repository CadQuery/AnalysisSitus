//-----------------------------------------------------------------------------
// Created on: 10 April 2020
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

#ifndef asiAlgo_UniformGrid_h
#define asiAlgo_UniformGrid_h

// asiAlgo includes
#include <asiAlgo.h>

// OpenCascade includes
#include <Standard_Type.hxx>

//! Convenience class for a three-dimensional array which represents a
//! uniform voxelization.
template <typename T>
class asiAlgo_UniformGrid : public Standard_Transient
{
public:

  //! Ctor.
  //! \param[in] xmin     min X.
  //! \param[in] ymin     min Y.
  //! \param[in] zmin     min Z.
  //! \param[in] nx       num. of cells along OX axis.
  //! \param[in] ny       num. of cells along OY axis.
  //! \param[in] nz       num. of cells along OZ axis.
  //! \param[in] cellSize cell size.
  asiAlgo_UniformGrid(const T   xmin,
                      const T   ymin,
                      const T   zmin,
                      const int nx,
                      const int ny,
                      const int nz,
                      const T   cellSize)
  {
    XMin     = xmin;
    YMin     = ymin;
    ZMin     = zmin;
    Nx       = nx;
    Ny       = ny;
    Nz       = nz;
    CellSize = cellSize;

    std::cout << "Num. of cells along OX: " << nx << std::endl;
    std::cout << "Num. of cells along OY: " << ny << std::endl;
    std::cout << "Num. of cells along OZ: " << nz << std::endl;
    std::cout << "Memory threshold: " << uint64_t( sizeof(T)*(nx + 1)*(ny + 1)*(nz + 1) ) / (1024.*1024.) << "MiB" << std::endl;

    pArray = new T**[nx + 1];
    //
    for ( int i = 0; i <= nx; ++i )
    {
      pArray[i] = new T*[ny + 1];
      for ( int j = 0; j <= ny; ++j )
      {
        pArray[i][j] = new T[nz + 1];
      }
    }
  }

  //! Dtor.
  ~asiAlgo_UniformGrid()
  {
    for ( int i = 0; i <= Nx; ++i )
    {
      for ( int j = 0; j <= Ny; ++j )
      {
        delete[] pArray[i][j];
      }
      delete[] pArray[i];
    }
    delete[] pArray;
  }

public:

  /* Min. corner. */
  T XMin, YMin, ZMin;

  /* Number of cells in each direction. */
  int Nx, Ny, Nz;

  /* Cell size. */
  T CellSize;

  /* Array of scalars. */
  T*** pArray;
};

#endif
