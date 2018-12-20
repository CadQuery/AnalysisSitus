//-----------------------------------------------------------------------------
// Created on: 15 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiAlgo_PointWithAttr.h>

// OCCT includes
#include <gp_Pnt2d.hxx>

//-----------------------------------------------------------------------------

//! Removes coincident points from the given point cloud.
//! \param pts  [in] point cloud to affect.
//! \param prec [in] precision.
//! \return purified point cloud.
Handle(asiAlgo_PointWithAttr2dCloud)
  asiAlgo_PointWithAttr2dCloud::RemoveCoincidentPoints(const Handle(asiAlgo_PointWithAttr2dCloud)& pts,
                                                       const double                                prec)
{
  Handle(asiAlgo_PointWithAttr2dCloud) purified = pts->EmptyCopy();
  int ip = 1;
  do
  {
    const gp_XY& p = pts->Value(ip);
    //
    purified->AddCoord(p);

    // Traverse till the end skipping all coincident points
    int jp = ip + 1;
    gp_XY p_next = p;
    for ( ; ; )
    {
      ip = jp;
      //
      if ( jp > pts->Length() )
        break;

      p_next = pts->Value(jp++);
      //
      if ( (p_next - p).Modulus() > prec )
        break;
    }
  }
  while ( ip <= pts->Length() );

  // Override the passed list
  return purified;
}

//-----------------------------------------------------------------------------
// Function: Read
//-----------------------------------------------------------------------------

bool asiAlgo_PointWithAttr2dCloud::Read(const TCollection_AsciiString&   filename,
                                        Handle(TColgp_HSequenceOfPnt2d)& points)
{
  std::ifstream FILE( filename.ToCString() );
  if ( !FILE.is_open() )
  {
    std::cout << "Error: cannot open file for reading" << std::endl;
    return false;
  }

  // Read points
  Handle(TColgp_HSequenceOfPnt2d) pts = new TColgp_HSequenceOfPnt2d;
  while ( !FILE.eof() )
  {
    char str[256];
    FILE.getline(str, 256);

    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::copy( std::istream_iterator<std::string>(iss),
               std::istream_iterator<std::string>(),
               std::back_inserter< std::vector<std::string> >(tokens) );

    if ( tokens.empty() )
      continue;

    const double x = atof(tokens[0].c_str()),
                 y = atof(tokens[1].c_str());

    pts->Append( gp_Pnt2d(x, y) );
  }

  FILE.close();
  points = pts;
  return true;
}

//-----------------------------------------------------------------------------
// Function: Write
//-----------------------------------------------------------------------------

bool asiAlgo_PointWithAttr2dCloud::Write(const Handle(TColgp_HSequenceOfPnt2d)& points,
                                         const TCollection_AsciiString&         filename)
{
  std::ofstream FILE( filename.ToCString() );
  if ( !FILE.is_open() )
  {
    std::cout << "Error: cannot open file for writing" << std::endl;
    return false;
  }

  // Write points
  for ( int p = 1; p < points->Length(); ++p )
  {
    const gp_Pnt2d& P = points->Value(p);
    FILE << P.X() << " " << P.Y() << "\n";
  }

  FILE.close();
  return true;
}
