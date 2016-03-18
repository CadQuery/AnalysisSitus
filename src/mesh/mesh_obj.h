//-----------------------------------------------------------------------------
// Created on: 14 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef mesh_obj_h
#define mesh_obj_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

// Active Data includes
#include <ActAux_Common.h>

//! Services to work with obj files.
namespace mesh_obj
{
  //! Saves the passed shape to OBJ file extracting its associated triangulation.
  //! \param theShape    [in] B-Rep to access tessellation.
  //! \param theFilename [in] target filename.
  //! \return true in case of success, false -- otherwise.
  ASitus_EXPORT bool
    Write(const TopoDS_Shape&            theShape,
          const TCollection_AsciiString& theFilename);

};

#endif
