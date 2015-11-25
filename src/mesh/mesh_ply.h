//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
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
  ASitus_EXPORT bool
    Read(const TCollection_AsciiString&     theFilename,
         Handle(OMFDS_Mesh)&                theMesh,
         NCollection_Sequence<TNamedArray>& theNodeArrays,
         NCollection_Sequence<TNamedArray>& theElemArrays);

};

#endif
