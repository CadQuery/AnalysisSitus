//-----------------------------------------------------------------------------
// Created on: 15 April 2016
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
#include <asiAlgo_CommonCloud.h>

// OCCT includes
#include <gp_Pnt2d.hxx>

//-----------------------------------------------------------------------------

//! Removes coincident points from the given point cloud.
//! \param pts  [in] point cloud to affect.
//! \param prec [in] precision.
//! \return purified point cloud.
Handle(asiAlgo_CommonCloud2d)
  asiAlgo_CommonCloud2d::RemoveCoincidentPoints(const Handle(asiAlgo_CommonCloud2d)& pts,
                                                const double                         prec)
{
  Handle(asiAlgo_CommonCloud2d) purified = pts->EmptyCopy();
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

bool asiAlgo_CommonCloud2d::Read(const TCollection_AsciiString&   filename,
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

bool asiAlgo_CommonCloud2d::Write(const Handle(TColgp_HSequenceOfPnt2d)& points,
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
