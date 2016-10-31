//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PLY.h>

// STL includes
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>

// Active Data (mesh) includes
#include <Mesh_ElementsIterator.h>
#include <Mesh_Node.h>
#include <Mesh_Quadrangle.h>
#include <Mesh_Triangle.h>

//-----------------------------------------------------------------------------

bool asiAlgo_PLY::Read(const TCollection_AsciiString&     theFilename,
                       Handle(Mesh)&                      theMesh,
                       NCollection_Sequence<TNamedArray>& theNodeArrays,
                       NCollection_Sequence<TNamedArray>& theElemArrays)
    {
  std::ifstream FILE( theFilename.ToCString() );
  if ( !FILE.is_open() )
  {
    return false;
  }

  // Create container for mesh
  Handle(Mesh) MeshDS = new Mesh;

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

        const int id = MeshDS->AddNode(xyz[0], xyz[1], xyz[2]);

        for ( int k = 3; k < tokens.size(); ++k )
        {
          const double val = atof( tokens[k].c_str() );
          theNodeArrays(k - 2).Data->SetValue( id - 1, val );
        }

        if ( MeshDS->NbNodes() == nVertices )
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

          elem_id = MeshDS->AddFace(nodes, 3);
        }
        else if ( nNodes == 4 )
        {
          int nodes[4] = { atoi( tokens[1].c_str() ) + 1,
                           atoi( tokens[2].c_str() ) + 1,
                           atoi( tokens[3].c_str() ) + 1,
                           atoi( tokens[4].c_str() ) + 1 };

          elem_id = MeshDS->AddFace(nodes, 4);
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
  theMesh = MeshDS;
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PLY::Write(const Handle(Mesh)&            theMesh,
                        const TCollection_AsciiString& theFilename)
{
  std::ofstream FILE( theFilename.ToCString() );
  if ( !FILE.is_open() )
  {
    std::cout << "Error: cannot open file for writing" << std::endl;
    return false;
  }

  // Write header
  FILE << "ply\n";
  FILE << "format ascii 1.0\n";
  FILE << "comment author: Analysis Situs\n";
  FILE << "element vertex " << theMesh->NbNodes() << "\n";
  //
  FILE << "property double x\n";
  FILE << "property double y\n";
  FILE << "property double z\n";
  //
  FILE << "element face " << theMesh->NbFaces() << "\n";
  FILE << "property list uchar uint vertex_indices\n";
  FILE << "end_header\n";
  FILE.close();

  bool isOk = _writeNodes(theMesh, theFilename);
  isOk = isOk && _writeElements(theMesh, 0, theFilename);
  return isOk;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PLY::_writeNodes(const Handle(Mesh)&            theMesh,
                              const TCollection_AsciiString& theFilename)
{
  std::ofstream FILE(theFilename.ToCString(), std::ofstream::app);
  if ( !FILE.is_open() )
  {
    std::cout << "Error: cannot open file for writing" << std::endl;
    return false;
  }

  // Write nodes
  for ( int node_idx = 1; node_idx <= theMesh->NbNodes(); ++node_idx )
  {
    const gp_Pnt& P = theMesh->FindNode(node_idx)->Pnt();
    FILE << P.X() << " " << P.Y() << " " << P.Z();

    FILE << "\n";
  }

  FILE.close();
  return true;
}

bool asiAlgo_PLY::_writeElements(const Handle(Mesh)&            theMesh,
                              const int                      theShift,
                              const TCollection_AsciiString& theFilename)
{
  std::ofstream FILE(theFilename.ToCString(), std::ofstream::app);
  if ( !FILE.is_open() )
  {
    std::cout << "Error: cannot open file for writing" << std::endl;
    return false;
  }

  // Loop over the mesh elements
  for ( Mesh_ElementsIterator it(theMesh, Mesh_ET_Face); it.More(); it.Next() )
  {
    const Handle(Mesh_Element)& E = it.GetValue();

    // Get node IDs resolving the actual element's type
    int node_idx[4] = {0, 0, 0, 0};
    int nNodes      = 0;
    //
    if ( E->IsInstance( STANDARD_TYPE(Mesh_Triangle) ) )
    {
      Handle(Mesh_Triangle) TE = Handle(Mesh_Triangle)::DownCast(E);
      //
      TE->GetFaceDefinedByNodes(3, node_idx, nNodes);
    }
    else if ( E->IsInstance( STANDARD_TYPE(Mesh_Quadrangle) ) )
    {
      Handle(Mesh_Quadrangle) QE = Handle(Mesh_Quadrangle)::DownCast(E);
      //
      QE->GetFaceDefinedByNodes(4, node_idx, nNodes);
    }
    else
      continue;

    // Write elements
    FILE << nNodes << " ";
    for ( int k = 0; k < nNodes; ++k )
    {
      FILE << node_idx[k] - 1 + theShift;
      if ( k < nNodes - 1 )
        FILE << " ";
    }
    FILE << "\n";
  }

  FILE.close();
  return true;
}
