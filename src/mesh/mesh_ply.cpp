//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <mesh_ply.h>

// STL includes
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>

bool mesh_ply::Read(const TCollection_AsciiString&     theFilename,
                    Handle(OMFDS_Mesh)&                theMesh,
                    NCollection_Sequence<TNamedArray>& theNodeArrays,
                    NCollection_Sequence<TNamedArray>& theElemArrays)
{
  std::ifstream FILE( theFilename.ToCString() );
  if ( !FILE.is_open() )
  {
    return false;
  }

  // Create container for mesh
  Handle(OMFDS_Mesh) Mesh = new OMFDS_Mesh;

  // Working variables
  int nVertices = 0, nFaces = 0;
  bool isHeaderPassed    = false,
       isHeaderNodes     = false,
       isHeaderFaces     = false,
       areVerticesPassed = false,
       areFacesPassed    = false;

  // Loop over the file
  while ( !FILE.eof() )
  {
    char str[1024];
    FILE.getline(str, 1024);

    // Save tokens to vector
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::copy( std::istream_iterator<std::string>(iss),
               std::istream_iterator<std::string>(),
               std::back_inserter< std::vector<std::string> >(tokens) );

    if ( !tokens.size() )
      continue;

    // Skip header
    if ( !isHeaderPassed && (tokens[0] == "end_header") )
    {
      isHeaderPassed = true;
      continue;
    }
    else if ( !isHeaderPassed )
    {
      // ...
      // Process header
      // ...

      if ( tokens.size() >= 3 )
      {
        int* iTarget = NULL;
        if ( tokens[1] == "vertex" )
        {
          iTarget       = &nVertices;
          isHeaderNodes = true;
        }
        else if ( tokens[1] == "face" )
        {
          isHeaderNodes = false;
          isHeaderFaces = true;
          iTarget       = &nFaces;
        }

        if ( isHeaderNodes && tokens[0] == "property" && tokens[2] != "x" && tokens[2] != "y" && tokens[2] != "z" )
        {
          TNamedArray narr;
          narr.Name = tokens[2].c_str();
          narr.Data = new HRealArray(0, nVertices - 1, 0.0);
          theNodeArrays.Append(narr);
        }
        else if ( isHeaderFaces && tokens[0] == "property" && tokens.size() == 3 )
        {
          TNamedArray narr;
          narr.Name = tokens[2].c_str();
          narr.Data = new HRealArray(0, nFaces - 1, 0.0);
          theElemArrays.Append(narr);
        }

        if ( iTarget )
        {
          *iTarget = atoi( tokens[2].c_str() );
          iTarget = NULL;
        }
      }
    }
    else if ( isHeaderPassed )
    {
      // ...
      // Process vertices
      // ...

      if ( !areVerticesPassed && tokens.size() >= 3 )
      {
        const double xyz[3] = { atof( tokens[0].c_str() ),
                                atof( tokens[1].c_str() ),
                                atof( tokens[2].c_str() ) };

        const int id = Mesh->AddNode(xyz[0], xyz[1], xyz[2]);

        for ( int k = 3; k < tokens.size(); ++k )
        {
          const double val = atof( tokens[k].c_str() );
          theNodeArrays(k - 2).Data->SetValue( id - 1, val );
        }

        if ( Mesh->NbNodes() == nVertices )
        {
          areVerticesPassed = true;
          continue;
        }
      }

      // ...
      // Process faces
      // ...

      if ( areVerticesPassed && !areFacesPassed && tokens.size() > 3 )
      {
        int nNodes = atoi( tokens[0].c_str() );

        int elem_id = -1;
        if ( nNodes == 3 )
        {
          int nodes[3] = { atoi( tokens[1].c_str() ) + 1,
                           atoi( tokens[2].c_str() ) + 1,
                           atoi( tokens[3].c_str() ) + 1 };

          elem_id = Mesh->AddFace(nodes, 3);
        }
        else if ( nNodes == 4 )
        {
          int nodes[4] = { atoi( tokens[1].c_str() ) + 1,
                           atoi( tokens[2].c_str() ) + 1,
                           atoi( tokens[3].c_str() ) + 1,
                           atoi( tokens[4].c_str() ) + 1 };

          elem_id = Mesh->AddFace(nodes, 4);
        }

        for ( int k = nNodes + 1; k < tokens.size(); ++k )
        {
          const double val = atof( tokens[k].c_str() );
          theElemArrays(k - nNodes).Data->SetValue( elem_id - 1, val );
        }
      }
    }
  } // Until EOF

  FILE.close();
  theMesh = Mesh;
  return true;
}
