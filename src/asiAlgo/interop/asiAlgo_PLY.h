//-----------------------------------------------------------------------------
// Created on: 25 November 2015
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
#include <Mesh.h>

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
