//-----------------------------------------------------------------------------
// Created on: 25 November 2015
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

// Own include
#include <asiAlgo_PLY.h>

// STL includes
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>

// Active Data (mesh) includes
#include <ActData_Mesh_ElementsIterator.h>
#include <ActData_Mesh_Node.h>
#include <ActData_Mesh_Quadrangle.h>
#include <ActData_Mesh_Triangle.h>

//-----------------------------------------------------------------------------

//! RAII handler for reading PLY files.
class asiAlgp_InPLYFile
{
public:

  //! Ctor accepting filename. This object opens the file.
  //! \param[in] filename target filename.
  asiAlgp_InPLYFile(const TCollection_AsciiString& filename)
  {
    m_FILE.open( filename.ToCString() );
  }

  //! Dtor.
  ~asiAlgp_InPLYFile()
  {
    m_FILE.close();
  }

public:

  //! \return true if the file is open.
  bool IsOpen() const
  {
    return m_FILE.is_open();
  }

  //! \return file resource.
  std::ifstream& FILE()
  {
    return m_FILE;
  }

protected:

  std::ifstream m_FILE; //!< File to read.

};

//-----------------------------------------------------------------------------

//! RAII handler for writing PLY files.
class asiAlgp_OutPLYFile
{
public:

  //! Ctor accepting filename. This object opens the file.
  //! \param[in] filename target filename.
  asiAlgp_OutPLYFile(const TCollection_AsciiString& filename)
  {
    m_FILE.open(filename.ToCString(), std::ios::out | std::ios::trunc);
  }

  //! Dtor.
  ~asiAlgp_OutPLYFile()
  {
    m_FILE.close();
  }

public:

  //! \return true if the file is open.
  bool IsOpen() const
  {
    return m_FILE.is_open();
  }

  //! \return file resource.
  std::ofstream& FILE()
  {
    return m_FILE;
  }

protected:

  std::ofstream m_FILE; //!< File to write.

};

//-----------------------------------------------------------------------------

