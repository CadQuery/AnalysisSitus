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

#ifndef asiAlgo_PLY_h
#define asiAlgo_PLY_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// Mesh includes
#include <ActData_Mesh.h>

//-----------------------------------------------------------------------------

class asiAlgp_OutPLYFile;

//! Services to work with ply files.
class asiAlgo_PLY : public ActAPI_IAlgorithm
{
public:

  //! Auxiliary data structure to exchange named arrays.
  struct TNamedArray
  {
    TCollection_AsciiString Name; //!< Name of data series.
    Handle(HRealArray)      Data; //!< Data array.
  };

public:

  //! Ctor accepting progress notifier and imperative plotter.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_PLY(ActAPI_ProgressEntry progress = NULL,
              ActAPI_PlotterEntry  plotter  = NULL) : ActAPI_IAlgorithm(progress, plotter)
  {}

public:

  //! Reads mesh from the given file.
  //! \param[in]  filename   target filename.
  //! \param[out] mesh       restored tessellation.
  //! \param[out] nodeArrays collection of data arrays associated with nodes.
  //! \param[out] elemArrays collection of data arrays associated with faces.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Read(const TCollection_AsciiString&     filename,
         Handle(ActData_Mesh)&              mesh,
         NCollection_Sequence<TNamedArray>& nodeArrays,
         NCollection_Sequence<TNamedArray>& elemArrays);

  //! Saves the passed mesh to a ply file.
  //! \param[in] mesh     tessellation to store.
  //! \param[in] filename target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const Handle(ActData_Mesh)&    mesh,
          const TCollection_AsciiString& filename);

  //! Saves the passed triangulation to a ply file.
  //! \param[in] tris     triangulation to store.
  //! \param[in] filename target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const Handle(Poly_Triangulation)& tris,
          const TCollection_AsciiString&    filename);

protected:

  //! Writes header info to the file.
  //! \param[in]     numNodes number of nodes in a mesh.
  //! \param[in]     numFaces number of faces in a mesh.
  //! \param[in,out] FILE     file resource.
  asiAlgo_EXPORT void
    writeHeader(const int           numNodes,
                const int           numFaces,
                asiAlgp_OutPLYFile& FILE);

  //! Saves the nodes only from the passed tessellation into a ply file.
  //! \param[in]     mesh mesh to save.
  //! \param[in,out] FILE file resource.
  asiAlgo_EXPORT void
    writeNodes(const Handle(ActData_Mesh)& mesh,
               asiAlgp_OutPLYFile&         FILE);

  //! Saves the nodes only from the passed triangulation into a ply file.
  //! \param[in]     tris triangulation to save.
  //! \param[in,out] FILE file resource.
  asiAlgo_EXPORT void
    writeNodes(const Handle(Poly_Triangulation)& tris,
               asiAlgp_OutPLYFile&               FILE);

  //! Save the elements only from the passed tessellation into a ply file.
  //! \param[in]     mesh  mesh to save.
  //! \param[in]     shift number of nodes already written.
  //! \param[in,out] FILE  file resource.
  asiAlgo_EXPORT void
    writeElements(const Handle(ActData_Mesh)& mesh,
                  const int                   shift,
                  asiAlgp_OutPLYFile&         FILE);

  //! Save the elements only from the passed triangulation into a ply file.
  //! \param[in]     tris  triangulation to save.
  //! \param[in]     shift number of nodes already written.
  //! \param[in,out] FILE  file resource.
  asiAlgo_EXPORT void
    writeElements(const Handle(Poly_Triangulation)& tris,
                  const int                         shift,
                  asiAlgp_OutPLYFile&               FILE);
};

#endif
