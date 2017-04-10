//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