bool asiAlgo_PLY::Read(const TCollection_AsciiString&     filename,
                       Handle(ActData_Mesh)&              mesh,
                       NCollection_Sequence<TNamedArray>& nodeArrays,
                       NCollection_Sequence<TNamedArray>& elemArrays)
{
  asiAlgp_InPLYFile FILE(filename);
  //
  if ( !FILE.IsOpen() )
    return false;

  // Create container for mesh.
  Handle(ActData_Mesh) MeshDS = new ActData_Mesh;

  // Working variables.
  int nVertices = 0, nFaces = 0;
  bool isHeaderPassed    = false,
       isHeaderNodes     = false,
       isHeaderFaces     = false,
       areVerticesPassed = false,
       areFacesPassed    = false;

  // Loop over the file.
  while ( !FILE.FILE().eof() )
  {
    char str[1024];
    FILE.FILE().getline(str, 1024);

    // Save tokens to vector.
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::copy( std::istream_iterator<std::string>(iss),
               std::istream_iterator<std::string>(),
               std::back_inserter< std::vector<std::string> >(tokens) );

    if ( !tokens.size() )
      continue;

    // Skip header.
    if ( !isHeaderPassed && (tokens[0] == "end_header") )
    {
      isHeaderPassed = true;
      continue;
    }
    else if ( !isHeaderPassed )
    {
      // ...
      // Process header.
      // ...

      if ( tokens.size() >= 3 )
      {
        int* iTarget = nullptr;
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
          nodeArrays.Append(narr);
        }
        else if ( isHeaderFaces && tokens[0] == "property" && tokens.size() == 3 )
        {
          TNamedArray narr;
          narr.Name = tokens[2].c_str();
          narr.Data = new HRealArray(0, nFaces - 1, 0.0);
          elemArrays.Append(narr);
        }

        if ( iTarget )
        {
          *iTarget = atoi( tokens[2].c_str() );
          iTarget = nullptr;
        }
      }
    }
    else if ( isHeaderPassed )
    {
      // ...
      // Process vertices.
      // ...

      if ( !areVerticesPassed && tokens.size() >= 3 )
      {
        const double xyz[3] = { atof( tokens[0].c_str() ),
                                atof( tokens[1].c_str() ),
                                atof( tokens[2].c_str() ) };

        const int id = MeshDS->AddNode(xyz[0], xyz[1], xyz[2]);

        for ( int k = 3; k < int( tokens.size() ); ++k )
        {
          const double val = atof( tokens[k].c_str() );
          nodeArrays(k - 2).Data->SetValue( id - 1, val );
        }

        if ( MeshDS->NbNodes() == nVertices )
        {
          areVerticesPassed = true;
          continue;
        }
      }

      // ...
      // Process faces.
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

        for ( int k = nNodes + 1; k < int ( tokens.size() ); ++k )
        {
          const double val = atof( tokens[k].c_str() );
          elemArrays(k - nNodes).Data->SetValue( elem_id - 1, val );
        }
      }
    }
  } // Until EOF.

  mesh = MeshDS;
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PLY::Write(const Handle(ActData_Mesh)&    mesh,
                        const TCollection_AsciiString& filename)
{
  asiAlgp_OutPLYFile FILE(filename);
  //
  if ( !FILE.IsOpen() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot open file for writing.");
    return false;
  }

  this->writeHeader   (mesh->NbNodes(), mesh->NbFaces(), FILE);
  this->writeNodes    (mesh, FILE);
  this->writeElements (mesh, 0, FILE);

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_PLY::Write(const Handle(Poly_Triangulation)& tris,
                        const TCollection_AsciiString&    filename)
{
  asiAlgp_OutPLYFile FILE(filename);
  //
  if ( !FILE.IsOpen() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot open file for writing.");
    return false;
  }

  this->writeHeader   (tris->NbNodes(), tris->NbTriangles(), FILE);
  this->writeNodes    (tris, FILE);
  this->writeElements (tris, 0, FILE);

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_PLY::writeHeader(const int           numNodes,
                              const int           numFaces,
                              asiAlgp_OutPLYFile& FILE)
{
  // Write header.
  FILE.FILE() << "ply\n";
  FILE.FILE() << "format ascii 1.0\n";
  FILE.FILE() << "comment author: Analysis Situs\n";
  FILE.FILE() << "element vertex " << numNodes << "\n";
  //
  FILE.FILE() << "property double x\n";
  FILE.FILE() << "property double y\n";
  FILE.FILE() << "property double z\n";
  //
  FILE.FILE() << "element face " << numFaces << "\n";
  FILE.FILE() << "property list uchar uint vertex_indices\n";
  FILE.FILE() << "end_header\n";
}

//-----------------------------------------------------------------------------

void asiAlgo_PLY::writeNodes(const Handle(ActData_Mesh)& mesh,
                             asiAlgp_OutPLYFile&         FILE)
{
  for ( int node_idx = 1; node_idx <= mesh->NbNodes(); ++node_idx )
  {
    const gp_Pnt& P = mesh->FindNode(node_idx)->Pnt();
    FILE.FILE() << P.X() << " " << P.Y() << " " << P.Z();
    FILE.FILE() << "\n";
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_PLY::writeNodes(const Handle(Poly_Triangulation)& tris,
                             asiAlgp_OutPLYFile&               FILE)
{
  for ( int node_idx = 1; node_idx <= tris->NbNodes(); ++node_idx )
  {
    const gp_Pnt& P = tris->Nodes()(node_idx);
    FILE.FILE() << P.X() << " " << P.Y() << " " << P.Z();
    FILE.FILE() << "\n";
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_PLY::writeElements(const Handle(ActData_Mesh)& mesh,
                                const int                   shift,
                                asiAlgp_OutPLYFile&         FILE)
{
  // Loop over the mesh elements.
  for ( ActData_Mesh_ElementsIterator it(mesh, ActData_Mesh_ET_Face); it.More(); it.Next() )
  {
    const Handle(ActData_Mesh_Element)& E = it.GetValue();

    // Get node IDs resolving the actual element's type.
    int node_idx[4] = {0, 0, 0, 0};
    int nNodes      = 0;
    //
    if ( E->IsInstance( STANDARD_TYPE(ActData_Mesh_Triangle) ) )
    {
      Handle(ActData_Mesh_Triangle) TE = Handle(ActData_Mesh_Triangle)::DownCast(E);
      //
      TE->GetFaceDefinedByNodes(3, node_idx, nNodes);
    }
    else if ( E->IsInstance( STANDARD_TYPE(ActData_Mesh_Quadrangle) ) )
    {
      Handle(ActData_Mesh_Quadrangle) QE = Handle(ActData_Mesh_Quadrangle)::DownCast(E);
      //
      QE->GetFaceDefinedByNodes(4, node_idx, nNodes);
    }
    else
      continue;

    // Write elements.
    FILE.FILE() << nNodes << " ";
    for ( int k = 0; k < nNodes; ++k )
    {
      FILE.FILE() << node_idx[k] - 1 + shift;
      if ( k < nNodes - 1 )
        FILE.FILE() << " ";
    }
    FILE.FILE() << "\n";
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_PLY::writeElements(const Handle(Poly_Triangulation)& tris,
                                const int                         shift,
                                asiAlgp_OutPLYFile&               FILE)
{
  const Poly_Array1OfTriangle& trisArr = tris->Triangles();
  //
  for ( int i = 1; i <= trisArr.Length(); ++i )
  {
    const Poly_Triangle& tri = trisArr(i);

    // Get node IDs.
    int node_idx[4] = {0, 0, 0, 0};
    tri.Get(node_idx[0], node_idx[1], node_idx[2]);

    // Write elements.
    FILE.FILE() << 3 << " ";
    for ( int k = 0; k < 3; ++k )
    {
      FILE.FILE() << node_idx[k] - 1 + shift;
      if ( k < 2 )
        FILE.FILE() << " ";
    }
    FILE.FILE() << "\n";
  }
}
