//-----------------------------------------------------------------------------
// Created on: 15 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_common_point_cloud_2d.h>

// OCCT includes
#include <gp_Pnt2d.hxx>

//-----------------------------------------------------------------------------

//! Removes coincident points from the given point cloud.
//! \param pts  [in] point cloud to affect.
//! \param prec [in] precision.
//! \return purified point cloud.
Handle(geom_common_point_cloud_2d)
  geom_common_point_cloud_2d::RemoveCoincidentPoints(const Handle(geom_common_point_cloud_2d)& pts,
                                                     const double                              prec)
{
  Handle(geom_common_point_cloud_2d) purified = pts->EmptyCopy();
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

bool geom_common_point_cloud_2d::Read(const TCollection_AsciiString&   filename,
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

bool geom_common_point_cloud_2d::Write(const Handle(TColgp_HSequenceOfPnt2d)& points,
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
