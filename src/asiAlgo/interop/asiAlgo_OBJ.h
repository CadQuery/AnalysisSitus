//-----------------------------------------------------------------------------
// Created on: 14 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_OBJ_h
#define asiAlgo_OBJ_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

// Active Data includes
#include <ActAux_Common.h>

//! Services to work with obj files.
namespace asiAlgo_OBJ
{
  //! Saves the passed shape to OBJ file extracting its associated triangulation.
  //! \param theShape    [in] B-Rep to access tessellation.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Write(const TopoDS_Shape&            theShape,
          const TCollection_AsciiString& theFilename);

};

#endif
