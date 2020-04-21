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

#ifndef asiAlgo_WriteREK_h
#define asiAlgo_WriteREK_h

// asiAlgo includes
#include <asiAlgo_UniformGrid.h>

// Standard includes
#include <string>
#include <ostream>

//! Writes the voxelization in Fraunhofer Raw Volumes file format (REK).
class asiAlgo_WriteREK
{
public:

  //! RAII ctor.
  //! \param[in] filename   input file.
  //! \param[in] scaleCoeff floating-point coefficient to multiply the
  //!                       raw data values by. Use this to perform
  //!                       unit conversion. E.g., if the initial CAD data
  //!                       is in millimeters, passing 1000 here would mean
  //!                       conversion to microns.
  asiAlgo_EXPORT
    asiAlgo_WriteREK(const std::string& filename,
                     const float        scaleCoeff = 1.0);

  //! Dtor.
  asiAlgo_EXPORT
    ~asiAlgo_WriteREK();

public:

  struct t_header
  {
    uint16_t SizeX;       //!< Size X in pixels.
    uint16_t SizeY;       //!< Size Y in pixels.
    uint16_t Pixel;       //!< 16 for WORD | 32 for FLOAT.
    uint16_t SizeZ;       //!< Number of 2D volume slices.
    char     Res1[572];   //!< Reserved.
    float    SomeValue;   //!< Reserved.
    float    PixelSize;   //!< Voxel size (in microns).
    float    SliceDist;   //!< Slice step (in microns).
    char     Res2[1456];  //!< Reserved.

    //! Default ctor.
    t_header() : SizeX(0),
                 SizeY(0),
                 Pixel(32),
                 SizeZ(0),
                 SomeValue(0.),
                 PixelSize(0.),
                 SliceDist(0.)
    {
      memset(&Res1, 0, sizeof(Res1));
      memset(&Res2, 0, sizeof(Res2));
    }
  };

public:

  //! Writes the passed uniform grid of 32-bit floating-point values to
  //! the REK file.
  //! \param[in] grid the grid to serialize.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const Handle(asiAlgo_UniformGrid<float>)& grid);

protected:

  std::ofstream* m_pFILE;       //!< File handle.
  float          m_fScaleCoeff; //!< Raw data scaling coefficient.

};

#endif
