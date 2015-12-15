//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef mesh_ply_h
#define mesh_ply_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>

// Active Data includes
#include <ActAux_Common.h>

// OMFDS includes
#include <OMFDS_Mesh.hxx>

//! Services to work with ply files.
namespace mesh_ply
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
   bool
    Read(const TCollection_AsciiString&     theFilename,
         Handle(OMFDS_Mesh)&                theMesh,
         NCollection_Sequence<TNamedArray>& theNodeArrays,
         NCollection_Sequence<TNamedArray>& theElemArrays);

  //! Saves the passed mesh to a ply file.
  //! \param theMesh     [in] tessellation to store.
  //! \param theFilename [in]  target filename.
  //! \return true in case of success, false -- otherwise.
  bool
    Write(const Handle(OMFDS_Mesh)&      theMesh,
          const TCollection_AsciiString& theFilename);

//-----------------------------------------------------------------------------

  //! Saves the nodes only from the passed tessellation into a ply file.
  //! \param theMesh     [in] mesh to save.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  bool
    _writeNodes(const Handle(OMFDS_Mesh)&      theMesh,
                const TCollection_AsciiString& theFilename);

  //! Save the elements only from the passed tessellation into a ply file.
  //! \param theMesh     [in] mesh to save.
  //! \param theShift    [in] number of nodes already written.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  bool
    _writeElements(const Handle(OMFDS_Mesh)&      theMesh,
                   const int                      theShift,
                   const TCollection_AsciiString& theFilename);

};

#endif
