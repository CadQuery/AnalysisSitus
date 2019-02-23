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

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>

// Active Data includes
#include <ActAux_Common.h>

// Mesh includes
#include <ActData_Mesh.h>

//-----------------------------------------------------------------------------

//! Services to work with ply files.
namespace asiAlgo_PLY
{
  struct TNamedArray
  {
    TCollection_AsciiString Name; //!< Name of data series.
    Handle(HRealArray)      Data; //!< Data array.
  };

  //! Reads mesh from the given file.
  //! \param theFilename   [in]  target filename.
  //! \param theMesh       [out] restored tessellation.
  //! \param theNodeArrays [out] collection of data arrays associated with nodes.
  //! \param theElemArrays [out] collection of data arrays associated with faces.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Read(const TCollection_AsciiString&     theFilename,
         Handle(ActData_Mesh)&              theMesh,
         NCollection_Sequence<TNamedArray>& theNodeArrays,
         NCollection_Sequence<TNamedArray>& theElemArrays);

  //! Saves the passed mesh to a ply file.
  //! \param theMesh     [in] tessellation to store.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const Handle(ActData_Mesh)&    theMesh,
          const TCollection_AsciiString& theFilename);

//-----------------------------------------------------------------------------

  //! Saves the nodes only from the passed tessellation into a ply file.
  //! \param theMesh     [in] mesh to save.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    _writeNodes(const Handle(ActData_Mesh)&    theMesh,
                const TCollection_AsciiString& theFilename);

  //! Save the elements only from the passed tessellation into a ply file.
  //! \param theMesh     [in] mesh to save.
  //! \param theShift    [in] number of nodes already written.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    _writeElements(const Handle(ActData_Mesh)&    theMesh,
                   const int                      theShift,
                   const TCollection_AsciiString& theFilename);

};

#endif
