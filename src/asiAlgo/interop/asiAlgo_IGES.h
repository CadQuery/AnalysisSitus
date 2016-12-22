//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_IGES_h
#define asiAlgo_IGES_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

//! IGES interoperability tool.
namespace asiAlgo_IGES
{
  asiAlgo_EXPORT bool
    Read(const TCollection_AsciiString& filename,
         const bool                     doHealing,
         TopoDS_Shape&                  result);
};

#endif
