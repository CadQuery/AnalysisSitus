//-----------------------------------------------------------------------------
// Created on: 10 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_InvertFaces.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param aag [in] AAG instance of the part to modify.
asiAlgo_InvertFaces::asiAlgo_InvertFaces(const Handle(asiAlgo_AAG)& aag)
{
  m_aag = aag;
}

//-----------------------------------------------------------------------------

//! Performs face inversion.
//! \param faceIds [in] IDs of the faces to invert.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_InvertFaces::Perform(const TColStd_PackedMapOfInteger& faceIds)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;

  // TODO: NYI

  return true; // Success
}
