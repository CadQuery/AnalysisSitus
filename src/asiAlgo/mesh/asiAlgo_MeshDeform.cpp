//-----------------------------------------------------------------------------
// Created on: 17 April 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiAlgo_MeshDeform.h>

// OCCT includes
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>

//-----------------------------------------------------------------------------

double calcNewCoordRand(double fi, double ratio, double origin, double L)
{
  if ( ratio == 0. )
    return fi;

  const double position = fi - origin;
  const double coeff    = Abs(ratio * position) / L;

  const double finalCoord = position*(1. + coeff) + origin;
  return finalCoord;
}

//-----------------------------------------------------------------------------

void DeformCoord(TColgp_Array1OfPnt&    input,
                 const TopLoc_Location& location,
                 const double           ratioX,
                 const double           ratioY,
                 const double           ratioZ,
                 const double           Ox,
                 const double           Oy,
                 const double           Oz,
                 const double           Lx,
                 const double           Ly,
                 const double           Lz)
{
  for ( int i = 1; i <= input.Size(); ++i )
  {
    const gp_Pnt& oldP = input[i];
    gp_Pnt        newP = oldP.Transformed(location);

    const double newX = calcNewCoordRand(newP.Coord(1), ratioX, Ox, Lx);
    const double newY = calcNewCoordRand(newP.Coord(2), ratioY, Oy, Ly);
    const double newZ = calcNewCoordRand(newP.Coord(3), ratioZ, Oz, Lz);

    newP.SetX(newX);
    newP.SetY(newY);
    newP.SetZ(newZ);
    newP.Transform( location.Inverted() );

    input.SetValue(i, newP);
  }
}

//-----------------------------------------------------------------------------

asiAlgo_MeshDeform::asiAlgo_MeshDeform(const double         corners[6],
                                       const double         ratioX,
                                       const double         ratioY,
                                       const double         ratioZ,
                                       ActAPI_ProgressEntry progress,
                                       ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  srand( time(NULL) );

  for (int i = 0; i < 6; ++i)
    m_corners[i] = corners[i];

  m_ratioX = ratioX;
  m_ratioY = ratioY;
  m_ratioZ = ratioZ;

  Lx = m_corners[3] - m_corners[0];
  Ly = m_corners[4] - m_corners[1];
  Lz = m_corners[5] - m_corners[2];

  Ox = (double(std::rand() % 100) / 100.) * Lx + m_corners[0];
  Oy = (double(std::rand() % 100) / 100.) * Ly + m_corners[1];
  Oz = (double(std::rand() % 100) / 100.) * Lz + m_corners[2];
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshDeform::Perform(Handle(Poly_Triangulation)& mesh,
                                 const TopLoc_Location&      loc)

{
  m_input = mesh;
  m_loc = loc;

  if ( m_input.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Input mesh is null.");
    return false;
  }

  m_result = m_input->Copy();
  TColgp_Array1OfPnt& nodes = m_result->ChangeNodes();

  DeformCoord(nodes, m_loc, m_ratioX, m_ratioY, m_ratioZ, Ox, Oy, Oz, Lx, Ly, Lz);

  return true;
}
